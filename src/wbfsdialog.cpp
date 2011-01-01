/***************************************************************************
 *   Copyright (C) 2010-2011 Kai Heitkamp                                  *
 *   dynup@ymail.com | wiibafu.codeplex.com                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "wbfsdialog.h"
#include "ui_wbfsdialog.h"

WBFSDialog::WBFSDialog(QWidget *parent) : QDialog(parent), ui(new Ui::WBFSDialog) {
    ui->setupUi(this);
}

void WBFSDialog::on_pushButton_Open_clicked() {
    QString path = QFileDialog::getSaveFileName(this, tr("Create WBFS file"), QDir::homePath(), tr("WBFS file *.wbfs"));

    if (!path.isEmpty()) {
        ui->lineEdit_Path->setText(path);
    }
}

QString WBFSDialog::path() {
    return ui->lineEdit_Path->text();
}

QString WBFSDialog::size() {
    return ui->lineEdit_Size->text();
}

QString WBFSDialog::splitSize() {
    return ui->lineEdit_SplitSize->text();
}

QString WBFSDialog::hdSectorSize() {
    return ui->lineEdit_HDDSectorSize->text();
}

QString WBFSDialog::wbfsSectorSize() {
    return ui->lineEdit_WBFSSectorSize->text();
}

bool WBFSDialog::recover() {
    return ui->checkBox_Recover->checkState();
}

bool WBFSDialog::inode() {
    return ui->checkBox_Inode->checkState();
}

bool WBFSDialog::test() {
    return ui->checkBox_Test->checkState();
}

void WBFSDialog::setMacOSXStyle() {
    if (WiiBaFuSettings.value("Main/MacOSXStyle", QVariant("Aqua")).toString().contains("BrushedMetal")) {
        ui->frame->setFrameShape(QFrame::NoFrame);
        this->setAttribute(Qt::WA_MacBrushedMetal, true);
    }
    else {
        ui->frame->setFrameShape(QFrame::StyledPanel);
        this->setAttribute(Qt::WA_MacBrushedMetal, false);
    }
}

WBFSDialog::~WBFSDialog() {
    delete ui;
}
