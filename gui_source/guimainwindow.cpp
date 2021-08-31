// Copyright (c) 2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "guimainwindow.h"
#include "ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *pParent)
    : QMainWindow(pParent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("%1 v%2").arg(X_APPLICATIONDISPLAYNAME).arg(X_APPLICATIONVERSION));

    setAcceptDrops(true);

    g_xOptions.setName(X_OPTIONSFILE);

    QList<XOptions::ID> listIDs;

    listIDs.append(XOptions::ID_STYLE);
    listIDs.append(XOptions::ID_QSS);
    listIDs.append(XOptions::ID_LANG);
    listIDs.append(XOptions::ID_SCANAFTEROPEN);
    listIDs.append(XOptions::ID_STAYONTOP);
    listIDs.append(XOptions::ID_SAVELASTDIRECTORY);
    listIDs.append(XOptions::ID_SEARCHSIGNATURESPATH);

    g_xOptions.setValueIDs(listIDs);
    g_xOptions.load();

    g_xShortcuts.setName(X_SHORTCUTSFILE);
    g_xShortcuts.setNative(g_xOptions.isNative());

    g_xShortcuts.addGroup(XShortcuts::ID_DEBUGGER);
    g_xShortcuts.load();

    adjust();

    ui->widgetDebugger->setShortcuts(&g_xShortcuts);

    setShortcuts();

    if(QCoreApplication::arguments().count()>1)
    {
        handleFile(QCoreApplication::arguments().at(1));
    }
}

GuiMainWindow::~GuiMainWindow()
{
    g_xOptions.save();
    g_xShortcuts.save();

    delete ui;
}

void GuiMainWindow::setShortcuts()
{
    ui->actionFileOpen->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_OPEN));
    ui->actionFileAttach->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_ATTACH));
    ui->actionFileDetach->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_DETACH));
    ui->actionFileExit->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_EXIT));

    ui->actionViewCPU->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_CPU));
    ui->actionViewActions->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_ACTIONS));
    ui->actionViewLog->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_LOG));
    ui->actionViewBreakpoint->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_BREAKPOINTS));
    ui->actionViewMemoryMap->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_MEMORYMAP));
    ui->actionViewCallStack->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_CALLSTACK));
    ui->actionViewThreads->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_THREADS));
    ui->actionViewHandles->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_HANDLES));

    ui->actionDebugRun->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_RUN));
    ui->actionDebugStepInto->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_STEPINTO));
    ui->actionDebugStepOver->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_STEPOVER));
}

void GuiMainWindow::on_actionFileExit_triggered()
{
    // TODO
    this->close();
}

void GuiMainWindow::on_actionDebugRun_triggered()
{
    ui->widgetDebugger->debugRun();
}

void GuiMainWindow::on_actionDebugStepInto_triggered()
{
    ui->widgetDebugger->debugStepInto();
}

void GuiMainWindow::on_actionDebugStepOver_triggered()
{
    ui->widgetDebugger->debugStepOver();
}

void GuiMainWindow::on_actionToolsOptions_triggered()
{
    // TODO
}

void GuiMainWindow::on_actionToolsShortcuts_triggered()
{
    DialogShortcuts dialogShortcuts(this);

    dialogShortcuts.setData(&g_xShortcuts);

    dialogShortcuts.exec();

    setShortcuts();
}

void GuiMainWindow::on_actionHelpAbout_triggered()
{
    // TODO
}

void GuiMainWindow::handleFile(QString sFileName)
{
    QFileInfo fi(sFileName);

    if(fi.isFile())
    {
        // TODO Check
        ui->widgetDebugger->loadFile(sFileName);

        g_xOptions.setLastDirectory(sFileName);
    }
}

void GuiMainWindow::adjust()
{
    g_xOptions.adjustStayOnTop(this);
}

void GuiMainWindow::on_actionFileOpen_triggered()
{
    QString sDirectory=g_xOptions.getLastDirectory();

    QString sFileName=QFileDialog::getOpenFileName(this,tr("Open file")+QString("..."),sDirectory,tr("All files")+QString(" (*)")); // TODO extension

    if(!sFileName.isEmpty())
    {
        handleFile(sFileName);
    }
}

void GuiMainWindow::on_actionFileAttach_triggered()
{
    // TODO
}

void GuiMainWindow::on_actionFileDetach_triggered()
{
    // TODO
}

void GuiMainWindow::dragEnterEvent(QDragEnterEvent *pEvent)
{
    pEvent->acceptProposedAction();
}

void GuiMainWindow::dragMoveEvent(QDragMoveEvent *pEvent)
{
    pEvent->acceptProposedAction();
}

void GuiMainWindow::dropEvent(QDropEvent *pEvent)
{
    const QMimeData *pMimeData=pEvent->mimeData();

    if(pMimeData->hasUrls())
    {
        QList<QUrl> urlList=pMimeData->urls();

        if(urlList.count())
        {
            QString sFileName=urlList.at(0).toLocalFile();

            sFileName=XBinary::convertFileName(sFileName);

            handleFile(sFileName);
        }
    }
}

void GuiMainWindow::on_actionViewCPU_triggered()
{
    ui->widgetDebugger->viewCPU();
}

void GuiMainWindow::on_actionViewActions_triggered()
{
    ui->widgetDebugger->viewActions();
}

void GuiMainWindow::on_actionViewLog_triggered()
{
    ui->widgetDebugger->viewLog();
}

void GuiMainWindow::on_actionViewBreakpoint_triggered()
{
    ui->widgetDebugger->viewBreakpoints();
}

void GuiMainWindow::on_actionViewMemoryMap_triggered()
{
    ui->widgetDebugger->viewMemoryMap();
}

void GuiMainWindow::on_actionViewCallStack_triggered()
{
    ui->widgetDebugger->viewCallstack();
}

void GuiMainWindow::on_actionViewThreads_triggered()
{
    ui->widgetDebugger->viewThreads();
}

void GuiMainWindow::on_actionViewHandles_triggered()
{
    ui->widgetDebugger->viewHandles();
}
