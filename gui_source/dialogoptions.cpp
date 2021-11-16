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
#include "dialogoptions.h"
#include "ui_dialogoptions.h"

DialogOptions::DialogOptions(QWidget *pParent, XOptions *pOptions) :
    QDialog(pParent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    this->g_pOptions=pOptions;

    g_pOptions->setCheckBox(ui->checkBoxStayOnTop,XOptions::ID_STAYONTOP);
    g_pOptions->setCheckBox(ui->checkBoxSaveLastDirectory,XOptions::ID_SAVELASTDIRECTORY);
    g_pOptions->setComboBox(ui->comboBoxStyle,XOptions::ID_STYLE);
}

DialogOptions::~DialogOptions()
{
    delete ui;
}

void DialogOptions::on_pushButtonOK_clicked()
{
    g_pOptions->getCheckBox(ui->checkBoxSaveLastDirectory,XOptions::ID_SAVELASTDIRECTORY);
    g_pOptions->getCheckBox(ui->checkBoxStayOnTop,XOptions::ID_STAYONTOP);
    g_pOptions->getComboBox(ui->comboBoxStyle,XOptions::ID_STYLE);

    this->close();
}

void DialogOptions::on_pushButtonCancel_clicked()
{
    this->close();
}
