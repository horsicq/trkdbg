# Debugger backend analysis: edb-debugger (Unix) and x64dbg (Windows)

Reference study for improving `_mylibs/XSpecDebugger` (`XUnixDebugger` / `XLinuxDebugger` /
`XOSXDebugger` on Unix, `XWindowsDebugger` on Windows).

Sources analysed:

- **edb-debugger** — <https://github.com/eteran/edb-debugger>, `plugins/DebuggerCore/unix/**`
- **x64dbg** — <https://github.com/x64dbg/x64dbg>, `src/dbg/**` (+ vendored TitanEngine)

---

## 0. Licensing — read before copying anything

| Project | License | Can we copy source into XSpecDebugger? |
| --- | --- | --- |
| edb-debugger | **GPL-2.0** | ❌ No |
| x64dbg | **GPL-3.0** | ❌ No |
| XSpecDebugger / trkdbg | **MIT** (see file headers) | — |

Copying GPL source into an MIT-licensed codebase re-licenses the whole binary under the GPL
and violates both licenses. **Do not paste code from either project.**

What is safe and what this document does: copy the *techniques* — the sequence of `ptrace`
requests, the Windows API call order, the DR7 bit layout, the event-decoding logic. Algorithms,
syscall sequences, and hardware register layouts are facts, not copyrightable expression.
Reimplement them from scratch against our own `XInfoDB` / `XProcess` abstractions. Keep the MIT
headers; add no GPL notices.

(License facts above are from each repo's `LICENSE` at time of writing — reconfirm before you
rely on them.)

---

## 1. Both references validate our architecture

The good news: XSpecDebugger is already shaped the same way as both mature debuggers, so this is
gap-filling, not a rewrite.

- **edb Unix layering** = `Unix.cpp` / `Posix.cpp` (shared) + `linux/`, `freebsd/`, `openbsd/`,
  `osx/` subdirs. → We already have `XUnixDebugger` (shared) + `XLinuxDebugger` / `XOSXDebugger`.
- **x64dbg Windows loop** = `WaitForDebugEvent` / `ContinueDebugEvent` with per-event-code
  callbacks. → We already have `XWindowsDebugger::_debugLoop` + `on_*_DEBUG_EVENT` handlers.

So the work is: fill the `// TODO`s in our existing files using the proven recipes below.

---

## 2. Unix / Linux — techniques from edb → our gaps

### 2.1 Launching a process — `XLinuxDebugger::load()`

edb's child-side recipe (after `fork()`), in order:

1. `ptrace(PTRACE_TRACEME, 0, 0, 0)`
2. `personality(ADDR_NO_RANDOMIZE)` — disable ASLR so addresses are reproducible
3. `setenv("LD_BIND_NOW", "1")` — eager PLT binding so imports resolve before entry
4. redirect stdin/stdout/stderr (`dup2`) when running with a console
5. `execv`/`execvp` the target **with full argv**, then `abort()` on failure

Our `xlinuxdebugger.cpp` already forks + `PTRACE_TRACEME` + `executeProcess()`, but the child is
missing items 2–4, and `XUnixDebugger::executeProcess()` builds a 2-element argv that passes
**only `argv[0]`** — command-line arguments (`getOptions()->sArguments`) are dropped. Fixes:

- Add `personality(ADDR_NO_RANDOMIZE)` and `setenv("LD_BIND_NOW","1")` in the child branch
  (the code already has a `// TODO personality(ADDR_NO_RANDOMIZE);` marker at the right spot).
- Rework `executeProcess()` to split `sArguments` into a real argv vector (respect quoting).
- Honor `sDirectory` on Linux too (currently `chdir` is `#ifdef Q_OS_MAC` only).

### 2.2 Attaching — `XLinuxDebugger::attach()` (currently `return false`)

edb's recipe:

1. Enumerate threads by reading the directory `/proc/<pid>/task/` — each entry name is a TID.
2. For each TID: `ptrace(PTRACE_ATTACH, tid, 0, 0)` (edb also supports `PTRACE_SEIZE`),
   then `waitpid(tid, &status, __WALL)` to synchronize on the stop.
