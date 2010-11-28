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
    QString filePath = QFileDialog::getSaveFileName(this, tr("Open file"), QDir::homePath(), tr("Wii Plain ISO *.iso;;Wii Compact ISO *.ciso;;Wii ISO Archive *.wia;;Wii Disc Format *.wdf;;Wii Backup File System Container *.wbfs"));

    if (!filePath.isEmpty()) {
        ui->lineEdit_FilePath->setText(filePath);
        setCurrentImageFormat(filePath);
    }
}

QString WiiBaFuDialog::imageDirectory() {
    return ui->lineEdit_Directory->text();
}

QString WiiBaFuDialog::imageFilePath() {
    return ui->lineEdit_FilePath->text();
}

QString WiiBaFuDialog::imageFormat() {
    QString format;
    setCurrentImageFormat(ui->lineEdit_FilePath->text());

    switch (ui->comboBox_ImageFormat->currentIndex()) {
        case 0:     format = QString("iso");
                    break;
        case 1:     format = QString("ciso");
                    break;
        case 2:     format = QString("wia");
                    break;
        case 3:     format = QString("wdf");
                    break;
        case 4:     format = QString("wbfs");
                    break;
        default:    format = QString("");
    }

    return format;
}

QString WiiBaFuDialog::compression() {
    QString compression;

    if (ui->comboBox_ImageFormat->currentIndex() != 2) {
        compression = "";
    }
    else {
        switch (ui->comboBox_compressionDefaults->currentIndex()) {
            case 0:
            case 1:
            case 2:
            case 3:
                    compression = ui->comboBox_compressionDefaults->currentText().toUpper();
                    break;
            case 4:
                    compression = QString("%1.%2@%3").arg(ui->comboBox_compressionMethod->currentText().toUpper(), ui->comboBox_compressionLevel->currentText().mid(0, 1), ui->lineEdit_compressionFactor->text());
                    break;
            default:
                    compression = "DEFAULT";
        }
    }

    return compression;
}

void WiiBaFuDialog::setCurrentImageFormat(QString path) {
    QString extension = path.right(path.length() - path.lastIndexOf("."));

    if (extension.contains(".iso")) {
        ui->comboBox_ImageFormat->setCurrentIndex(0);
    }
    else if (extension.contains(".ciso")) {
        ui->comboBox_ImageFormat->setCurrentIndex(1);
    }
    else if (extension.contains(".wia")) {
        ui->comboBox_ImageFormat->setCurrentIndex(2);
    }
    else if (extension.contains(".wdf")) {
        ui->comboBox_ImageFormat->setCurrentIndex(3);
    }
    else if (extension.contains(".wbfs")) {
        ui->comboBox_ImageFormat->setCurrentIndex(4);
    }
}

void WiiBaFuDialog::on_comboBox_ImageFormat_currentIndexChanged(int index) {
    if (index == 2) {
        ui->comboBox_compressionDefaults->setEnabled(true);
        ui->frame_compressionMethod->setEnabled(true);
    }
    else {
        ui->comboBox_compressionDefaults->setEnabled(false);
        ui->frame_compressionMethod->setEnabled(false);
    }
}

void WiiBaFuDialog::on_comboBox_compressionDefaults_currentIndexChanged(int index) {
    switch (index) {
        case 0:
                ui->comboBox_compressionMethod->setCurrentIndex(3);
                ui->comboBox_compressionLevel->setCurrentIndex(5);
                ui->lineEdit_compressionFactor->setText("20");
                ui->comboBox_compressionMethod->setEnabled(false);
                ui->comboBox_compressionLevel->setEnabled(false);
                ui->lineEdit_compressionFactor->setEnabled(false);
                break;
        case 1:
                ui->comboBox_compressionMethod->setCurrentIndex(2);
                ui->comboBox_compressionLevel->setCurrentIndex(3);
                ui->lineEdit_compressionFactor->setText("10");
                ui->comboBox_compressionMethod->setEnabled(false);
                ui->comboBox_compressionLevel->setEnabled(false);
                ui->lineEdit_compressionFactor->setEnabled(false);
                break;
        case 2:
                ui->comboBox_compressionMethod->setCurrentIndex(3);
                ui->comboBox_compressionLevel->setCurrentIndex(5);
                ui->lineEdit_compressionFactor->setText("20");
                ui->comboBox_compressionMethod->setEnabled(false);
                ui->comboBox_compressionLevel->setEnabled(false);
                ui->lineEdit_compressionFactor->setEnabled(false);
                break;
        case 3:
                ui->comboBox_compressionMethod->setCurrentIndex(3);
                ui->comboBox_compressionLevel->setCurrentIndex(7);
                ui->lineEdit_compressionFactor->setText("50");
                ui->comboBox_compressionMethod->setEnabled(false);
                ui->comboBox_compressionLevel->setEnabled(false);
                ui->lineEdit_compressionFactor->setEnabled(false);
                break;
        case 4:
                ui->comboBox_compressionMethod->setCurrentIndex(3);
                ui->comboBox_compressionLevel->setCurrentIndex(5);
                ui->lineEdit_compressionFactor->setText("20");
                ui->comboBox_compressionMethod->setEnabled(true);
                ui->comboBox_compressionLevel->setEnabled(true);
                ui->lineEdit_compressionFactor->setEnabled(true);
                break;
        default:
                ui->comboBox_compressionMethod->setCurrentIndex(3);
                ui->comboBox_compressionLevel->setCurrentIndex(5);
                ui->lineEdit_compressionFactor->setText("20");
                ui->comboBox_compressionMethod->setEnabled(false);
                ui->comboBox_compressionLevel->setEnabled(false);
                ui->lineEdit_compressionFactor->setEnabled(false);
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
