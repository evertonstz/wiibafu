/***************************************************************************
 *   Copyright (C) 2010-2013 Kai Heitkamp                                  *
 *   dynup@ymail.com | http://dynup.de.vu                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
#include "global.h"

Settings::Settings(QWidget *parent) : QDialog(parent), ui(new Ui::Settings) {
    ui->setupUi(this);
    setupGeometry();
    load();

    #ifdef Q_OS_MACX
        setMacOSXStyle();
        ui->main_label_MacOSXStyle->setEnabled(true);
        ui->main_radioButton_MacOSXStyle_Aqua->setEnabled(true);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setEnabled(true);
    #else
        ui->main_label_MacOSXStyle->setEnabled(false);
        ui->main_radioButton_MacOSXStyle_Aqua->setEnabled(false);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setEnabled(false);
    #endif
}

void Settings::on_main_pushButton_OpenLogFile_clicked() {
    QString path = WIIBAFU_SETTINGS.value("Main/LogFile", QVariant("")).toString();

    if (path.isEmpty()) {
        path = QDir::homePath().append("/WiiBaFu.log");
    }
    else {
        path = ui->main_lineEdit_LogFile->text();
    }

    QString logFile = QFileDialog::getSaveFileName(this, tr("Save log file"), path, tr("WiiBaFu log file (*.log)"));

    if (!logFile.isEmpty()) {
        ui->main_lineEdit_LogFile->setText(logFile);
    }
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

void Settings::on_wit_pushButton_PathToTitlesOpen_clicked() {
    QString path;

    if (!ui->wit_lineEdit_PathToTitles->text().isEmpty()) {
        path = ui->wit_lineEdit_PathToTitles->text();
    }
    else {
        path = QDir::homePath();
    }

    QString titlesPath = QFileDialog::getExistingDirectory(this, tr("Open path to titles"), path, QFileDialog::ShowDirsOnly);

    if (!titlesPath.isEmpty()) {
        ui->wit_lineEdit_PathToTitles->setText(titlesPath);
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
    ui->main_checkBox_useProxy->setChecked(WIIBAFU_SETTINGS.value("Main/UseProxy", QVariant(false)).toBool());
    ui->main_lineEdit_proxyHost->setText(WIIBAFU_SETTINGS.value("Main/ProxyHost", QVariant("")).toString());
    ui->main_lineEdit_proxyPort->setText(WIIBAFU_SETTINGS.value("Main/ProxyPort", QVariant("")).toString());
    ui->main_lineEdit_proxyUser->setText(WIIBAFU_SETTINGS.value("Main/ProxyUser", QVariant("")).toString());
    ui->main_lineEdit_proxyPassword->setText(WIIBAFU_SETTINGS.value("Main/ProxyPassword", QVariant("")).toString());
    ui->main_comboBox_proxyType->setCurrentIndex(WIIBAFU_SETTINGS.value("Main/ProxyType", QVariant(0)).toInt());
    ui->main_comboBox_ApplicationLanguage->setCurrentIndex(WIIBAFU_SETTINGS.value("Main/ApplicationLanguage", QVariant(0)).toInt());
    ui->main_comboBox_Language->setCurrentIndex(WIIBAFU_SETTINGS.value("Main/GameLanguage", QVariant(0)).toInt());
    ui->main_comboBox_Logging->setCurrentIndex(WIIBAFU_SETTINGS.value("Main/Logging", QVariant(0)).toInt());
    ui->main_checkBox_LogWitCommandLine->setChecked(WIIBAFU_SETTINGS.value("Main/LogWitCommandLine", QVariant(true)).toBool());
    ui->main_lineEdit_LogFile->setText(WIIBAFU_SETTINGS.value("Main/LogFile", QVariant("")).toString());

    if (WIIBAFU_SETTINGS.value("Main/MacOSXStyle", QVariant("Aqua")) == "Aqua") {
        ui->main_radioButton_MacOSXStyle_Aqua->setChecked(true);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(false);
    }
    else if (WIIBAFU_SETTINGS.value("Main/MacOSXStyle", QVariant("Aqua")) == "BrushedMetal") {
        ui->main_radioButton_MacOSXStyle_Aqua->setChecked(false);
        ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(true);
    }

    ui->wit_lineEdit_PathToWIT->setText(WIIBAFU_SETTINGS.value("WIT/PathToWIT", QVariant(DEFAULT_WIT_PATH)).toString());
    ui->wit_lineEdit_PathToTitles->setText(WIIBAFU_SETTINGS.value("WIT/PathToTitles", QVariant(DEFAULT_TITLES_PATH)).toString());
    ui->wit_checkBox_Auto->setChecked(WIIBAFU_SETTINGS.value("WIT/Auto", QVariant(true)).toBool());
    ui->wit_lineEdit_WBFSPath->setText(WIIBAFU_SETTINGS.value("WIT/WBFSPath", QVariant("")).toString());
    ui->wit_lineEdit_DVDDrivePath->setText(WIIBAFU_SETTINGS.value("WIT/DVDDrivePath", QVariant("/cdrom")).toString());
    ui->wit_spinBox_RecurseDepth->setValue(WIIBAFU_SETTINGS.value("WIT/RecurseDepth", QVariant(10)).toInt());

    ui->gameLists_checkBox_ShowGrid->setChecked(WIIBAFU_SETTINGS.value("GameListBehavior/ShowGrid", QVariant(false)).toBool());
    ui->gameLists_checkBox_AlternatingRowColors->setChecked(WIIBAFU_SETTINGS.value("GameListBehavior/AlternatingRowColors", QVariant(true)).toBool());
    ui->gameLists_checkBox_ToolTips->setChecked(WIIBAFU_SETTINGS.value("GameListBehavior/ToolTips", QVariant(false)).toBool());
    setScrollMode(WIIBAFU_SETTINGS.value("GameListBehavior/ScrollMode", QVariant(QAbstractItemView::ScrollPerPixel)).toInt());
    setResizeMode(WIIBAFU_SETTINGS.value("GameListBehavior/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());
    ui->gameLists_comboBox_SelectionMode->setCurrentIndex(WIIBAFU_SETTINGS.value("GameListBehavior/SelectionMode", QVariant(3)).toInt());

    ui->filesColumn_checkBox_ID->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnID", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Name->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnName", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Title->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnTitle", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Region->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnRegion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Size->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnSize", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Insertion->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnInsertion", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastModification->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnLastModification", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastStatusChange->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnLastStatusChange", QVariant(true)).toBool());
    ui->filesColumn_checkBox_LastAccess->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnLastAccess", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Type->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnType", QVariant(true)).toBool());
    ui->filesColumn_checkBox_Source->setChecked(WIIBAFU_SETTINGS.value("FilesGameList/columnSource", QVariant(true)).toBool());

    ui->wbfsColumn_checkBox_ID->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnID", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Name->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnName", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Title->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnTitle", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Region->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnRegion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Size->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnSize", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_UsedBlocks->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnUsedBlocks", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Insertion->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnInsertion", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastModification->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnLastModification", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastStatusChange->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnLastStatusChange", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_LastAccess->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnLastAccess", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Type->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnType", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_WBFSSlot->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnWBFSSlot", QVariant(true)).toBool());
    ui->wbfsColumn_checkBox_Source->setChecked(WIIBAFU_SETTINGS.value("WBFSGameList/columnSource", QVariant(true)).toBool());

    ui->transferToWBFS_checkBox_Force->setChecked(WIIBAFU_SETTINGS.value("TransferToWBFS/Force", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Test->setChecked(WIIBAFU_SETTINGS.value("TransferToWBFS/Test", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Newer->setChecked(WIIBAFU_SETTINGS.value("TransferToWBFS/Newer", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Update->setChecked(WIIBAFU_SETTINGS.value("TransferToWBFS/Update", QVariant(false)).toBool());
    ui->transferToWBFS_checkBox_Overwrite->setChecked(WIIBAFU_SETTINGS.value("TransferToWBFS/Overwrite", QVariant(false)).toBool());

    ui->transferToImageFST_checkBox_Test->setChecked(WIIBAFU_SETTINGS.value("TransferToImageFST/Test", QVariant(false)).toBool());
    ui->transferToImageFST_checkBox_Update->setChecked(WIIBAFU_SETTINGS.value("TransferToImageFST/Update", QVariant(false)).toBool());
    ui->transferToImageFST_checkBox_Overwrite->setChecked(WIIBAFU_SETTINGS.value("TransferToImageFST/Overwrite", QVariant(false)).toBool());
    ui->transferToImageFST_checkBox_Diff->setChecked(WIIBAFU_SETTINGS.value("TransferToImageFST/Diff", QVariant(false)).toBool());

    ui->transferFromWBFS_checkBox_Force->setChecked(WIIBAFU_SETTINGS.value("TransferFromWBFS/Force", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Test->setChecked(WIIBAFU_SETTINGS.value("TransferFromWBFS/Test", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Update->setChecked(WIIBAFU_SETTINGS.value("TransferFromWBFS/Update", QVariant(false)).toBool());
    ui->transferFromWBFS_checkBox_Overwrite->setChecked(WIIBAFU_SETTINGS.value("TransferFromWBFS/Overwrite", QVariant(false)).toBool());

    ui->transfer_checkBox_ScrubbingRaw->setChecked(WIIBAFU_SETTINGS.value("Scrubbing/Raw", QVariant(false)).toBool());
    ui->transfer_checkBox_ScrubbingWhole->setChecked(WIIBAFU_SETTINGS.value("Scrubbing/Whole", QVariant(false)).toBool());
    ui->transfer_checkBox_ScrubbingData->setChecked(WIIBAFU_SETTINGS.value("Scrubbing/Data", QVariant(false)).toBool());
    ui->transfer_checkBox_ScrubbingUpdate->setChecked(WIIBAFU_SETTINGS.value("Scrubbing/Update", QVariant(false)).toBool());
    ui->transfer_checkBox_ScrubbingChannel->setChecked(WIIBAFU_SETTINGS.value("Scrubbing/Channel", QVariant(false)).toBool());

    ui->checkWBFS_checkBox_Repair->setChecked(WIIBAFU_SETTINGS.value("CheckWBFS/Repair", QVariant(true)).toBool());
    ui->checkWBFS_checkBox_Test->setChecked(WIIBAFU_SETTINGS.value("CheckWBFS/Test", QVariant(false)).toBool());

    ui->repairWBFS_checkBox_FBT->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/FBT", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_INODES->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/INODES", QVariant(true)).toBool());
    ui->repairWBFS_checkBox_RMEMPTY->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/RM-EMTPY", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMFREE->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/RM-FREE", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMINVALID->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/RM-INVALID", QVariant(false)).toBool());
    ui->repairWBFS_checkBox_RMOVERLAP->setChecked(WIIBAFU_SETTINGS.value("RepairWBFS/RM-OVERLAP", QVariant(false)).toBool());

    ui->removeFromWBFS_checkBox_Force->setChecked(WIIBAFU_SETTINGS.value("RemoveFromWBFS/Force", QVariant(false)).toBool());
    ui->removeFromWBFS_checkBox_Test->setChecked(WIIBAFU_SETTINGS.value("RemoveFromWBFS/Test", QVariant(false)).toBool());
}

void Settings::save() {
    WIIBAFU_SETTINGS.setValue("Main/UseProxy", ui->main_checkBox_useProxy->checkState());
    WIIBAFU_SETTINGS.setValue("Main/ProxyHost", ui->main_lineEdit_proxyHost->text());
    WIIBAFU_SETTINGS.setValue("Main/ProxyPort", ui->main_lineEdit_proxyPort->text());
    WIIBAFU_SETTINGS.setValue("Main/ProxyUser", ui->main_lineEdit_proxyUser->text());
    WIIBAFU_SETTINGS.setValue("Main/ProxyPassword", ui->main_lineEdit_proxyPassword->text());
    WIIBAFU_SETTINGS.setValue("Main/ProxyType", ui->main_comboBox_proxyType->currentIndex());
    WIIBAFU_SETTINGS.setValue("Main/ApplicationLanguage", ui->main_comboBox_ApplicationLanguage->currentIndex());
    WIIBAFU_SETTINGS.setValue("Main/GameLanguage", ui->main_comboBox_Language->currentIndex());
    WIIBAFU_SETTINGS.setValue("Main/Logging", ui->main_comboBox_Logging->currentIndex());
    WIIBAFU_SETTINGS.setValue("Main/LogWitCommandLine", ui->main_checkBox_LogWitCommandLine->checkState());
    WIIBAFU_SETTINGS.setValue("Main/LogFile", ui->main_lineEdit_LogFile->text());
    WIIBAFU_SETTINGS.setValue("Main/MacOSXStyle", macOSXStyle());

    WIIBAFU_SETTINGS.setValue("WIT/PathToWIT", ui->wit_lineEdit_PathToWIT->text());
    WIIBAFU_SETTINGS.setValue("WIT/PathToTitles", ui->wit_lineEdit_PathToTitles->text());
    WIIBAFU_SETTINGS.setValue("WIT/Auto", ui->wit_checkBox_Auto->checkState());
    WIIBAFU_SETTINGS.setValue("WIT/WBFSPath", ui->wit_lineEdit_WBFSPath->text());
    WIIBAFU_SETTINGS.setValue("WIT/DVDDrivePath", ui->wit_lineEdit_DVDDrivePath->text());
    WIIBAFU_SETTINGS.setValue("WIT/RecurseDepth", ui->wit_spinBox_RecurseDepth->value());

    WIIBAFU_SETTINGS.setValue("GameListBehavior/ShowGrid", ui->gameLists_checkBox_ShowGrid->checkState());
    WIIBAFU_SETTINGS.setValue("GameListBehavior/AlternatingRowColors", ui->gameLists_checkBox_AlternatingRowColors->checkState());
    WIIBAFU_SETTINGS.setValue("GameListBehavior/ToolTips", ui->gameLists_checkBox_ToolTips->checkState());
    WIIBAFU_SETTINGS.setValue("GameListBehavior/ScrollMode", scrollMode());
    WIIBAFU_SETTINGS.setValue("GameListBehavior/ResizeMode", resizeMode());
    WIIBAFU_SETTINGS.setValue("GameListBehavior/SelectionMode", ui->gameLists_comboBox_SelectionMode->currentIndex());

    WIIBAFU_SETTINGS.setValue("FilesGameList/columnID", ui->filesColumn_checkBox_ID->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnName", ui->filesColumn_checkBox_Name->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnTitle", ui->filesColumn_checkBox_Title->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnRegion", ui->filesColumn_checkBox_Region->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnSize", ui->filesColumn_checkBox_Size->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnInsertion", ui->filesColumn_checkBox_Insertion->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnLastModification", ui->filesColumn_checkBox_LastModification->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnLastStatusChange", ui->filesColumn_checkBox_LastStatusChange->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnLastAccess", ui->filesColumn_checkBox_LastAccess->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnType", ui->filesColumn_checkBox_Type->checkState());
    WIIBAFU_SETTINGS.setValue("FilesGameList/columnSource", ui->filesColumn_checkBox_Source->checkState());

    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnID", ui->wbfsColumn_checkBox_ID->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnName", ui->wbfsColumn_checkBox_Name->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnTitle", ui->wbfsColumn_checkBox_Title->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnRegion", ui->wbfsColumn_checkBox_Region->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnSize", ui->wbfsColumn_checkBox_Size->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnUsedBlocks", ui->wbfsColumn_checkBox_UsedBlocks->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnInsertion", ui->wbfsColumn_checkBox_Insertion->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnLastModification", ui->wbfsColumn_checkBox_LastModification->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnLastStatusChange", ui->wbfsColumn_checkBox_LastStatusChange->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnLastAccess", ui->wbfsColumn_checkBox_LastAccess->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnType", ui->wbfsColumn_checkBox_Type->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnWBFSSlot", ui->wbfsColumn_checkBox_WBFSSlot->checkState());
    WIIBAFU_SETTINGS.setValue("WBFSGameList/columnSource", ui->wbfsColumn_checkBox_Source->checkState());

    WIIBAFU_SETTINGS.setValue("TransferToWBFS/Force", ui->transferToWBFS_checkBox_Force->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToWBFS/Test", ui->transferToWBFS_checkBox_Test->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToWBFS/Newer", ui->transferToWBFS_checkBox_Newer->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToWBFS/Update", ui->transferToWBFS_checkBox_Update->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToWBFS/Overwrite", ui->transferToWBFS_checkBox_Overwrite->checkState());

    WIIBAFU_SETTINGS.setValue("TransferToImageFST/Test", ui->transferToImageFST_checkBox_Test->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToImageFST/Update", ui->transferToImageFST_checkBox_Update->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToImageFST/Overwrite", ui->transferToImageFST_checkBox_Overwrite->checkState());
    WIIBAFU_SETTINGS.setValue("TransferToImageFST/Diff", ui->transferToImageFST_checkBox_Diff->checkState());

    WIIBAFU_SETTINGS.setValue("TransferFromWBFS/Force", ui->transferFromWBFS_checkBox_Force->checkState());
    WIIBAFU_SETTINGS.setValue("TransferFromWBFS/Test", ui->transferFromWBFS_checkBox_Test->checkState());
    WIIBAFU_SETTINGS.setValue("TransferFromWBFS/Update", ui->transferFromWBFS_checkBox_Update->checkState());
    WIIBAFU_SETTINGS.setValue("TransferFromWBFS/Overwrite", ui->transferFromWBFS_checkBox_Overwrite->checkState());

    WIIBAFU_SETTINGS.setValue("Scrubbing/Raw", ui->transfer_checkBox_ScrubbingRaw->checkState());
    WIIBAFU_SETTINGS.setValue("Scrubbing/Whole", ui->transfer_checkBox_ScrubbingWhole->checkState());
    WIIBAFU_SETTINGS.setValue("Scrubbing/Data", ui->transfer_checkBox_ScrubbingData->checkState());
    WIIBAFU_SETTINGS.setValue("Scrubbing/Update", ui->transfer_checkBox_ScrubbingUpdate->checkState());
    WIIBAFU_SETTINGS.setValue("Scrubbing/Channel", ui->transfer_checkBox_ScrubbingChannel->checkState());

    WIIBAFU_SETTINGS.setValue("CheckWBFS/Repair", ui->checkWBFS_checkBox_Repair->checkState());
    WIIBAFU_SETTINGS.setValue("CheckWBFS/Test", ui->checkWBFS_checkBox_Test->checkState());

    WIIBAFU_SETTINGS.setValue("RepairWBFS/FBT", ui->repairWBFS_checkBox_FBT->checkState());
    WIIBAFU_SETTINGS.setValue("RepairWBFS/INODES", ui->repairWBFS_checkBox_INODES->checkState());
    WIIBAFU_SETTINGS.setValue("RepairWBFS/RM-EMTPY", ui->repairWBFS_checkBox_RMEMPTY->checkState());
    WIIBAFU_SETTINGS.setValue("RepairWBFS/RM-FREE", ui->repairWBFS_checkBox_RMFREE->checkState());
    WIIBAFU_SETTINGS.setValue("RepairWBFS/RM-INVALID", ui->repairWBFS_checkBox_RMINVALID->checkState());
    WIIBAFU_SETTINGS.setValue("RepairWBFS/RM-OVERLAP", ui->repairWBFS_checkBox_RMOVERLAP->checkState());

    WIIBAFU_SETTINGS.setValue("RemoveFromWBFS/Force", ui->removeFromWBFS_checkBox_Force->checkState());
    WIIBAFU_SETTINGS.setValue("RemoveFromWBFS/Test", ui->removeFromWBFS_checkBox_Test->checkState());
}

void Settings::restoreDefaults(const int index) {
    switch (index) {
        case 0: // Main
                ui->main_checkBox_useProxy->setChecked(false);
                ui->main_lineEdit_proxyHost->setEnabled(false);
                ui->main_lineEdit_proxyPort->setEnabled(false);
                ui->main_lineEdit_proxyUser->setEnabled(false);
                ui->main_lineEdit_proxyPassword->setEnabled(false);
                ui->main_comboBox_proxyType->setCurrentIndex(3);
                ui->main_comboBox_ApplicationLanguage->setCurrentIndex(0);
                ui->main_comboBox_Language->setCurrentIndex(0);
                ui->main_comboBox_Logging->setCurrentIndex(0);
                ui->main_checkBox_LogWitCommandLine->setChecked(true);
                ui->main_lineEdit_LogFile->setText("");
                ui->main_radioButton_MacOSXStyle_Aqua->setChecked(true);
                ui->main_radioButton_MacOSXStyle_BrushedMetal->setChecked(false);
                break;
        case 1: // WIT
                ui->wit_lineEdit_PathToWIT->setText(DEFAULT_WIT_PATH);
                ui->wit_lineEdit_PathToTitles->setText(DEFAULT_TITLES_PATH);
                ui->wit_checkBox_Auto->setChecked(true);
                ui->wit_lineEdit_DVDDrivePath->setText("/cdrom");
                ui->wit_spinBox_RecurseDepth->setValue(10);
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
                ui->transferToImageFST_checkBox_Diff->setChecked(false);
                ui->transferFromWBFS_checkBox_Force->setChecked(false);
                ui->transferFromWBFS_checkBox_Test->setChecked(false);
                ui->transferFromWBFS_checkBox_Update->setChecked(false);
                ui->transferFromWBFS_checkBox_Overwrite->setChecked(false);
                ui->transfer_checkBox_ScrubbingRaw->setChecked(false);
                ui->transfer_checkBox_ScrubbingWhole->setChecked(false);
                ui->transfer_checkBox_ScrubbingData->setChecked(true);
                ui->transfer_checkBox_ScrubbingUpdate->setChecked(true);
                ui->transfer_checkBox_ScrubbingChannel->setChecked(true);
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

void Settings::setScrollMode(const int mode) {
    switch (mode) {
        case QAbstractItemView::ScrollPerPixel:
            ui->gameLists_radioButton_ScrollPerPixel->setChecked(true);
            break;
    case QAbstractItemView::ScrollPerItem:
        ui->gameLists_radioButton_ScrollPerItem->setChecked(true);
        break;
    }
}

void Settings::setResizeMode(const int mode) {
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

void Settings::on_transfer_checkBox_ScrubbingData_stateChanged(int state) {
    if (state) {
        ui->transfer_checkBox_ScrubbingRaw->setChecked(false);
    }
}

void Settings::on_transfer_checkBox_ScrubbingUpdate_stateChanged(int state) {
    if (state) {
        ui->transfer_checkBox_ScrubbingRaw->setChecked(false);
    }
}

void Settings::on_transfer_checkBox_ScrubbingChannel_stateChanged(int state) {
    if (state) {
        ui->transfer_checkBox_ScrubbingRaw->setChecked(false);
    }
}

void Settings::on_transfer_checkBox_ScrubbingRaw_stateChanged(int state) {
    if (state) {
        ui->transfer_checkBox_ScrubbingData->setChecked(false);
        ui->transfer_checkBox_ScrubbingUpdate->setChecked(false);
        ui->transfer_checkBox_ScrubbingChannel->setChecked(false);
    }
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
    if (WIIBAFU_SETTINGS.contains("Settings/x")) {
        QRect rect;
        rect.setX(WIIBAFU_SETTINGS.value("Settings/x", QVariant(0)).toInt());
        rect.setY(WIIBAFU_SETTINGS.value("Settings/y", QVariant(0)).toInt());
        rect.setWidth(WIIBAFU_SETTINGS.value("Settings/width", QVariant(800)).toInt());
        rect.setHeight(WIIBAFU_SETTINGS.value("Settings/height", QVariant(600)).toInt());

        this->setGeometry(rect);
    }
}

void Settings::saveGeometry() {
    WIIBAFU_SETTINGS.setValue("Settings/x", this->geometry().x());
    WIIBAFU_SETTINGS.setValue("Settings/y", this->geometry().y());
    WIIBAFU_SETTINGS.setValue("Settings/width", this->geometry().width());
    WIIBAFU_SETTINGS.setValue("Settings/height", this->geometry().height());
}

void Settings::setMacOSXStyle() {
    if (WIIBAFU_SETTINGS.value("Main/MacOSXStyle", QVariant("Aqua")).toString().contains("BrushedMetal")) {
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
