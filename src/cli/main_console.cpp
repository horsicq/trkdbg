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
    sDescription.append(QString("%1\n").arg("Copyright(C) 2023-2024 hors<horsicq@gmail.com> Web: http://ntinfo.biz"));
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

    QList<QString> listArgs = parser.positionalArguments();

    if (listArgs.count()) {
        XProcess::setDebugPrivilege(true);

        QString sFileName = listArgs.at(0);

        XAbstractDebugger::OPTIONS options = XAbstractDebugger::getDefaultOptions(sFileName);

        if (parser.isSet(clScript)) {
            // Script
            QString sScript = parser.value(clScript);

            if (XBinary::isFileExists(sScript)) {
                XInfoDB xInfoDB;
                xInfoDB.setDebuggerState(true);
#ifdef Q_OS_WIN
                XWindowsDebugger debugger(0, &xInfoDB);
#endif
#ifdef Q_OS_LINUX
                XLinuxDebugger debugger(0, &xInfoDB);
#endif
#ifdef Q_OS_OSX
                XOSXDebugger debugger(0, &xInfoDB);
#endif
                debugger.setOptions(options);

                XDebugScript debugScript;

                if (debugScript.setData(&debugger, sScript)) {
                    debugger.load();
                }
            } else {
                // TODO error
            }
        } else {
            XDebuggerConsole debuggerConsole;
            debuggerConsole.run(options);
        }
    } else {
        parser.showHelp();
        Q_UNREACHABLE();
    }

    return 0;
}
