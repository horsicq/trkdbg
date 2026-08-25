/* Copyright (c) 2021-2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

#include <atomic>

#include "../global.h"
#include "xdebugscript.h"
#include "xdebuggerconsole.h"
#include "xoptions.h"
#ifdef Q_OS_WIN
#include "xwindowsdebugger.h"
#endif
#ifdef Q_OS_LINUX
#include "xlinuxdebugger.h"
#endif
#ifdef Q_OS_MACOS
#include "xosxdebugger.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    QString sDescription;
    sDescription.append(QString("%1 v%2\n").arg(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));
    sDescription.append(QString("%1\n").arg("Copyright(C) 2020-2026 hors<horsicq@gmail.com> Web: https://ntinfo.biz"));
    parser.setApplicationDescription(sDescription);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("file", "The file to open.");

    //    QCommandLineOption clShowConsole(QStringList() << "c"
    //                                                   << "showconsole",
    //                                     "Show console(If target is a console application).");
    QCommandLineOption clScript(QStringList() << "s"
                                              << "script",
                                "Script <script_file_path>.", "script_file_path");

    //    parser.addOption(clShowConsole);
    parser.addOption(clScript);

    parser.process(app);

    const QList<QString> listArgs = parser.positionalArguments();
    QTextStream streamError(stderr);
    QTextStream streamOutput(stdout);

    enum EXIT_CODE {
        EXIT_OK = 0,
        EXIT_USAGE = 2,
        EXIT_INPUT = 3,
        EXIT_SCRIPT = 4,
        EXIT_DEBUGGER = 5
    };

    if (listArgs.count() != 1) {
        streamError << "Error: exactly one target file is required.\n\n" << parser.helpText();
        return EXIT_USAGE;
    }

    const QFileInfo targetInfo(listArgs.constFirst());

    if ((!targetInfo.exists()) || (!targetInfo.isFile())) {
        streamError << "Error: target file does not exist: " << targetInfo.absoluteFilePath() << Qt::endl;
        return EXIT_INPUT;
    }

    XProcess::setDebugPrivilege(true);

    const QString sFileName = targetInfo.absoluteFilePath();
    XAbstractDebugger::OPTIONS options = XAbstractDebugger::getDefaultOptions(sFileName);

    if (parser.isSet(clScript)) {
        const QFileInfo scriptInfo(parser.value(clScript));

        if ((!scriptInfo.exists()) || (!scriptInfo.isFile())) {
            streamError << "Error: script file does not exist: " << scriptInfo.absoluteFilePath() << Qt::endl;
            return EXIT_SCRIPT;
        }

        XInfoDB xInfoDB;
        xInfoDB.setDebuggerState(true);
#ifdef Q_OS_WIN
        XWindowsDebugger debugger(0, &xInfoDB);
#endif
#ifdef Q_OS_LINUX
        XLinuxDebugger debugger(0, &xInfoDB);
#endif
#ifdef Q_OS_MACOS
        XOSXDebugger debugger(0, &xInfoDB);
#endif
        debugger.setOptions(options);

        XDebugScript debugScript;
        std::atomic_bool bScriptError(false);

        QObject::connect(
            &debugScript, &XDebugScript::infoMessage, &app,
            [&streamOutput](const QString &sText) {
                streamOutput << sText << Qt::endl;
            },
            Qt::DirectConnection);
        QObject::connect(
            &debugScript, &XDebugScript::errorMessage, &app,
            [&streamError, &bScriptError](const QString &sText) {
                bScriptError.store(true, std::memory_order_relaxed);
                streamError << "Error: " << sText << Qt::endl;
                // Runtime script errors stop Unix debugger polling before an exit event can be
                // emitted, so also release the console event loop explicitly.
                QCoreApplication::quit();
            },
            Qt::DirectConnection);

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        // Unix backends are driven by a zero-delay QTimer. Keep the application alive until the
        // traced process actually exits; otherwise the stack debugger and script are destroyed
        // immediately after load() schedules its first poll.
        bool bProcessExited = false;
        QObject::connect(
            &debugger, &XAbstractDebugger::eventExitProcess, &app,
            [&app, &bProcessExited](XInfoDB::EXITPROCESS_INFO *) {
                bProcessExited = true;
                app.quit();
            },
            Qt::DirectConnection);
#endif

        if (!debugScript.setData(&debugger, scriptInfo.absoluteFilePath())) {
            if (!bScriptError.load(std::memory_order_relaxed)) {
                streamError << "Error: failed to load or evaluate script: " << scriptInfo.absoluteFilePath() << Qt::endl;
            }
            return EXIT_SCRIPT;
        }

        const bool bDebuggerLoaded = debugger.load();

        if (bScriptError.load(std::memory_order_relaxed)) {
            return EXIT_SCRIPT;
        }

        if (!bDebuggerLoaded) {
            streamError << "Error: debugger could not start target: " << sFileName << Qt::endl;
            return EXIT_DEBUGGER;
        }

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        const int nEventLoopResult = bProcessExited ? 0 : app.exec();
        if (bScriptError.load(std::memory_order_relaxed)) {
            return EXIT_SCRIPT;
        }
        if ((nEventLoopResult != 0) || (!bProcessExited)) {
            streamError << "Error: debugger event loop stopped unexpectedly for target: " << sFileName << Qt::endl;
            return EXIT_DEBUGGER;
        }
#endif
    } else {
        XDebuggerConsole debuggerConsole;

        if (!debuggerConsole.run(options)) {
            streamError << "Error: debugger could not start target: " << sFileName << Qt::endl;
            return EXIT_DEBUGGER;
        }
    }

    return EXIT_OK;
}
