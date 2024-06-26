/* Copyright (c) 2021-2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QMimeData>

#include "../global.h"
#include "dialogabout.h"
#include "dialogoptions.h"
#include "dialogshortcuts.h"
#include "xoptions.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GuiMainWindow;
}
QT_END_NAMESPACE

class GuiMainWindow : public QMainWindow {
    Q_OBJECT

    enum MENUACTION {
        MA_FILE_OPEN,
        MA_FILE_CLOSE,
        MA_FILE_ATTACH,
        MA_FILE_DETACH,
        MA_FILE_EXIT,
        MA_VIEW_CPU,
        MA_VIEW_LOG,
        MA_VIEW_BREAKPOINTS,
        MA_VIEW_MEMORYMAP,
        MA_VIEW_CALLSTACK,
        MA_VIEW_THREADS,
        MA_VIEW_HANDLES,
        MA_VIEW_MODULES,
        MA_VIEW_SYMBOLS,
        MA_DEBUG_RUN,
        MA_DEBUG_CLOSE,
        MA_DEBUG_STEPINTO,
        MA_DEBUG_STEPOVER,
        MA_ANIMATE_STEPINTO,
        MA_ANIMATE_STEPOVER,
        MA_ANIMATE_STOP,
        MA_TRACE_STEPINTO,
        MA_TRACE_STEPOVER,
        MA_TRACE_STOP,
        MA_TOOLS_SHORTCUTS,
        MA_TOOLS_OPTIONS,
        MA_HELP_ABOUT,
        __MA_SIZE,
    };

public:
    GuiMainWindow(QWidget *pParent = nullptr);
    ~GuiMainWindow();

    void setShortcuts();

private slots:
    void createMenus();
    void handleFile(const QString &sFileName);
    void errorMessageSlot(const QString &sText);
    void adjustWindow();
    void actionFileOpen();
    void actionFileClose();
    void actionFileAttach();
    void actionFileDetach();
    void actionFileExit();
    void actionDebugRun();
    void actionDebugClose();
    void actionDebugStepInto();
    void actionDebugStepOver();
    void actionAnimateStepInto();
    void actionAnimateStepOver();
    void actionAnimateStop();
    void actionTraceStepInto();
    void actionTraceStepOver();
    void actionTraceStop();
    void actionToolsOptions();
    void actionToolsShortcuts();
    void actionHelpAbout();
    void actionViewCPU();
    void actionViewLog();
    void actionViewBreakpoint();
    void actionViewMemoryMap();
    void actionViewCallStack();
    void actionViewThreads();
    void actionViewHandles();
    void actionViewModules();
    void actionViewSymbols();
    void stateChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *pEvent) override;
    void dragMoveEvent(QDragMoveEvent *pEvent) override;
    void dropEvent(QDropEvent *pEvent) override;

private:
    Ui::GuiMainWindow *ui;
    XOptions g_xOptions;
    XShortcuts g_xShortcuts;
    QAction *menuAction[__MA_SIZE];
};
#endif  // GUIMAINWINDOW_H