3. `ptrace(PTRACE_SETOPTIONS, tid, 0, options)` on each.
4. Build `PROCESS_INFO` / `THREAD_INFO` and open the memory handles exactly like `load()` does.

`PTRACE_SEIZE` is worth preferring where available: it attaches **without** an initial `SIGSTOP`
side effect and enables `PTRACE_INTERRUPT`, giving a cleaner stop/continue model.

### 2.3 ptrace options — `XUnixDebugger::setPtraceOptions()`

Currently sets `PTRACE_O_EXITKILL | PTRACE_O_TRACECLONE`. edb's superset, add as needed:

- `PTRACE_O_TRACECLONE` — deliver an event on thread creation (**essential for multithreading**)
- `PTRACE_O_EXITKILL` — kill the debuggee if the debugger dies
- `PTRACE_O_TRACEEXIT` — stop on thread/process exit (get exit code before teardown)
- `PTRACE_O_TRACESYSGOOD` — flag syscall-stops (bit 0x80 in the stop signal) if you add syscall tracing
- `PTRACE_O_TRACEEXEC` — clean event on `execve` for exec-in-place targets

The existing `// TODO getOptions !!!` should read these from `getOptions()` toggles.

### 2.4 Waiting & decoding events — `XUnixDebugger::waitForSignal()` (`// TODO Clone event`)

This is the most important Unix gap. edb decodes the 32-bit `status` from `waitpid` as:

```
if (WIFSTOPPED(status)) {
    const int sig   = WSTOPSIG(status);
    const int event = status >> 8;               // ptrace event in the high byte
    if (event == (SIGTRAP | (PTRACE_EVENT_CLONE  << 8))) { /* new thread */ }
    else if (event == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) { /* about to exit */ }
    else if (event == (SIGTRAP | (PTRACE_EVENT_EXEC << 8))) { /* execve done */ }
    // else ordinary signal-delivery-stop → sig
}
```

On a **clone** event: `ptrace(PTRACE_GETEVENTMSG, tid, 0, &newtid)` returns the new TID; register
a `THREAD_INFO`, `waitpid(newtid, …, __WALL)` it, and **copy DR0–DR7 from the parent thread** so
hardware breakpoints propagate. Our `waitForSignal()` already reads `PTRACE_GETSIGINFO` and maps
`si_code` (`TRAP_TRACE`/`TRAP_BRKPT`/`SI_KERNEL`) — good — but never inspects `status >> 8`, so
clone/exit/exec events are invisible. Add the high-byte decode and a `DEBUGGER_STATUS_*` for each.

### 2.5 Multi-thread wait model — `XUnixDebugger::_debugEvent()`

Current loop polls one PID only:

```
qint64 nId = getXInfoDB()->getProcessInfo()->nProcessID;   // main thread only
STATE state = waitForSignal(nId, __WALL | WNOHANG);
```

Under multiple threads this misses events on sibling TIDs. edb's model: after
`Posix::wait_for_sigchld()`, iterate every known thread with
`waitpid(tid, &status, __WALL | WNOHANG)` (or wait on `-1`). Adopt one of:

- `waitpid(-1, &status, __WALL | WNOHANG)` in a loop until it returns 0, dispatching each, **or**
- iterate `getXInfoDB()` thread list and `WNOHANG`-poll each TID.

### 2.6 Whole-process stop (freeze siblings)

On Linux there is no "suspend thread" primitive; edb freezes siblings by sending `SIGSTOP` via
`tgkill`: `syscall(SYS_tgkill, pid, tid, SIGSTOP)`, then reaping each stop. Wire our
`XInfoDB::suspendAllThreads()` on Linux to this `tgkill(SIGSTOP)` mechanism (resume =
`PTRACE_CONT`). This is what makes "all threads stop at a breakpoint" actually work.

### 2.7 Registers, mode, hardware breakpoints

- **GP registers**: `ptrace(PTRACE_GETREGSET/SETREGSET, tid, NT_PRSTATUS, &iovec)` (portable
  across arches; falls back to `PTRACE_GETREGS` on old kernels).
