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

#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent), ui(new Ui::Settings) {
    ui->setupUi(this);
    load();
}

void Settings::on_main_pushButton_WBFSOpenFile_clicked() {
    QString path;
    if (!ui->main_lineEdit_WBFSPath->text().isEmpty()) {
        path = ui->main_lineEdit_WBFSPath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString wbfsFile = QFileDialog::getOpenFileName(this, tr("Open WBFS file"), path, "WBFS files (*.wbfs)");

    if (!wbfsFile.isEmpty()) {
        ui->main_lineEdit_WBFSPath->setText(wbfsFile);
    }
}

void Settings::on_main_pushButton_WBFSOpenDirectory_clicked() {
    QString path;
    if (!ui->main_lineEdit_WBFSPath->text().isEmpty()) {
        path = ui->main_lineEdit_WBFSPath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString wbfsDirectory = QFileDialog::getExistingDirectory(this, tr("Open WBFS directory"), path, QFileDialog::ShowDirsOnly);

    if (!wbfsDirectory.isEmpty()) {
        ui->main_lineEdit_WBFSPath->setText(wbfsDirectory);
    }
}

void Settings::on_main_pushButton_DVDDriveOpenDirectory_clicked() {
    QString path;
    if (!ui->main_lineEdit_DVDDrivePath->text().isEmpty()) {
        path = ui->main_lineEdit_DVDDrivePath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString dvdPath = QFileDialog::getExistingDirectory(this, tr("Open DVD path"), path, QFileDialog::ShowDirsOnly);

    if (!dvdPath.isEmpty()) {
        ui->main_lineEdit_DVDDrivePath->setText(dvdPath);
    }
}

void Settings::on_listWidget_itemSelectionChanged() {
    ui->stackedWidget->setCurrentIndex(ui->listWidget->currentIndex().row());
}

void Settings::on_buttonBox_clicked(QAbstractButton *button) {
    if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
        apply();
    }
    else if (button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        restoreDefaults(ui->stackedWidget->currentIndex());
    }
}

void Settings::on_buttonBox_accepted() {
    save();
}

void Settings::apply() {
    save();
}

void Settings::load() {
    QSettings wiiBaFuSettings("WiiBaFu", "wiibafu");

    ui->main_checkBox_Auto->setChecked(wiiBaFuSettings.value("Main/Auto", QVariant(true)).toBool());
    ui->main_lineEdit_WBFSPath->setText(wiiBaFuSettings.value("Main/WBFSPath", QVariant("")).toString());
    ui->main_lineEdit_DVDDrivePath->setText(wiiBaFuSettings.value("Main/DVDDrivePath", QVariant("")).toString());
    ui->main_comboBox_LogOutput->setCurrentIndex(wiiBaFuSettings.value("Main/LogOutput", QVariant(0)).toInt());

    ui->gameLists_checkBox_ShowGrid->setChecked(wiiBaFuSettings.value("GameLists/ShowGrid", QVariant(false)).toBool());
    ui->gameLists_checkBox_AlternatingRowColors->setChecked(wiiBaFuSettings.value("GameLists/AlternatingRowColors", QVariant(true)).toBool());

    setScrollMode(wiiBaFuSettings.value("GameLists/ScrollMode", QVariant(QAbstractItemView::ScrollPerPixel)).toInt());
    setResizeMode(wiiBaFuSettings.value("GameLists/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());

    ui->filesToWBFS_checkBox_Force->setChecked(wiiBaFuSettings.value("FilesToWBFS/Force", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Test->setChecked(wiiBaFuSettings.value("FilesToWBFS/Test", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Newer->setChecked(wiiBaFuSettings.value("FilesToWBFS/Newer", QVariant(true)).toBool());
    ui->filesToWBFS_checkBox_Update->setChecked(wiiBaFuSettings.value("FilesToWBFS/Update", QVariant(true)).toBool());
    ui->filesToWBFS_checkBox_Overwrite->setChecked(wiiBaFuSettings.value("FilesToWBFS/Overwrite", QVariant(false)).toBool());

    ui->dvdToWBFS_checkBox_Force->setChecked(wiiBaFuSettings.value("DVDtoWBFS/Force", QVariant(false)).toBool());
    ui->dvdToWBFS_checkBox_Test->setChecked(wiiBaFuSettings.value("DVDtoWBFS/Test", QVariant(false)).toBool());
    ui->dvdToWBFS_checkBox_Newer->setChecked(wiiBaFuSettings.value("DVDtoWBFS/Newer", QVariant(true)).toBool());
    ui->dvdToWBFS_checkBox_Update->setChecked(wiiBaFuSettings.value("DVDtoWBFS/Update", QVariant(true)).toBool());
    ui->dvdToWBFS_checkBox_Overwrite->setChecked(wiiBaFuSettings.value("DVDtoWBFS/Overwrite", QVariant(false)).toBool());

    ui->WBFSToFiles_checkBox_Force->setChecked(wiiBaFuSettings.value("WBFStoFiles/Force", QVariant(false)).toBool());
    ui->WBFSToFiles_checkBox_Test->setChecked(wiiBaFuSettings.value("WBFStoFiles/Test", QVariant(false)).toBool());
    ui->WBFSToFiles_checkBox_Newer->setChecked(wiiBaFuSettings.value("WBFStoFiles/Newer", QVariant(true)).toBool());
    ui->WBFSToFiles_checkBox_Update->setChecked(wiiBaFuSettings.value("WBFStoFiles/Update", QVariant(true)).toBool());
    ui->WBFSToFiles_checkBox_Overwrite->setChecked(wiiBaFuSettings.value("WBFStoFiles/Overwrite", QVariant(false)).toBool());

    ui->removeFromWBFS_checkBox_Force->setChecked(wiiBaFuSettings.value("RemoveFromWBFS/Force", QVariant(false)).toBool());
    ui->removeFromWBFS_checkBox_Test->setCheckable(wiiBaFuSettings.value("RemoveFromWBFS/Test", QVariant(false)).toBool());

    ui->checkWBFS_checkBox_Repair->setChecked(wiiBaFuSettings.value("CheckWBFS/Repair", QVariant(true)).toBool());
    ui->checkWBFS_checkBox_Test->setChecked(wiiBaFuSettings.value("CheckWBFS/Test", QVariant(false)).toBool());

    ui->repairWBFS_checkBox_FBT->setChecked(wiiBaFuSettings.value("RepairWBFS/FBT", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_INODES->setChecked(wiiBaFuSettings.value("RepairWBFS/INODES", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMALL->setChecked(wiiBaFuSettings.value("RepairWBFS/RM-ALL", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMEMPTY->setChecked(wiiBaFuSettings.value("RepairWBFS/RM-EMTPY", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMFREE->setChecked(wiiBaFuSettings.value("RepairWBFS/RM-FREE", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMINVALID->setChecked(wiiBaFuSettings.value("RepairWBFS/RM-INVALID", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMOVERLAP->setChecked(wiiBaFuSettings.value("RepairWBFS/RM-OVERLAP", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_STANDARD->setChecked(wiiBaFuSettings.value("RepairWBFS/STANDARD", QVariant(true)).toBool());
}

void Settings::save() {
    QSettings wiiBaFuSettings("WiiBaFu", "wiibafu");

    wiiBaFuSettings.setValue("Main/Auto", ui->main_checkBox_Auto->checkState());
    wiiBaFuSettings.setValue("Main/WBFSPath", ui->main_lineEdit_WBFSPath->text());
    wiiBaFuSettings.setValue("Main/DVDDrivePath", ui->main_lineEdit_DVDDrivePath->text());
    wiiBaFuSettings.setValue("Main/LogOutput", ui->main_comboBox_LogOutput->currentIndex());

    wiiBaFuSettings.setValue("GameLists/ShowGrid", ui->gameLists_checkBox_ShowGrid->checkState());
    wiiBaFuSettings.setValue("GameLists/AlternatingRowColors", ui->gameLists_checkBox_AlternatingRowColors->checkState());
    wiiBaFuSettings.setValue("GameLists/ScrollMode", scrollMode());
    wiiBaFuSettings.setValue("GameLists/ResizeMode", resizeMode());

    wiiBaFuSettings.setValue("FilesToWBFS/Force", ui->filesToWBFS_checkBox_Force->checkState());
    wiiBaFuSettings.setValue("FilesToWBFS/Test", ui->filesToWBFS_checkBox_Test->checkState());
    wiiBaFuSettings.setValue("FilesToWBFS/Newer", ui->filesToWBFS_checkBox_Newer->checkState());
    wiiBaFuSettings.setValue("FilesToWBFS/Update", ui->filesToWBFS_checkBox_Update->checkState());
    wiiBaFuSettings.setValue("FilesToWBFS/Overwrite", ui->filesToWBFS_checkBox_Overwrite->checkState());

    wiiBaFuSettings.setValue("DVDtoWBFS/Force", ui->dvdToWBFS_checkBox_Force->checkState());
    wiiBaFuSettings.setValue("DVDtoWBFS/Test", ui->dvdToWBFS_checkBox_Test->checkState());
    wiiBaFuSettings.setValue("DVDtoWBFS/Newer", ui->dvdToWBFS_checkBox_Newer->checkState());
    wiiBaFuSettings.setValue("DVDtoWBFS/Update", ui->dvdToWBFS_checkBox_Update->checkState());
    wiiBaFuSettings.setValue("DVDtoWBFS/Overwrite", ui->dvdToWBFS_checkBox_Overwrite->checkState());

    wiiBaFuSettings.setValue("WBFStoFiles/Force", ui->WBFSToFiles_checkBox_Force->checkState());
    wiiBaFuSettings.setValue("WBFStoFiles/Test", ui->WBFSToFiles_checkBox_Test->checkState());
    wiiBaFuSettings.setValue("WBFStoFiles/Newer", ui->WBFSToFiles_checkBox_Newer->checkState());
    wiiBaFuSettings.setValue("WBFStoFiles/Update", ui->WBFSToFiles_checkBox_Update->checkState());
    wiiBaFuSettings.setValue("WBFStoFiles/Overwrite", ui->WBFSToFiles_checkBox_Overwrite->checkState());

    wiiBaFuSettings.setValue("RemoveFromWBFS/Force", ui->removeFromWBFS_checkBox_Force->checkState());
    wiiBaFuSettings.setValue("RemoveFromWBFS/Test", ui->removeFromWBFS_checkBox_Test->checkState());

    wiiBaFuSettings.setValue("CheckWBFS/Repair", ui->checkWBFS_checkBox_Repair->checkState());
    wiiBaFuSettings.setValue("CheckWBFS/Test", ui->checkWBFS_checkBox_Test->checkState());

    wiiBaFuSettings.setValue("RepairWBFS/FBT", ui->repairWBFS_checkBox_FBT->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/INODES", ui->repairWBFS_checkBox_INODES->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/RM-ALL", ui->repairWBFS_checkBox_RMALL->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/RM-EMTPY", ui->repairWBFS_checkBox_RMEMPTY->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/RM-FREE", ui->repairWBFS_checkBox_RMFREE->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/RM-INVALID", ui->repairWBFS_checkBox_RMINVALID->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/RM-OVERLAP", ui->repairWBFS_checkBox_RMOVERLAP->checkState());
    wiiBaFuSettings.setValue("RepairWBFS/STANDARD", ui->repairWBFS_checkBox_STANDARD->checkState());
}

void Settings::restoreDefaults(int index) {
    switch (index) {
        case 0:
                ui->main_checkBox_Auto->setChecked(true);
                ui->main_lineEdit_DVDDrivePath->setText("/cdrom");
                ui->main_comboBox_LogOutput->setCurrentIndex(0);
                break;
        case 1:
                ui->gameLists_checkBox_AlternatingRowColors->setChecked(true);
                ui->gameLists_checkBox_ShowGrid->setChecked(false);
                ui->gameLists_radioButton_ScrollPerPixel->setChecked(true);
                ui->gameLists_radioButton_ResizeToContents->setChecked(true);
                break;
        case 2:
                ui->filesToWBFS_checkBox_Force->setChecked(false);
                ui->filesToWBFS_checkBox_Test->setChecked(false);
                ui->filesToWBFS_checkBox_Newer->setChecked(true);
                ui->filesToWBFS_checkBox_Update->setChecked(true);
                ui->filesToWBFS_checkBox_Overwrite->setChecked(false);
                break;
        case 3:
                ui->dvdToWBFS_checkBox_Force->setChecked(false);
                ui->dvdToWBFS_checkBox_Test->setChecked(false);
                ui->dvdToWBFS_checkBox_Newer->setChecked(true);
                ui->dvdToWBFS_checkBox_Update->setChecked(true);
                ui->dvdToWBFS_checkBox_Overwrite->setChecked(false);
                break;
        case 4:
                ui->WBFSToFiles_checkBox_Force->setChecked(false);
                ui->WBFSToFiles_checkBox_Test->setChecked(false);
                ui->WBFSToFiles_checkBox_Newer->setChecked(true);
                ui->WBFSToFiles_checkBox_Update->setChecked(true);
                ui->WBFSToFiles_checkBox_Overwrite->setChecked(false);
                break;
        case 5:
                ui->removeFromWBFS_checkBox_Force->setChecked(false);
                ui->removeFromWBFS_checkBox_Test->setChecked(false);
                break;
        case 6:
                ui->checkWBFS_groupBox_RepairOptions->setEnabled(true);
                ui->checkWBFS_checkBox_Test->setChecked(false);
                ui->checkWBFS_checkBox_Repair->setChecked(true);
                ui->repairWBFS_checkBox_FBT->setChecked(true);
                ui->repairWBFS_checkBox_INODES->setChecked(true);
                ui->repairWBFS_checkBox_RMALL->setChecked(true);
                ui->repairWBFS_checkBox_RMEMPTY->setChecked(true);
                ui->repairWBFS_checkBox_RMFREE->setChecked(true);
                ui->repairWBFS_checkBox_RMINVALID->setChecked(true);
                ui->repairWBFS_checkBox_RMOVERLAP->setChecked(true);
                ui->repairWBFS_checkBox_STANDARD->setChecked(true);
                break;
    }
}

void Settings::setScrollMode(int mode) {
    switch (mode) {
        case QAbstractItemView::ScrollPerPixel:
            ui->gameLists_radioButton_ScrollPerPixel->setChecked(true);
            break;
    case QAbstractItemView::ScrollPerItem:
        ui->gameLists_radioButton_ScrollPerItem->setChecked(true);
        break;
    }
}

void Settings::setResizeMode(int mode) {
    switch (mode) {
        case QHeaderView::Fixed:
            ui->gameLists_radioButton_Fixed->setChecked(true);
            break;
        case QHeaderView::Interactive:
            ui->gameLists_radioButton_Interactive->setChecked(true);
            break;
        case QHeaderView::ResizeToContents:
            ui->gameLists_radioButton_ResizeToContents->setChecked(true);
            break;
        case QHeaderView::Stretch:
            ui->gameLists_radioButton_Sretch->setChecked(true);
            break;
    }
}

int Settings::scrollMode() {
    if (ui->gameLists_radioButton_ScrollPerPixel->isChecked()) {
        return QAbstractItemView::ScrollPerPixel;
    }
    else {
        return QAbstractItemView::ScrollPerItem;
    }
}

int Settings::resizeMode() {
    if (ui->gameLists_radioButton_Fixed->isChecked()) {
        return QHeaderView::Fixed;
    }
    else if (ui->gameLists_radioButton_Interactive->isChecked()) {
        return QHeaderView::Interactive;
    }
    else if (ui->gameLists_radioButton_ResizeToContents->isChecked()) {
        return QHeaderView::ResizeToContents;
    }
    else {
        return QHeaderView::Stretch;
    }
}

void Settings::on_main_checkBox_Auto_stateChanged(int state) {
    state ? ui->main_lineEdit_WBFSPath->setEnabled(false) : ui->main_lineEdit_WBFSPath->setEnabled(true);
    state ? ui->main_pushButton_WBFSOpenFile->setEnabled(false) : ui->main_pushButton_WBFSOpenFile->setEnabled(true);
    state ? ui->main_pushButton_WBFSOpenDirectory->setEnabled(false) : ui->main_pushButton_WBFSOpenDirectory->setEnabled(true);
}

void Settings::on_checkWBFS_checkBox_Repair_stateChanged(int state) {
    state ? ui->repairWBFS_checkBox_FBT->setChecked(true) : ui->repairWBFS_checkBox_FBT->setChecked(false);
    state ? ui->repairWBFS_checkBox_INODES->setChecked(true) : ui->repairWBFS_checkBox_INODES->setChecked(false);
    state ? ui->repairWBFS_checkBox_RMALL->setChecked(true) : ui->repairWBFS_checkBox_RMALL->setChecked(false);
    state ? ui->repairWBFS_checkBox_RMEMPTY->setChecked(true) : ui->repairWBFS_checkBox_RMEMPTY->setChecked(false);
    state ? ui->repairWBFS_checkBox_RMFREE->setChecked(true) : ui->repairWBFS_checkBox_RMFREE->setChecked(false);
    state ? ui->repairWBFS_checkBox_RMINVALID->setChecked(true) : ui->repairWBFS_checkBox_RMINVALID->setChecked(false);
    state ? ui->repairWBFS_checkBox_RMOVERLAP->setChecked(true) : ui->repairWBFS_checkBox_RMOVERLAP->setChecked(false);
    state ? ui->repairWBFS_checkBox_STANDARD->setChecked(true) : ui->repairWBFS_checkBox_STANDARD->setChecked(false);
}

Settings::~Settings() {
    delete ui;
}
