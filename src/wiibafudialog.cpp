/***************************************************************************
 *   Copyright (C) 2010 Kai Heitkamp                                       *
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

#include <QtGui>

#include "wiibafudialog.h"
#include "ui_wiibafudialog.h"

WiiBaFuDialog::WiiBaFuDialog(QWidget *parent) : QDialog(parent), ui(new Ui::WiiBaFuDialog) {
    ui->setupUi(this);
}

void WiiBaFuDialog::on_pushButton_Open_clicked() {
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        ui->lineEdit_Directory->setText(directory);
    }
}

void WiiBaFuDialog::on_pushButton_OpenFile_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Open file"), QDir::homePath(), tr("Wii Plain ISO *.iso;;Wii Compact ISO *.ciso;;Wii Disc Format *.wdf;;Wii Backup File System Container *.wbfs"));

    if (!filePath.isEmpty()) {
        ui->lineEdit_FilePath->setText(filePath);

        QString extension = filePath.right(filePath.length() - filePath.indexOf("."));
        if (extension.contains(".iso")) {
            ui->comboBox_ImageFormat->setCurrentIndex(0);
        }
        else if (extension.contains(".ciso")) {
            ui->comboBox_ImageFormat->setCurrentIndex(1);
        }
        else if (extension.contains(".wdf")) {
            ui->comboBox_ImageFormat->setCurrentIndex(2);
        }
        else if (extension.contains(".wbfs")) {
            ui->comboBox_ImageFormat->setCurrentIndex(3);
        }
    }
}

QString WiiBaFuDialog::imageDirectory() {
    return ui->lineEdit_Directory->text();
}

QString WiiBaFuDialog::imageFilePath() {
    return ui->lineEdit_FilePath->text();
}

QString WiiBaFuDialog::imageFormat() {
    switch (ui->comboBox_ImageFormat->currentIndex()) {
        case 0:     return QString("iso");
                    break;
        case 1:     return QString("ciso");
                    break;
        case 2:     return QString("wdf");
                    break;
        case 3:     return QString("wbfs");
                    break;
        default:    return QString("");
    }
}

void WiiBaFuDialog::setOpenDirectory() {
    this->setWindowTitle(tr("Open directory"));

    ui->label_Directory->setVisible(true);
    ui->lineEdit_Directory->setVisible(true);
    ui->pushButton_Open->setVisible(true);

    ui->label_FilePath->setVisible(false);
    ui->lineEdit_FilePath->setVisible(false);
    ui->pushButton_OpenFile->setVisible(false);
}

void WiiBaFuDialog::setOpenFile() {
    this->setWindowTitle(tr("Open file"));

    ui->label_FilePath->setVisible(true);
    ui->lineEdit_FilePath->setVisible(true);
    ui->pushButton_OpenFile->setVisible(true);

    ui->label_Directory->setVisible(false);
    ui->lineEdit_Directory->setVisible(false);
    ui->pushButton_Open->setVisible(false);
}

WiiBaFuDialog::~WiiBaFuDialog() {
    delete ui;
}