- **FPU/SSE/AVX**: `NT_X86_XSTATE` (or `NT_PRFPREG`) via the same GETREGSET mechanism.
- **Debug registers DR0–DR7**: `ptrace(PTRACE_POKEUSER, tid, offsetof(struct user, u_debugreg[n]), val)`
  and `PTRACE_PEEKUSER` to read. This is the Unix path for hardware breakpoints/watchpoints —
  currently absent from `XUnixDebugger`.
- **Arch detection**: `XLinuxDebugger::getArch()` is hardcoded `"AMD64"`. edb reads the `CS`
  segment (via `PEEKUSER`) to distinguish 32- vs 64-bit debuggees; do the same instead of
  assuming the host arch.

### 2.8 Memory access

edb reads/writes target memory through **`/proc/<pid>/mem`** with `pread`/`pwrite`, and falls
back to `PTRACE_PEEKTEXT`/`PTRACE_POKETEXT` when that fails; original bytes for software
breakpoints are stored in a patch table and spliced back out on read so the user never sees the
`0xCC`. Our `XProcess::openMemoryIO()` already abstracts this — just confirm the Linux
implementation uses `/proc/<pid>/mem` (fast) rather than word-at-a-time `PEEK/POKE`, and that BP
bytes are hidden on read-back.

---

## 3. Windows — techniques from x64dbg → our gaps

### 3.1 Run the debug loop on its own thread — `XWindowsDebugger::_debugLoop()`

x64dbg runs the whole `WaitForDebugEvent`/`ContinueDebugEvent` loop on a dedicated
`hDebugLoopThread`, because the Win32 rule is that **only the thread that started debugging may
call `WaitForDebugEvent`/`ContinueDebugEvent`**, and it blocks. Our `_debugLoop()` is a blocking
`while (isDebugActive())` invoked *directly inside* `load()`. If `load()` is called on the GUI
thread it freezes the UI, and step/continue requests from the UI thread would violate the
same-thread rule. Confirm `XDebuggerWidget` drives `XWindowsDebugger` through the
`XAbstractDebugger` `QThread` + `process()` slot (the base class already includes `<QThread>`),
and route all continue/step commands to that thread (queued signals), matching x64dbg.

### 3.2 Attach — `XWindowsDebugger::attach()` (currently `return false`)

x64dbg recipe:

1. `DebugActiveProcess(pid)` — attaches and starts delivering `DEBUG_EVENT`s.
2. `DebugSetProcessKillOnExit(FALSE)` — so detaching later leaves the target alive.
3. Identify the main thread (x64dbg uses a `CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD)` scan,
   plus `EnumWindows` to find the GUI message-pump thread).
4. Enter the **same** `_debugLoop`; the kernel synthesizes `CREATE_PROCESS`/`CREATE_THREAD`/
   `LOAD_DLL` events for the already-running state, so `on_CREATE_PROCESS_DEBUG_EVENT` etc. run
   unchanged.

### 3.3 Detach cleanly (new capability)

x64dbg: remove **all** breakpoints (restore original bytes / clear DR7), then
`DebugSetProcessKillOnExit(FALSE)` + `DebugActiveProcessStop(pid)`. Add a `detach()` alongside
`stop()` (which currently only `TerminateProcess`es).

### 3.4 Software-breakpoint step-over-and-restore

This is the classic "continue past a breakpoint" dance and **we already implement it** via
`BPT_CODE_STEP_TO_RESTORE` in `_handleBreakpoint()`. For reference, x64dbg's sequence is:

1. On `EXCEPTION_BREAKPOINT`, rewind `EIP/RIP` by 1 to the BP address.
2. Restore the original byte (`WriteProcessMemory` over the `0xCC`).
3. Set the single-step trap flag (EFLAGS bit 8) via `SetThreadContext`, `ContinueDebugEvent`.
4. On the resulting `EXCEPTION_SINGLE_STEP`, re-write `0xCC` and clear the trap flag.

Our logic matches; just verify the trap-flag path is used for the re-arm step (the STEP_TO_RESTORE
breakpoint) rather than relying solely on address matching.

