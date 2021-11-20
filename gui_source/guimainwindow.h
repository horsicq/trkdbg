/* Copyright (c) 2021 hors<horsicq@gmail.com>
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

#include "../global.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include "xoptions.h"
#include "dialogshortcuts.h"
#include "dialogoptions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GuiMainWindow; }
QT_END_NAMESPACE

class GuiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GuiMainWindow(QWidget *pParent=nullptr);
    ~GuiMainWindow();
    void setShortcuts();

private slots:
    void handleFile(QString sFileName);
    void adjust();
    void on_actionFileOpen_triggered();
    void on_actionFileAttach_triggered();
    void on_actionFileDetach_triggered();
    void on_actionFileExit_triggered();
    void on_actionDebugRun_triggered();
    void on_actionDebugStepInto_triggered();
    void on_actionDebugStepOver_triggered();
    void on_actionToolsOptions_triggered();
    void on_actionToolsShortcuts_triggered();
    void on_actionHelpAbout_triggered();
    void on_actionViewCPU_triggered();
    void on_actionViewActions_triggered();
    void on_actionViewLog_triggered();
    void on_actionViewBreakpoint_triggered();
    void on_actionViewMemoryMap_triggered();
    void on_actionViewCallStack_triggered();
    void on_actionViewThreads_triggered();
    void on_actionViewHandles_triggered();

protected:
    void dragEnterEvent(QDragEnterEvent *pEvent) override;
    void dragMoveEvent(QDragMoveEvent *pEvent) override;
    void dropEvent(QDropEvent *pEvent) override;

private:
    Ui::GuiMainWindow *ui;
    XOptions g_xOptions;
    XShortcuts g_xShortcuts;
};
#endif // GUIMAINWINDOW_H
