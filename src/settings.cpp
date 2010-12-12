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

    #ifdef Q_OS_MACX
        ui->main_label_MacOSXStyle->setEnabled(true);
        ui->main_radioButton_MacOSXStyle_Aqua->setEnabled(true);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setEnabled(true);
    #else
        ui->main_label_MacOSXStyle->setEnabled(false);
        ui->main_radioButton_MacOSXStyle_Aqua->setEnabled(false);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setEnabled(false);
    #endif
}

void Settings::on_wit_pushButton_PathToWITOpen_clicked() {
    QString path;

    if (!ui->wit_lineEdit_PathToWIT->text().isEmpty()) {
        path = ui->wit_lineEdit_PathToWIT->text();
    }
    else {
        path = QDir::homePath();
    }

    QString witPath = QFileDialog::getExistingDirectory(this, tr("Open path to WIT"), path, QFileDialog::ShowDirsOnly);

    if (!witPath.isEmpty()) {
        ui->wit_lineEdit_PathToWIT->setText(witPath);
    }
}

void Settings::on_wit_pushButton_WBFSOpenFile_clicked() {
    QString path;

    if (!ui->wit_lineEdit_WBFSPath->text().isEmpty()) {
        path = ui->wit_lineEdit_WBFSPath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString wbfsFile = QFileDialog::getOpenFileName(this, tr("Open WBFS file"), path, "WBFS files (*.wbfs)");

    if (!wbfsFile.isEmpty()) {
        ui->wit_lineEdit_WBFSPath->setText(wbfsFile);
    }
}

void Settings::on_wit_pushButton_WBFSOpenDirectory_clicked() {
    QString path;

    if (!ui->wit_lineEdit_WBFSPath->text().isEmpty()) {
        path = ui->wit_lineEdit_WBFSPath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString wbfsDirectory = QFileDialog::getExistingDirectory(this, tr("Open WBFS directory"), path, QFileDialog::ShowDirsOnly);

    if (!wbfsDirectory.isEmpty()) {
        ui->wit_lineEdit_WBFSPath->setText(wbfsDirectory);
    }
}

void Settings::on_wit_pushButton_DVDDriveOpenDirectory_clicked() {
    QString path;

    if (!ui->wit_lineEdit_DVDDrivePath->text().isEmpty()) {
        path = ui->wit_lineEdit_DVDDrivePath->text();
    }
    else {
        path = QDir::homePath();
    }

    QString dvdPath = QFileDialog::getExistingDirectory(this, tr("Open DVD path"), path, QFileDialog::ShowDirsOnly);

    if (!dvdPath.isEmpty()) {
        ui->wit_lineEdit_DVDDrivePath->setText(dvdPath);
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
    ui->main_checkBox_useProxy->setChecked(WiiBaFuSettings.value("Main/UseProxy", QVariant(false)).toBool());
    ui->main_lineEdit_proxyHost->setText(WiiBaFuSettings.value("Main/ProxyHost", QVariant("")).toString());
    ui->main_lineEdit_proxyPort->setText(WiiBaFuSettings.value("Main/ProxyPort", QVariant("")).toString());
    ui->main_lineEdit_proxyUser->setText(WiiBaFuSettings.value("Main/ProxyUser", QVariant("")).toString());
    ui->main_lineEdit_proxyPassword->setText(WiiBaFuSettings.value("Main/ProxyPassword", QVariant("")).toString());
    ui->main_comboBox_proxyType->setCurrentIndex(WiiBaFuSettings.value("Main/ProxyType", QVariant(0)).toInt());
    ui->main_comboBox_Language->setCurrentIndex(WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt());
    ui->main_comboBox_Logging->setCurrentIndex(WiiBaFuSettings.value("Main/Logging", QVariant(0)).toInt());
    ui->main_checkBox_LogWitCommandLine->setChecked(WiiBaFuSettings.value("Main/LogWitCommandLine", QVariant(true)).toBool());

    if (WiiBaFuSettings.value("Main/MacOSXStyle", QVariant("Aqua")) == "Aqua") {
        ui->main_radioButton_MacOSXStyle_Aqua->setChecked(true);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(false);
    }
    else if (WiiBaFuSettings.value("Main/MacOSXStyle", QVariant("Aqua")) == "BrushedMetal") {
        ui->main_radioButton_MacOSXStyle_Aqua->setChecked(false);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(true);
    }

    ui->wit_lineEdit_PathToWIT->setText(WiiBaFuSettings.value("WIT/PathToWIT", QVariant(QDir::currentPath().append("/wit"))).toString());
    ui->wit_checkBox_Auto->setChecked(WiiBaFuSettings.value("WIT/Auto", QVariant(true)).toBool());
    ui->wit_lineEdit_WBFSPath->setText(WiiBaFuSettings.value("WIT/WBFSPath", QVariant("")).toString());
    ui->wit_lineEdit_DVDDrivePath->setText(WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/dev/sr0")).toString());

    ui->gameLists_checkBox_ShowGrid->setChecked(WiiBaFuSettings.value("GameListBehavior/ShowGrid", QVariant(false)).toBool());
    ui->gameLists_checkBox_AlternatingRowColors->setChecked(WiiBaFuSettings.value("GameListBehavior/AlternatingRowColors", QVariant(true)).toBool());
    ui->gameLists_checkBox_ToolTips->setChecked(WiiBaFuSettings.value("GameListBehavior/ToolTips", QVariant(false)).toBool());
    setScrollMode(WiiBaFuSettings.value("GameListBehavior/ScrollMode", QVariant(QAbstractItemView::ScrollPerPixel)).toInt());
    setResizeMode(WiiBaFuSettings.value("GameListBehavior/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());
    ui->gameLists_comboBox_SelectionMode->setCurrentIndex(WiiBaFuSettings.value("GameListBehavior/SelectionMode", QVariant(3)).toInt());

    ui->filesColumn_checkBox_ID->setChecked(WiiBaFuSettings.value("FilesGameList/columnID", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Name->setChecked(WiiBaFuSettings.value("FilesGameList/columnName", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Title->setChecked(WiiBaFuSettings.value("FilesGameList/columnTitle", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Region->setChecked(WiiBaFuSettings.value("FilesGameList/columnRegion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Size->setChecked(WiiBaFuSettings.value("FilesGameList/columnSize", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Insertion->setChecked(WiiBaFuSettings.value("FilesGameList/columnInsertion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastModification->setChecked(WiiBaFuSettings.value("FilesGameList/columnLastModification", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastStatusChange->setChecked(WiiBaFuSettings.value("FilesGameList/columnLastStatusChange", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastAccess->setChecked(WiiBaFuSettings.value("FilesGameList/columnLastAccess", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Type->setChecked(WiiBaFuSettings.value("FilesGameList/columnType", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Source->setChecked(WiiBaFuSettings.value("FilesGameList/columnSource", QVariant(true)).toBool());

    ui->wbfsColumn_checkBox_ID->setChecked(WiiBaFuSettings.value("WBFSGameList/columnID", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Name->setChecked(WiiBaFuSettings.value("WBFSGameList/columnName", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Title->setChecked(WiiBaFuSettings.value("WBFSGameList/columnTitle", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Region->setChecked(WiiBaFuSettings.value("WBFSGameList/columnRegion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Size->setChecked(WiiBaFuSettings.value("WBFSGameList/columnSize", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_UsedBlocks->setChecked(WiiBaFuSettings.value("WBFSGameList/columnUsedBlocks", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Insertion->setChecked(WiiBaFuSettings.value("WBFSGameList/columnInsertion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastModification->setChecked(WiiBaFuSettings.value("WBFSGameList/columnLastModification", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastStatusChange->setChecked(WiiBaFuSettings.value("WBFSGameList/columnLastStatusChange", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastAccess->setChecked(WiiBaFuSettings.value("WBFSGameList/columnLastAccess", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Type->setChecked(WiiBaFuSettings.value("WBFSGameList/columnType", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_WBFSSlot->setChecked(WiiBaFuSettings.value("WBFSGameList/columnWBFSSlot", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Source->setChecked(WiiBaFuSettings.value("WBFSGameList/columnSource", QVariant(true)).toBool());

    ui->transferToWBFS_checkBox_Force->setChecked(WiiBaFuSettings.value("TransferToWBFS/Force", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("TransferToWBFS/Test", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Newer->setChecked(WiiBaFuSettings.value("TransferToWBFS/Newer", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Update->setChecked(WiiBaFuSettings.value("TransferToWBFS/Update", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("TransferToWBFS/Overwrite", QVariant(false)).toBool());

    ui->transferToImageFST_checkBox_Test->setChecked(WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool());
    ui->transferToImageFST_checkBox_Update->setChecked(WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool());
    ui->transferToImageFST_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool());

    ui->transferFromWBFS_checkBox_Force->setChecked(WiiBaFuSettings.value("TransferFromWBFS/Force", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("TransferFromWBFS/Test", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Update->setChecked(WiiBaFuSettings.value("TransferFromWBFS/Update", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Overwrite->setChecked(WiiBaFuSettings.value("TransferFromWBFS/Overwrite", QVariant(false)).toBool());

    ui->checkWBFS_checkBox_Repair->setChecked(WiiBaFuSettings.value("CheckWBFS/Repair", QVariant(true)).toBool());
    ui->checkWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("CheckWBFS/Test", QVariant(false)).toBool());

    ui->repairWBFS_checkBox_FBT->setChecked(WiiBaFuSettings.value("RepairWBFS/FBT", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_INODES->setChecked(WiiBaFuSettings.value("RepairWBFS/INODES", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMEMPTY->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-EMTPY", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMFREE->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-FREE", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMINVALID->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-INVALID", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMOVERLAP->setChecked(WiiBaFuSettings.value("RepairWBFS/RM-OVERLAP", QVariant(false)).toBool());

    ui->removeFromWBFS_checkBox_Force->setChecked(WiiBaFuSettings.value("RemoveFromWBFS/Force", QVariant(false)).toBool());
    ui->removeFromWBFS_checkBox_Test->setChecked(WiiBaFuSettings.value("RemoveFromWBFS/Test", QVariant(false)).toBool());
}

void Settings::save() {
    WiiBaFuSettings.setValue("Main/UseProxy", ui->main_checkBox_useProxy->checkState());
    WiiBaFuSettings.setValue("Main/ProxyHost", ui->main_lineEdit_proxyHost->text());
    WiiBaFuSettings.setValue("Main/ProxyPort", ui->main_lineEdit_proxyPort->text());
    WiiBaFuSettings.setValue("Main/ProxyUser", ui->main_lineEdit_proxyUser->text());
    WiiBaFuSettings.setValue("Main/ProxyPassword", ui->main_lineEdit_proxyPassword->text());
    WiiBaFuSettings.setValue("Main/ProxyType", ui->main_comboBox_proxyType->currentIndex());
    WiiBaFuSettings.setValue("Main/GameLanguage", ui->main_comboBox_Language->currentIndex());
    WiiBaFuSettings.setValue("Main/Logging", ui->main_comboBox_Logging->currentIndex());
    WiiBaFuSettings.setValue("Main/LogWitCommandLine", ui->main_checkBox_LogWitCommandLine->checkState());
    WiiBaFuSettings.setValue("Main/MacOSXStyle", macOSXStyle());

    WiiBaFuSettings.setValue("WIT/PathToWIT", ui->wit_lineEdit_PathToWIT->text());
    WiiBaFuSettings.setValue("WIT/Auto", ui->wit_checkBox_Auto->checkState());
    WiiBaFuSettings.setValue("WIT/WBFSPath", ui->wit_lineEdit_WBFSPath->text());
    WiiBaFuSettings.setValue("WIT/DVDDrivePath", ui->wit_lineEdit_DVDDrivePath->text());

    WiiBaFuSettings.setValue("GameListBehavior/ShowGrid", ui->gameLists_checkBox_ShowGrid->checkState());
    WiiBaFuSettings.setValue("GameListBehavior/AlternatingRowColors", ui->gameLists_checkBox_AlternatingRowColors->checkState());
    WiiBaFuSettings.setValue("GameListBehavior/ToolTips", ui->gameLists_checkBox_ToolTips->checkState());
    WiiBaFuSettings.setValue("GameListBehavior/ScrollMode", scrollMode());
    WiiBaFuSettings.setValue("GameListBehavior/ResizeMode", resizeMode());
    WiiBaFuSettings.setValue("GameListBehavior/SelectionMode", ui->gameLists_comboBox_SelectionMode->currentIndex());

    WiiBaFuSettings.setValue("FilesGameList/columnID", ui->filesColumn_checkBox_ID->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnName", ui->filesColumn_checkBox_Name->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnTitle", ui->filesColumn_checkBox_Title->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnRegion", ui->filesColumn_checkBox_Region->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnSize", ui->filesColumn_checkBox_Size->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnInsertion", ui->filesColumn_checkBox_Insertion->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnLastModification", ui->filesColumn_checkBox_LastModification->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnLastStatusChange", ui->filesColumn_checkBox_LastStatusChange->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnLastAccess", ui->filesColumn_checkBox_LastAccess->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnType", ui->filesColumn_checkBox_Type->checkState());
    WiiBaFuSettings.setValue("FilesGameList/columnSource", ui->filesColumn_checkBox_Source->checkState());

    WiiBaFuSettings.setValue("WBFSGameList/columnID", ui->wbfsColumn_checkBox_ID->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnName", ui->wbfsColumn_checkBox_Name->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnTitle", ui->wbfsColumn_checkBox_Title->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnRegion", ui->wbfsColumn_checkBox_Region->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnSize", ui->wbfsColumn_checkBox_Size->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnUsedBlocks", ui->wbfsColumn_checkBox_UsedBlocks->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnInsertion", ui->wbfsColumn_checkBox_Insertion->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnLastModification", ui->wbfsColumn_checkBox_LastModification->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnLastStatusChange", ui->wbfsColumn_checkBox_LastStatusChange->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnLastAccess", ui->wbfsColumn_checkBox_LastAccess->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnType", ui->wbfsColumn_checkBox_Type->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnWBFSSlot", ui->wbfsColumn_checkBox_WBFSSlot->checkState());
    WiiBaFuSettings.setValue("WBFSGameList/columnSource", ui->wbfsColumn_checkBox_Source->checkState());

    WiiBaFuSettings.setValue("TransferToWBFS/Force", ui->transferToWBFS_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("TransferToWBFS/Test", ui->transferToWBFS_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("TransferToWBFS/Newer", ui->transferToWBFS_checkBox_Newer->checkState());
    WiiBaFuSettings.setValue("TransferToWBFS/Update", ui->transferToWBFS_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("TransferToWBFS/Overwrite", ui->transferToWBFS_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("TransferToImageFST/Test", ui->transferToImageFST_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("TransferToImageFST/Update", ui->transferToImageFST_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("TransferToImageFST/Overwrite", ui->transferToImageFST_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("TransferFromWBFS/Force", ui->transferFromWBFS_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("TransferFromWBFS/Test", ui->transferFromWBFS_checkBox_Test->checkState());
    WiiBaFuSettings.setValue("TransferFromWBFS/Update", ui->transferFromWBFS_checkBox_Update->checkState());
    WiiBaFuSettings.setValue("TransferFromWBFS/Overwrite", ui->transferFromWBFS_checkBox_Overwrite->checkState());

    WiiBaFuSettings.setValue("CheckWBFS/Repair", ui->checkWBFS_checkBox_Repair->checkState());
    WiiBaFuSettings.setValue("CheckWBFS/Test", ui->checkWBFS_checkBox_Test->checkState());

    WiiBaFuSettings.setValue("RepairWBFS/FBT", ui->repairWBFS_checkBox_FBT->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/INODES", ui->repairWBFS_checkBox_INODES->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-EMTPY", ui->repairWBFS_checkBox_RMEMPTY->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-FREE", ui->repairWBFS_checkBox_RMFREE->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-INVALID", ui->repairWBFS_checkBox_RMINVALID->checkState());
    WiiBaFuSettings.setValue("RepairWBFS/RM-OVERLAP", ui->repairWBFS_checkBox_RMOVERLAP->checkState());

    WiiBaFuSettings.setValue("RemoveFromWBFS/Force", ui->removeFromWBFS_checkBox_Force->checkState());
    WiiBaFuSettings.setValue("RemoveFromWBFS/Test", ui->removeFromWBFS_checkBox_Test->checkState());
}

void Settings::restoreDefaults(int index) {
    switch (index) {
        case 0: // Main
                ui->main_checkBox_useProxy->setChecked(false);
                ui->main_lineEdit_proxyHost->setEnabled(false);
                ui->main_lineEdit_proxyPort->setEnabled(false);
                ui->main_lineEdit_proxyUser->setEnabled(false);
                ui->main_lineEdit_proxyPassword->setEnabled(false);
                ui->main_comboBox_proxyType->setCurrentIndex(3);
                ui->main_comboBox_Language->setCurrentIndex(0);
                ui->main_comboBox_Logging->setCurrentIndex(0);
                ui->main_checkBox_LogWitCommandLine->setChecked(true);
                ui->main_radioButton_MacOSXStyle_Aqua->setChecked(true);
                ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(false);
                break;
        case 1: // WIT
                ui->wit_lineEdit_PathToWIT->setText(QDir::currentPath().append("/wit"));
                ui->wit_checkBox_Auto->setChecked(true);
                ui->wit_lineEdit_DVDDrivePath->setText("/dev/sr0");
                break;
        case 2: // Game list behavior
                ui->gameLists_checkBox_AlternatingRowColors->setChecked(true);
                ui->gameLists_checkBox_ShowGrid->setChecked(false);
                ui->gameLists_checkBox_ToolTips->setChecked(false);
                ui->gameLists_radioButton_ScrollPerPixel->setChecked(true);
                ui->gameLists_radioButton_ResizeToContents->setChecked(true);
                ui->gameLists_comboBox_SelectionMode->setCurrentIndex(3);
                break;
        case 3: // Game list columns
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
        case 4: // Transfer options
                ui->transferToWBFS_checkBox_Force->setChecked(false);
                ui->transferToWBFS_checkBox_Test->setChecked(false);
                ui->transferToWBFS_checkBox_Newer->setChecked(false);
                ui->transferToWBFS_checkBox_Update->setChecked(false);
                ui->transferToWBFS_checkBox_Overwrite->setChecked(false);
                ui->transferToImageFST_checkBox_Test->setChecked(false);
                ui->transferToImageFST_checkBox_Update->setChecked(false);
                ui->transferToImageFST_checkBox_Overwrite->setChecked(false);
                ui->transferFromWBFS_checkBox_Force->setChecked(false);
                ui->transferFromWBFS_checkBox_Test->setChecked(false);
                ui->transferFromWBFS_checkBox_Update->setChecked(false);
                ui->transferFromWBFS_checkBox_Overwrite->setChecked(false);
                break;
        case 5: // WBFS options
                ui->checkWBFS_groupBox_RepairOptions->setEnabled(true);
                ui->checkWBFS_checkBox_Test->setChecked(false);
                ui->checkWBFS_checkBox_Repair->setChecked(true);
                ui->repairWBFS_checkBox_FBT->setChecked(true);
                ui->repairWBFS_checkBox_INODES->setChecked(true);
                ui->repairWBFS_checkBox_RMEMPTY->setChecked(false);
                ui->repairWBFS_checkBox_RMFREE->setChecked(false);
                ui->repairWBFS_checkBox_RMINVALID->setChecked(false);
                ui->repairWBFS_checkBox_RMOVERLAP->setChecked(false);
                ui->removeFromWBFS_checkBox_Force->setChecked(false);
                ui->removeFromWBFS_checkBox_Test->setChecked(false);
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

QString Settings::macOSXStyle() {
    if (ui->main_radioButton_MacOSXStyle_Aqua->isChecked()) {
        return "Aqua";
    }
    else if (ui->main_radioButton_MacOSXStyle_BrushedMetal->isChecked()) {
        return "BrushedMetal";
    }
    else {
        return "";
    }
}

void Settings::on_main_checkBox_useProxy_stateChanged(int state) {
    state ? ui->main_lineEdit_proxyHost->setEnabled(true) : ui->main_lineEdit_proxyHost->setEnabled(false);
    state ? ui->main_lineEdit_proxyPort->setEnabled(true) : ui->main_lineEdit_proxyPort->setEnabled(false);
    state ? ui->main_lineEdit_proxyUser->setEnabled(true) : ui->main_lineEdit_proxyUser->setEnabled(false);
    state ? ui->main_lineEdit_proxyPassword->setEnabled(true) : ui->main_lineEdit_proxyPassword->setEnabled(false);
    state ? ui->main_comboBox_proxyType->setEnabled(true) : ui->main_comboBox_proxyType->setEnabled(false);
}

void Settings::on_wit_checkBox_Auto_stateChanged(int state) {
    state ? ui->wit_lineEdit_WBFSPath->setEnabled(false) : ui->wit_lineEdit_WBFSPath->setEnabled(true);
    state ? ui->wit_pushButton_WBFSOpenFile->setEnabled(false) : ui->wit_pushButton_WBFSOpenFile->setEnabled(true);
    state ? ui->wit_pushButton_WBFSOpenDirectory->setEnabled(false) : ui->wit_pushButton_WBFSOpenDirectory->setEnabled(true);
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

void Settings::setMacOSXStyle() {
    if (WiiBaFuSettings.value("Main/MacOSXStyle", QVariant("Aqua")).toString().contains("BrushedMetal")) {
        this->setAttribute(Qt::WA_MacBrushedMetal, true);
    }
    else {
        this->setAttribute(Qt::WA_MacBrushedMetal, false);
    }
}

Settings::~Settings() {
    saveGeometry();
    delete ui;
}
