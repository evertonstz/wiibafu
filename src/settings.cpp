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
    setupGeometry();
    load();
}

void Settings::on_main_pushButton_PathToWITOpen_clicked() {
    QString path;

    if (!ui->main_lineEdit_PathToWIT->text().isEmpty()) {
        path = ui->main_lineEdit_PathToWIT->text();
    }
    else {
        path = QDir::homePath();
    }

    QString witPath = QFileDialog::getExistingDirectory(this, tr("Open path to WIT"), path, QFileDialog::ShowDirsOnly);

    if (!witPath.isEmpty()) {
        ui->main_lineEdit_PathToWIT->setText(witPath);
    }
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
    ui->main_lineEdit_PathToWIT->setText(WiiBaFuSettings.value("Main/PathToWIT", QVariant("")).toString());
    ui->main_checkBox_Auto->setChecked(WiiBaFuSettings.value("Main/Auto", QVariant(true)).toBool());
    ui->main_lineEdit_WBFSPath->setText(WiiBaFuSettings.value("Main/WBFSPath", QVariant("")).toString());
    ui->main_lineEdit_DVDDrivePath->setText(WiiBaFuSettings.value("Main/DVDDrivePath", QVariant("")).toString());
    ui->main_comboBox_Logging->setCurrentIndex(WiiBaFuSettings.value("Main/Logging", QVariant(0)).toInt());
    ui->main_comboBox_Language->setCurrentIndex(WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt());

    ui->gameLists_checkBox_ShowGrid->setChecked(WiiBaFuSettings.value("GameLists/ShowGrid", QVariant(false)).toBool());
    ui->gameLists_checkBox_AlternatingRowColors->setChecked(WiiBaFuSettings.value("GameLists/AlternatingRowColors", QVariant(true)).toBool());
    ui->gameLists_checkBox_ToolTips->setChecked(WiiBaFuSettings.value("GameLists/ToolTips", QVariant(false)).toBool());

    setScrollMode(WiiBaFuSettings.value("GameLists/ScrollMode", QVariant(QAbstractItemView::ScrollPerPixel)).toInt());
    setResizeMode(WiiBaFuSettings.value("GameLists/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());

    ui->filesToWBFS_checkBox_Force->setChecked(WiiBaFuSettings.value("FilesToWBFS/Force", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("FilesToWBFS/Test", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Newer->setChecked(WiiBaFuSettings.value("FilesToWBFS/Newer", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Update->setChecked(WiiBaFuSettings.value("FilesToWBFS/Update", QVariant(false)).toBool());
    ui->filesToWBFS_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("FilesToWBFS/Overwrite", QVariant(false)).toBool());
    ui->filesColumn_checkBox_ID->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnID", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Name->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnName", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Title->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnTitle", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Region->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnRegion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Size->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnSize", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Insertion->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnInsertion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastModification->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnLastModification", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastStatusChange->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnLastStatusChange", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastAccess->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnLastAccess", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Type->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnType", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Source->setChecked(WiiBaFuSettings.value("FilesToWBFS/columnSource", QVariant(true)).toBool());

    ui->gamesToImage_checkBox_Test->setChecked(WiiBaFuSettings.value("GamesToImage/Test", QVariant(false)).toBool());
    ui->gamesToImage_checkBox_Update->setChecked(WiiBaFuSettings.value("GamesToImage/Update", QVariant(false)).toBool());
    ui->gamesToImage_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("GamesToImage/Overwrite", QVariant(false)).toBool());

    ui->WBFSToFiles_checkBox_Force->setChecked(WiiBaFuSettings.value("WBFStoFiles/Force", QVariant(false)).toBool());
    ui->WBFSToFiles_checkBox_Test->setChecked(WiiBaFuSettings.value("WBFStoFiles/Test", QVariant(false)).toBool());
    ui->WBFSToFiles_checkBox_Update->setChecked(WiiBaFuSettings.value("WBFStoFiles/Update", QVariant(false)).toBool());
    ui->WBFSToFiles_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("WBFStoFiles/Overwrite", QVariant(false)).toBool());
    ui->wbfsColumn_checkBox_ID->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnID", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Name->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnName", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Title->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnTitle", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Region->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnRegion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Size->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnSize", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_UsedBlocks->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnUsedBlocks", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Insertion->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnInsertion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastModification->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnLastModification", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastStatusChange->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnLastStatusChange", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastAccess->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnLastAccess", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Type->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnType", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_WBFSSlot->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnWBFSSlot", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Source->setChecked(WiiBaFuSettings.value("WBFStoFiles/columnSource", QVariant(true)).toBool());

    ui->removeFromWBFS_checkBox_Force->setChecked(WiiBaFuSettings.value("RemoveFromWBFS/Force", QVariant(false)).toBool());
    ui->removeFromWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("RemoveFromWBFS/Test", QVariant(false)).toBool());

    ui->checkWBFS_checkBox_Repair->setChecked(WiiBaFuSettings.value("CheckWBFS/Repair", QVariant(true)).toBool());
    ui->checkWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("CheckWBFS/Test", QVariant(false)).toBool());

    ui->repairWBFS_checkBox_FBT->setChecked(WiiBaFuSettings.value("RepairWBFS/FBT", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_INODES->setChecked(WiiBaFuSettings.value("RepairWBFS/INODES", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMEMPTY->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-EMTPY", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMFREE->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-FREE", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMINVALID->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-INVALID", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMOVERLAP->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-OVERLAP", QVariant(true)).toBool());
}

void Settings::save() {
    WiiBaFuSettings.setValue("Main/PathToWIT", ui->main_lineEdit_PathToWIT->text());
    WiiBaFuSettings.setValue("Main/Auto", ui->main_checkBox_Auto->checkState());
    WiiBaFuSettings.setValue("Main/WBFSPath", ui->main_lineEdit_WBFSPath->text());
    WiiBaFuSettings.setValue("Main/DVDDrivePath", ui->main_lineEdit_DVDDrivePath->text());
    WiiBaFuSettings.setValue("Main/Logging", ui->main_comboBox_Logging->currentIndex());
    WiiBaFuSettings.setValue("Main/Language", ui->main_comboBox_Language->currentIndex());

    WiiBaFuSettings.setValue("GameLists/ShowGrid", ui->gameLists_checkBox_ShowGrid->checkState());
    WiiBaFuSettings.setValue("GameLists/AlternatingRowColors", ui->gameLists_checkBox_AlternatingRowColors->checkState());
    WiiBaFuSettings.setValue("GameLists/ToolTips", ui->gameLists_checkBox_ToolTips->checkState());
    WiiBaFuSettings.setValue("GameLists/ScrollMode", scrollMode());
    WiiBaFuSettings.setValue("GameLists/ResizeMode", resizeMode());

    WiiBaFuSettings.setValue("FilesToWBFS/Force", ui->filesToWBFS_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/Test", ui->filesToWBFS_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/Newer", ui->filesToWBFS_checkBox_Newer->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/Update", ui->filesToWBFS_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/Overwrite", ui->filesToWBFS_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("FilesToWBFS/columnID", ui->filesColumn_checkBox_ID->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnName", ui->filesColumn_checkBox_Name->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnTitle", ui->filesColumn_checkBox_Title->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnRegion", ui->filesColumn_checkBox_Region->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnSize", ui->filesColumn_checkBox_Size->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnInsertion", ui->filesColumn_checkBox_Insertion->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnLastModification", ui->filesColumn_checkBox_LastModification->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnLastStatusChange", ui->filesColumn_checkBox_LastStatusChange->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnLastAccess", ui->filesColumn_checkBox_LastAccess->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnType", ui->filesColumn_checkBox_Type->checkState());
    WiiBaFuSettings.setValue("FilesToWBFS/columnSource", ui->filesColumn_checkBox_Source->checkState());

    WiiBaFuSettings.setValue("GamesToImage/Test", ui->gamesToImage_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("GamesToImage/Update", ui->gamesToImage_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("GamesToImage/Overwrite", ui->gamesToImage_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("WBFStoFiles/Force", ui->WBFSToFiles_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/Test", ui->WBFSToFiles_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/Update", ui->WBFSToFiles_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/Overwrite", ui->WBFSToFiles_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("WBFStoFiles/columnID", ui->wbfsColumn_checkBox_ID->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnName", ui->wbfsColumn_checkBox_Name->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnTitle", ui->wbfsColumn_checkBox_Title->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnRegion", ui->wbfsColumn_checkBox_Region->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnSize", ui->wbfsColumn_checkBox_Size->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnUsedBlocks", ui->wbfsColumn_checkBox_UsedBlocks->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnInsertion", ui->wbfsColumn_checkBox_Insertion->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnLastModification", ui->wbfsColumn_checkBox_LastModification->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnLastStatusChange", ui->wbfsColumn_checkBox_LastStatusChange->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnLastAccess", ui->wbfsColumn_checkBox_LastAccess->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnType", ui->wbfsColumn_checkBox_Type->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnWBFSSlot", ui->wbfsColumn_checkBox_WBFSSlot->checkState());
    WiiBaFuSettings.setValue("WBFStoFiles/columnSource", ui->wbfsColumn_checkBox_Source->checkState());

    WiiBaFuSettings.setValue("RemoveFromWBFS/Force", ui->removeFromWBFS_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("RemoveFromWBFS/Test", ui->removeFromWBFS_checkBox_Test->checkState());

    WiiBaFuSettings.setValue("CheckWBFS/Repair", ui->checkWBFS_checkBox_Repair->checkState());
    WiiBaFuSettings.setValue("CheckWBFS/Test", ui->checkWBFS_checkBox_Test->checkState());

    WiiBaFuSettings.setValue("RepairWBFS/FBT", ui->repairWBFS_checkBox_FBT->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/INODES", ui->repairWBFS_checkBox_INODES->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-EMTPY", ui->repairWBFS_checkBox_RMEMPTY->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-FREE", ui->repairWBFS_checkBox_RMFREE->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-INVALID", ui->repairWBFS_checkBox_RMINVALID->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-OVERLAP", ui->repairWBFS_checkBox_RMOVERLAP->checkState());
}

void Settings::restoreDefaults(int index) {
    switch (index) {
        case 0:
                ui->main_lineEdit_PathToWIT->setText("/usr/local/bin");
                ui->main_checkBox_Auto->setChecked(true);
                ui->main_lineEdit_DVDDrivePath->setText("/dev/sr0");
                ui->main_comboBox_Logging->setCurrentIndex(0);
                ui->main_comboBox_Language->setCurrentIndex(0);
                break;
        case 1:
                ui->gameLists_checkBox_AlternatingRowColors->setChecked(true);
                ui->gameLists_checkBox_ShowGrid->setChecked(false);
                ui->gameLists_checkBox_ToolTips->setChecked(false);
                ui->gameLists_radioButton_ScrollPerPixel->setChecked(true);
                ui->gameLists_radioButton_ResizeToContents->setChecked(true);
                break;
        case 2:
                ui->filesToWBFS_checkBox_Force->setChecked(false);
                ui->filesToWBFS_checkBox_Test->setChecked(false);
                ui->filesToWBFS_checkBox_Newer->setChecked(false);
                ui->filesToWBFS_checkBox_Update->setChecked(false);
                ui->filesToWBFS_checkBox_Overwrite->setChecked(false);
                ui->filesColumn_checkBox_ID->setChecked(true);
                ui->filesColumn_checkBox_Name->setChecked(true);
                ui->filesColumn_checkBox_Title->setChecked(true);
                ui->filesColumn_checkBox_Region->setChecked(true);
                ui->filesColumn_checkBox_Size->setChecked(true);
                ui->filesColumn_checkBox_Insertion->setChecked(true);
                ui->filesColumn_checkBox_LastModification->setChecked(true);
                ui->filesColumn_checkBox_LastStatusChange->setChecked(true);
                ui->filesColumn_checkBox_LastAccess->setChecked(true);
                ui->filesColumn_checkBox_Type->setChecked(true);
                ui->filesColumn_checkBox_Source->setChecked(true);
                break;
        case 3:
                ui->gamesToImage_checkBox_Test->setChecked(false);
                ui->gamesToImage_checkBox_Update->setChecked(false);
                ui->gamesToImage_checkBox_Overwrite->setChecked(false);
                break;
        case 4:
                ui->WBFSToFiles_checkBox_Force->setChecked(false);
                ui->WBFSToFiles_checkBox_Test->setChecked(false);
                ui->WBFSToFiles_checkBox_Update->setChecked(false);
                ui->WBFSToFiles_checkBox_Overwrite->setChecked(false);
                ui->wbfsColumn_checkBox_ID->setChecked(true);
                ui->wbfsColumn_checkBox_Name->setChecked(true);
                ui->wbfsColumn_checkBox_Title->setChecked(true);
                ui->wbfsColumn_checkBox_Region->setChecked(true);
                ui->wbfsColumn_checkBox_Size->setChecked(true);
                ui->wbfsColumn_checkBox_UsedBlocks->setChecked(true);
                ui->wbfsColumn_checkBox_Insertion->setChecked(true);
                ui->wbfsColumn_checkBox_LastModification->setChecked(true);
                ui->wbfsColumn_checkBox_LastStatusChange->setChecked(true);
                ui->wbfsColumn_checkBox_LastAccess->setChecked(true);
                ui->wbfsColumn_checkBox_Type->setChecked(true);
                ui->wbfsColumn_checkBox_WBFSSlot->setChecked(true);
                ui->wbfsColumn_checkBox_Source->setChecked(true);
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
                ui->repairWBFS_checkBox_RMEMPTY->setChecked(false);
                ui->repairWBFS_checkBox_RMFREE->setChecked(false);
                ui->repairWBFS_checkBox_RMINVALID->setChecked(false);
                ui->repairWBFS_checkBox_RMOVERLAP->setChecked(false);
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
    ui->repairWBFS_checkBox_RMEMPTY->setChecked(false);
    ui->repairWBFS_checkBox_RMFREE->setChecked(false);
    ui->repairWBFS_checkBox_RMINVALID->setChecked(false);
    ui->repairWBFS_checkBox_RMOVERLAP->setChecked(false);
}

void Settings::setupGeometry() {
    if (WiiBaFuSettings.contains("Settings/x")) {
        QRect rect;
        rect.setX(WiiBaFuSettings.value("Settings/x", QVariant(0)).toInt());
        rect.setY(WiiBaFuSettings.value("Settings/y", QVariant(0)).toInt());
        rect.setWidth(WiiBaFuSettings.value("Settings/width", QVariant(800)).toInt());
        rect.setHeight(WiiBaFuSettings.value("Settings/height", QVariant(600)).toInt());

        this->setGeometry(rect);
    }
}

void Settings::saveGeometry() {
    WiiBaFuSettings.setValue("Settings/x", this->geometry().x());
    WiiBaFuSettings.setValue("Settings/y", this->geometry().y());
    WiiBaFuSettings.setValue("Settings/width", this->geometry().width());
    WiiBaFuSettings.setValue("Settings/height", this->geometry().height());
}

Settings::~Settings() {
    saveGeometry();
    delete ui;
}
