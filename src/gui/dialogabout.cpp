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
#include "dialogabout.h"

#include "ui_dialogabout.h"

#include "../global.h"
#include "xoptions.h"

namespace {

QString centeredParagraph(const QString &sText)
{
    return QStringLiteral("<p align=\"center\"><span style=\" font-weight:600;\">%1</span></p>").arg(sText);
}

QString centeredLink(const QString &sLabel, const QString &sCaption, const QString &sUrl)
{
    return QStringLiteral(
               "<p align=\"center\"><span style=\" font-weight:600;\">%1: </span>"
               "<a href=\"%2\"><span style=\" text-decoration: underline; color:#ff0000;\">%3</span></a></p>")
        .arg(sLabel, sUrl, sCaption);
}

}  // namespace

XAboutWidget::DATA DialogAbout::createAboutData()
{
    XAboutWidget::DATA data = {};

    data.sInfo += centeredParagraph(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));
    data.sInfo += centeredParagraph(QStringLiteral("Copyright (C) 2021-2026 hors"));
    data.sInfo += centeredParagraph(QObject::tr("A cross-platform debugger for executable files"));
    data.sInfo += centeredLink(QObject::tr("Bugreports"), QStringLiteral("horsicq@gmail.com"), QStringLiteral("mailto:horsicq@gmail.com"));
    data.sInfo += centeredLink(QObject::tr("Website"), QStringLiteral("http://ntinfo.biz"), QStringLiteral("http://ntinfo.biz"));
    data.sInfo += centeredLink(QObject::tr("Source code"),
                               QStringLiteral("https://github.com/horsicq/trkdbg"),
                               QStringLiteral("https://github.com/horsicq/trkdbg"));

    data.sLibraries += centeredLink(QStringLiteral("Qt Library %1").arg(QT_VERSION_STR),
                                    QStringLiteral("https://www.qt.io"),
                                    QStringLiteral("https://www.qt.io"));
    data.sLibraries += centeredParagraph(QStringLiteral("Capstone / XEmulator / XInfoDB"));

    data.sLogoPath = QStringLiteral(":/images/logo.png");
    data.sUpdatesLink = QStringLiteral("https://github.com/horsicq/trkdbg");
    data.sThanksLink = QStringLiteral("https://github.com/horsicq/trkdbg/graphs/contributors");

    return data;
}

DialogAbout::DialogAbout(QWidget *pParent) : QDialog(pParent), ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->widgetAbout->setData(createAboutData());
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButtonOK_clicked()
{
    accept();
}