### 3.5 Hardware breakpoints DR0–DR7 (new capability)

Not implemented in `XWindowsDebugger`. Per-thread via `GetThreadContext`/`SetThreadContext` with
`ContextFlags = CONTEXT_DEBUG_REGISTERS`:

- Put the linear address in a free `Dr0..Dr3`.
- Encode `Dr7`:
  - bit `2*n` = **local enable** Ln for slot n (also set Gn/global if desired),
  - 2-bit **R/W** field at bit `16 + 4*n`: `00`=execute, `01`=write, `11`=read/write,
  - 2-bit **LEN** field at bit `18 + 4*n`: `00`=1 byte, `01`=2, `11`=4, `10`=8 bytes.
- On the `EXCEPTION_SINGLE_STEP`, read `Dr6` to learn which slot (B0–B3) fired.

Apply the DRs to **every** thread (and to new threads in `on_CREATE_THREAD_DEBUG_EVENT`), since
debug registers are per-thread — mirror this with §2.4's "copy DRs to cloned threads" on Linux.

### 3.6 Memory breakpoints via guard pages (new capability)

x64dbg implements memory BPs/watchpoints with `VirtualProtectEx` adding `PAGE_GUARD` (or
`PAGE_NOACCESS`) to the page, then handling `EXCEPTION_GUARD_PAGE` (`STATUS_GUARD_PAGE_VIOLATION`,
`0x80000001`) / `EXCEPTION_ACCESS_VIOLATION` in `on_EXCEPTION_DEBUG_EVENT`: identify the hit,
single-step once with the page temporarily un-guarded, then re-arm. Our exception handler has
slots for the exception codes but no guard-page path yet.

### 3.7 Software-BP write hygiene

When writing `0xCC`, x64dbg wraps the write: `VirtualProtectEx` to a writable protection →
`WriteProcessMemory` → restore protection → `FlushInstructionCache`. The `FlushInstructionCache`
matters on the re-arm path so the CPU doesn't execute a stale cached instruction. Confirm
`XInfoDB`'s Windows byte-patch path flushes.

### 3.8 Loose ends already flagged in our code

- `on_CREATE_PROCESS_DEBUG_EVENT`: symbol loading from the mapped image is commented out — x64dbg
  loads module symbols on process/DLL load. Re-enable via `XPE`.
- Entry/TLS breakpoints: we set an entry-point BP (`g_bBreakpointEntryPoint`) but the `// TODO
  DLLMain` and TLS-callback breakpoints are missing; x64dbg breaks on TLS callbacks *before* the
  entry point.
- WOW64: `load()` has `// TODO 32/64 do not load if not the same(WOW64)`. We already recognize the
  WOW64 exception codes `0x4000001f`/`0x4000001e`; add the bitness guard so a 64-bit build refuses
  (or correctly drives) a 32-bit target.

---

## 4. Suggested implementation order

Highest leverage first, each independently testable:

1. **Linux clone-event decoding** (§2.4) + **multi-thread wait** (§2.5) + **tgkill freeze** (§2.6)
   — without these, multithreaded Linux targets are broken. Single, coherent change to
   `waitForSignal` / `_debugEvent`.
2. **Linux `load()` hardening** (§2.1): ASLR off, `LD_BIND_NOW`, real argv, working dir.
3. **Windows debug-loop threading audit** (§3.1) — make sure the loop and all commands share the
   debugger's worker thread. Architectural; do before adding features.
4. **`attach()` on both platforms** (§2.2, §3.2) + **Windows `detach()`** (§3.3).
5. **Hardware breakpoints**: DR0–DR7 on Windows (§3.5) and Linux `POKEUSER` (§2.7); propagate to
   new threads on both.
6. **Windows memory breakpoints** via guard pages (§3.6).
7. **Arch detection** (§2.7) + **re-enable symbol/TLS/entry work** (§3.8).

None of this requires GPL code — every item above is a documented syscall/API sequence or a
hardware register layout, reimplemented against `XInfoDB`/`XProcess`.
