/* Copyright (c) 2021-2022 hors<horsicq@gmail.com>
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
#include "guimainwindow.h"
#include "ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *pParent)
    : QMainWindow(pParent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(XOptions::getTitle(X_APPLICATIONDISPLAYNAME,X_APPLICATIONVERSION));

    setAcceptDrops(true);

    g_xOptions.setName(X_OPTIONSFILE);

    g_xOptions.addID(XOptions::ID_VIEW_STYLE,"Fusion");
    g_xOptions.addID(XOptions::ID_VIEW_QSS,"");
    g_xOptions.addID(XOptions::ID_VIEW_LANG,"System");
    g_xOptions.addID(XOptions::ID_VIEW_STAYONTOP,false);
    g_xOptions.addID(XOptions::ID_VIEW_SHOWLOGO,true);
    g_xOptions.addID(XOptions::ID_FILE_SAVELASTDIRECTORY,true);
    g_xOptions.addID(XOptions::ID_FILE_SAVEBACKUP,true);
    g_xOptions.addID(XOptions::ID_FILE_SAVERECENTFILES,true);

#ifdef Q_OS_WIN
    g_xOptions.addID(XOptions::ID_FILE_CONTEXT,"*");
#endif

//    StaticScanOptionsWidget::setDefaultValues(&g_xOptions);
    SearchSignaturesOptionsWidget::setDefaultValues(&g_xOptions);
    XHexViewOptionsWidget::setDefaultValues(&g_xOptions);
    XDisasmViewOptionsWidget::setDefaultValues(&g_xOptions);
    XDebuggerOptionsWidget::setDefaultValues(&g_xOptions);

    g_xOptions.load();

    g_xShortcuts.setName(X_SHORTCUTSFILE);
    g_xShortcuts.setNative(g_xOptions.isNative());

    g_xShortcuts.addGroup(XShortcuts::ID_DEBUGGER);
    g_xShortcuts.load();

    ui->widgetDebugger->setGlobal(&g_xShortcuts,&g_xOptions);

    connect(&g_xOptions,SIGNAL(openFile(QString)),this,SLOT(handleFile(QString)));

    createMenus();

    adjustWindow();

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
    menuAction[MA_FILE_OPEN]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_OPEN));
    menuAction[MA_FILE_CLOSE]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_CLOSE));
    menuAction[MA_FILE_ATTACH]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_ATTACH));
    menuAction[MA_FILE_DETACH]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_DETACH));
    menuAction[MA_FILE_EXIT]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_FILE_EXIT));
    menuAction[MA_VIEW_CPU]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_CPU));
    menuAction[MA_VIEW_LOG]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_LOG));
    menuAction[MA_VIEW_BREAKPOINTS]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_BREAKPOINTS));
    menuAction[MA_VIEW_MEMORYMAP]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_MEMORYMAP));
    menuAction[MA_VIEW_CALLSTACK]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_CALLSTACK));
    menuAction[MA_VIEW_THREADS]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_THREADS));
    menuAction[MA_VIEW_HANDLES]->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_VIEW_HANDLES));

//    ui->actionDebugRun->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_RUN));
//    ui->actionDebugStepInto->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_STEPINTO));
//    ui->actionDebugStepOver->setShortcut(g_xShortcuts.getShortcut(XShortcuts::ID_DEBUGGER_DEBUG_STEPOVER));
}

void GuiMainWindow::createMenus()
{
    QMenu *pMenuFile=new QMenu(tr("File"),ui->menubar);
    QMenu *pMenuView=new QMenu(tr("View"),ui->menubar);
    QMenu *pMenuDebug=new QMenu(tr("Debug"),ui->menubar);
    QMenu *pMenuTracing=new QMenu(tr("Tracing"),ui->menubar);
    QMenu *pMenuTools=new QMenu(tr("Tools"),ui->menubar);
    QMenu *pMenuHelp=new QMenu(tr("Help"),ui->menubar);

    ui->menubar->addAction(pMenuFile->menuAction());
    ui->menubar->addAction(pMenuView->menuAction());
    ui->menubar->addAction(pMenuDebug->menuAction());
    ui->menubar->addAction(pMenuTracing->menuAction());
    ui->menubar->addAction(pMenuTools->menuAction());
    ui->menubar->addAction(pMenuHelp->menuAction());

    menuAction[MA_FILE_OPEN]=new QAction(tr("Open"),this);
    menuAction[MA_FILE_CLOSE]=new QAction(tr("Close"),this);
    menuAction[MA_FILE_ATTACH]=new QAction(tr("Attach"),this);
    menuAction[MA_FILE_DETACH]=new QAction(tr("Detach"),this);
    menuAction[MA_FILE_EXIT]=new QAction(tr("Exit"),this);
    menuAction[MA_VIEW_CPU]=new QAction(tr("CPU"),this);
    menuAction[MA_VIEW_LOG]=new QAction(tr("Log"),this);
    menuAction[MA_VIEW_BREAKPOINTS]=new QAction(tr("Breakpoints"),this);
    menuAction[MA_VIEW_MEMORYMAP]=new QAction(tr("Memory map"),this);
    menuAction[MA_VIEW_CALLSTACK]=new QAction(tr("Callstacks"),this);
    menuAction[MA_VIEW_THREADS]=new QAction(tr("Threads"),this);
    menuAction[MA_VIEW_HANDLES]=new QAction(tr("Handles"),this);

//    QAction *pActionOptions=new QAction(tr("Options"),this);
//    QAction *pActionAbout=new QAction(tr("About"),this);
//    QAction *pActionShortcuts=new QAction(tr("Shortcuts"),this);
//    QAction *pActionDemangle=new QAction(tr("Demangle"),this);

    pMenuFile->addAction(menuAction[MA_FILE_OPEN]);
    pMenuFile->addMenu(g_xOptions.createRecentFilesMenu(this));
    pMenuFile->addAction(menuAction[MA_FILE_CLOSE]);
    pMenuFile->addAction(menuAction[MA_FILE_ATTACH]);
    pMenuFile->addAction(menuAction[MA_FILE_DETACH]);
    pMenuFile->addAction(menuAction[MA_FILE_EXIT]);
    pMenuView->addAction(menuAction[MA_VIEW_CPU]);
    pMenuView->addAction(menuAction[MA_VIEW_LOG]);
    pMenuView->addAction(menuAction[MA_VIEW_BREAKPOINTS]);
    pMenuView->addAction(menuAction[MA_VIEW_MEMORYMAP]);
    pMenuView->addAction(menuAction[MA_VIEW_CALLSTACK]);
    pMenuView->addAction(menuAction[MA_VIEW_THREADS]);
    pMenuView->addAction(menuAction[MA_VIEW_HANDLES]);
//    pMenuTools->addAction(pActionDemangle);
//    pMenuTools->addAction(pActionShortcuts);
//    pMenuTools->addAction(pActionOptions);
//    pMenuHelp->addAction(pActionAbout);

    connect(menuAction[MA_FILE_OPEN],SIGNAL(triggered()),this,SLOT(actionFileOpen()));
    connect(menuAction[MA_FILE_CLOSE],SIGNAL(triggered()),this,SLOT(actionFileClose()));
    connect(menuAction[MA_FILE_ATTACH],SIGNAL(triggered()),this,SLOT(actionFileAttach()));
    connect(menuAction[MA_FILE_DETACH],SIGNAL(triggered()),this,SLOT(actionFileDetach()));
    connect(menuAction[MA_FILE_EXIT],SIGNAL(triggered()),this,SLOT(actionFileExit()));
    connect(menuAction[MA_VIEW_CPU],SIGNAL(triggered()),this,SLOT(actionViewCPU()));
    connect(menuAction[MA_VIEW_LOG],SIGNAL(triggered()),this,SLOT(actionViewLog()));
    connect(menuAction[MA_VIEW_BREAKPOINTS],SIGNAL(triggered()),this,SLOT(actionViewBreakpoint()));
    connect(menuAction[MA_VIEW_MEMORYMAP],SIGNAL(triggered()),this,SLOT(actionViewMemoryMap()));
    connect(menuAction[MA_VIEW_CALLSTACK],SIGNAL(triggered()),this,SLOT(actionViewCallStack()));
    connect(menuAction[MA_VIEW_THREADS],SIGNAL(triggered()),this,SLOT(actionViewThreads()));
    connect(menuAction[MA_VIEW_HANDLES],SIGNAL(triggered()),this,SLOT(actionViewHandles()));
//    connect(pActionOptions,SIGNAL(triggered()),this,SLOT(actionOptionsSlot()));
//    connect(pActionAbout,SIGNAL(triggered()),this,SLOT(actionAboutSlot()));
//    connect(pActionShortcuts,SIGNAL(triggered()),this,SLOT(actionShortcutsSlot()));
//    connect(pActionDemangle,SIGNAL(triggered()),this,SLOT(actionDemangleSlot()));
    // TODO
}

void GuiMainWindow::actionFileExit()
{
    // TODO
    this->close();
}

void GuiMainWindow::actionDebugRun()
{
    ui->widgetDebugger->debugRun();
}

void GuiMainWindow::actionDebugStepInto()
{
    ui->widgetDebugger->debugStepInto();
}

void GuiMainWindow::actionDebugStepOver()
{
    ui->widgetDebugger->debugStepOver();
}

void GuiMainWindow::actionToolsOptions()
{
    DialogOptions dialogOptions(this,&g_xOptions);

    dialogOptions.exec();

    adjustWindow();
}

void GuiMainWindow::actionToolsShortcuts()
{
    DialogShortcuts dialogShortcuts(this);

    dialogShortcuts.setData(&g_xShortcuts);

    dialogShortcuts.exec();

    setShortcuts();
}

void GuiMainWindow::actionHelpAbout()
{
    // TODO
}

void GuiMainWindow::handleFile(QString sFileName)
{
    QFileInfo fi(sFileName);

    if(fi.isFile())
    {
        // TODO Check
        if(ui->widgetDebugger->loadFile(sFileName))
        {
            g_xOptions.setLastFileName(sFileName);
        }
    }
}

void GuiMainWindow::adjustWindow()
{
    g_xOptions.adjustStayOnTop(this);
    ui->widgetDebugger->adjustView();
}

void GuiMainWindow::actionFileOpen()
{
    QString sDirectory=g_xOptions.getLastDirectory();

    QString sFileName=QFileDialog::getOpenFileName(this,tr("Open file")+QString("..."),sDirectory,tr("All files")+QString(" (*)")); // TODO extension

    if(!sFileName.isEmpty())
    {
        handleFile(sFileName);
    }
}

void GuiMainWindow::actionFileClose()
{
    // TODO
}

void GuiMainWindow::actionFileAttach()
{
    // TODO
}

void GuiMainWindow::actionFileDetach()
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

void GuiMainWindow::actionViewCPU()
{
    ui->widgetDebugger->viewCPU();
}

void GuiMainWindow::actionViewLog()
{
    ui->widgetDebugger->viewLog();
}

void GuiMainWindow::actionViewBreakpoint()
{
    ui->widgetDebugger->viewBreakpoints();
}

void GuiMainWindow::actionViewMemoryMap()
{
    ui->widgetDebugger->viewMemoryMap();
}

void GuiMainWindow::actionViewCallStack()
{
    ui->widgetDebugger->viewCallstack();
}

void GuiMainWindow::actionViewThreads()
{
    ui->widgetDebugger->viewThreads();
}

void GuiMainWindow::actionViewHandles()
{
    ui->widgetDebugger->viewHandles();
}
