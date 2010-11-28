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

#include "wiibafu.h"
#include "ui_wiibafu.h"

WiiBaFu::WiiBaFu(QWidget *parent) : QMainWindow(parent), ui(new Ui::WiiBaFu) {
    wiTools = new WiTools(this);
    common = new Common(this);
    settings = new Settings(this);
    wiibafudialog = new WiiBaFuDialog(this);
    coverViewDialog = new CoverViewDialog(this);
    wbfsDialog = new WBFSDialog(this);

    timer = new QTimer(this);

    filesListModel = new QStandardItemModel(this);
    dvdListModel = new QStandardItemModel(this);
    wbfsListModel = new QStandardItemModel(this);

    ui->setupUi(this);
    setWindowTitle("Wii Backup Fusion " + QCoreApplication::applicationVersion());
    setupMainProgressBar();
    setupConnections();
    setupGeometry();

    setGameListAttributes(ui->filesTab_tableView);
    setGameListAttributes(ui->dvdTab_tableView);
    setGameListAttributes(ui->wbfsTab_tableView);

    setStatusBarText(tr("Ready."));

    addEntryToLog(tr("(%1) Wii Backup Fusion %2 started.").arg(QDateTime::currentDateTime().toString(), QCoreApplication::applicationVersion()), WiTools::Info);
    addEntryToLog(wiTools->witVersion(), WiTools::Info);
    addEntryToLog(wiTools->wwtVersion(), WiTools::Info);

    QString witPath = wiTools->witTitlesPath();
    if (witPath.contains("witTitles:")) {
        addEntryToLog(tr("Titles not found!"), WiTools::Info);
    }
    else {
        addEntryToLog(tr("Titles found in: %1").arg(witPath), WiTools::Info);
    }
}

void WiiBaFu::setupConnections() {
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<WiTools::LogType>("WiTools::LogType");

    connect(this, SIGNAL(cancelTransfer()), wiTools, SLOT(transfer_cancel()));

    connect(this, SIGNAL(startBusy()), this, SLOT(startMainProgressBarBusy()));
    connect(this, SIGNAL(stopBusy()), this, SLOT(stopMainProgressBarBusy()));
    connect(timer, SIGNAL(timeout()), this, SLOT(showMainProgressBarBusy()));

    connect(wiTools, SIGNAL(setMainProgressBar(int, QString)), this, SLOT(setMainProgressBar(int,QString)));
    connect(wiTools, SIGNAL(setMainProgressBarVisible(bool)), this, SLOT(setMainProgressBarVisible(bool)));

    connect(wiTools, SIGNAL(setInfoTextWBFS(QString)), this, SLOT(setWBFSInfoText(QString)));
    connect(wiTools, SIGNAL(setProgressBarWBFS(int, int, int, QString)), this, SLOT(setWBFSProgressBar(int, int, int, QString)));
    connect(wiTools, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(wiTools, SIGNAL(newLogEntry(QString, WiTools::LogType)), this, SLOT(addEntryToLog(QString, WiTools::LogType)));

    connect(wiTools, SIGNAL(newFilesGameListModel()), this, SLOT(setFilesGameListModel()));
    connect(wiTools, SIGNAL(newDVDGameListModel()), this, SLOT(setDVDGameListModel()));
    connect(wiTools, SIGNAL(newWBFSGameListModel()), this, SLOT(setWBFSGameListModel()));

    connect(wiTools, SIGNAL(transferGamesToWBFSsuccessfully()), this, SLOT(transferGamesToWBFSsuccesfully()));
    connect(wiTools, SIGNAL(transferGamesToWBFScanceled(bool)), this, SLOT(transferGamesToWBFScanceled(bool)));

    connect(wiTools, SIGNAL(transferGameFromDVDToWBFSsuccessfully()), this, SLOT(transferGameFromDVDToWBFSsuccesfully()));
    connect(wiTools, SIGNAL(transferGameFromDVDToWBFScanceled(bool)), this, SLOT(transferGameFromDVDToWBFScanceled(bool)));
    connect(wiTools, SIGNAL(transferGameFromDVDToImageSuccessfully()), this, SLOT(transferGameFromDVDToImageSuccesfully()));
    connect(wiTools, SIGNAL(transferGameFromDVDToImageCanceled(bool)), this, SLOT(transferGameFromDVDToImageCanceled(bool)));

    connect(wiTools, SIGNAL(transferGamesToImageSuccessfully()), this, SLOT(transferGamesToImageSuccesfully()));
    connect(wiTools, SIGNAL(transferGamesToImageCanceled()), this, SLOT(transferGamesToImageCanceled()));
    connect(wiTools, SIGNAL(removeGamesFromWBFS_successfully()), this, SLOT(on_wbfsTab_pushButton_Load_clicked()));

    connect(wiTools, SIGNAL(stopBusy()), this, SLOT(stopMainProgressBarBusy()));

    connect(common, SIGNAL(newGame3DCover(QImage*)), this, SLOT(showGame3DCover(QImage*)));
    connect(common, SIGNAL(newGameFullHQCover(QImage*)), this, SLOT(showGameFullHQCover(QImage*)));
    connect(common, SIGNAL(newGameDiscCover(QImage*)), this, SLOT(showGameDiscCover(QImage*)));
    connect(common, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(common, SIGNAL(newLogEntry(QString, WiTools::LogType)), this, SLOT(addEntryToLog(QString, WiTools::LogType)));
    connect(common, SIGNAL(setMainProgressBarVisible(bool)), this, SLOT(setMainProgressBarVisible(bool)));
    connect(common, SIGNAL(setMainProgressBar(int,QString)), this, SLOT(setMainProgressBar(int,QString)));

    connect(ui->menuHelp_About_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->menuFile_Exit, SIGNAL(triggered()), this, SLOT(close()));
}

void WiiBaFu::setupMainProgressBar() {
    progressBar_Main = new QProgressBar(this);
    progressBar_Main->setObjectName(QString::fromUtf8("progressBarMain"));
    progressBar_Main->setVisible(false);
    progressBar_Main->setMinimum(0);
    progressBar_Main->setMaximum(100);
    progressBar_Main->setValue(0);

    progressBar_Main->setMaximumHeight(16);
    ui->statusBar->addPermanentWidget(progressBar_Main);

    #ifdef Q_OS_MACX
        ui->wbfsTab_label_Info->setVisible(true);
    #else
        ui->wbfsTab_label_Info->setVisible(false);
    #endif
}

void WiiBaFu::setupGeometry() {
    if (WiiBaFuSettings.contains("MainWindow/x")) {
        QRect rect;
        rect.setX(WiiBaFuSettings.value("MainWindow/x", QVariant(0)).toInt());
        rect.setY(WiiBaFuSettings.value("MainWindow/y", QVariant(0)).toInt());
        rect.setWidth(WiiBaFuSettings.value("MainWindow/width", QVariant(800)).toInt());
        rect.setHeight(WiiBaFuSettings.value("MainWindow/height", QVariant(600)).toInt());

        this->setGeometry(rect);
    }
}

void WiiBaFu::setMainProgressBarVisible(bool visible) {
    progressBar_Main->setVisible(visible);
}

void WiiBaFu::setGameListAttributes(QTableView *gameTableView) {
    gameTableView->setShowGrid(WiiBaFuSettings.value("GameLists/ShowGrid", QVariant(false)).toBool());
    gameTableView->setAlternatingRowColors(WiiBaFuSettings.value("GameLists/AlternatingRowColors", QVariant(true)).toBool());
    gameTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    gameTableView->setHorizontalScrollMode((QHeaderView::ScrollMode)WiiBaFuSettings.value("GameLists/ScrollMode", QVariant(QHeaderView::ScrollPerPixel)).toInt());
    gameTableView->setVerticalScrollMode((QHeaderView::ScrollMode)WiiBaFuSettings.value("GameLists/ScrollMode", QVariant(QHeaderView::ScrollPerPixel)).toInt());

    if (gameTableView != ui->dvdTab_tableView) {
        gameTableView->verticalHeader()->hide();
        gameTableView->horizontalHeader()->setMovable(true);
        gameTableView->verticalHeader()->setDefaultSectionSize(20);
        gameTableView->horizontalHeader()->setResizeMode((QHeaderView::ResizeMode)WiiBaFuSettings.value("GameLists/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());
        gameTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        gameTableView->setSortingEnabled(true);
        gameTableView->setTabKeyNavigation(false);
    }
    else {
        gameTableView->horizontalHeader()->hide();
        gameTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        gameTableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
        gameTableView->setSelectionMode(QAbstractItemView::NoSelection);
    }
}

void WiiBaFu::on_menuOptions_Settings_triggered() {
    int language = WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt();

    if (settings->exec() == QDialog::Accepted) {
        setGameListAttributes(ui->filesTab_tableView);
        setGameListAttributes(ui->dvdTab_tableView);
        setGameListAttributes(ui->wbfsTab_tableView);

        setFilesColumns();
        setWBFSColumns();

        if (WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt() != language) {
            updateTitles();
        }

        if (WiiBaFuSettings.value("GameLists/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->filesTab_tableView, filesListModel, tr("Title"), tr("Name"));
            setToolTips(ui->wbfsTab_tableView, wbfsListModel, tr("Title"), tr("Name"));
            ui->filesTab_tableView->update();
            ui->wbfsTab_tableView->update();
        }

        ui->infoTab_comboBox_CoverLanguages->setCurrentIndex(WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt());
    }
}

void WiiBaFu::on_menuTools_CheckWBFS_triggered() {
    on_wbfsTab_pushButton_Check_clicked();
}

void WiiBaFu::on_menuTools_CreateWBFS_triggered() {
    if (wbfsDialog->exec() == QDialog::Accepted) {
        emit startBusy();

        WiTools::CreateWBFSParameters parameters;
        parameters.Path = wbfsDialog->path();
        parameters.Size = wbfsDialog->size();
        parameters.SplitSize = wbfsDialog->splitSize();
        parameters.HDSectorSize = wbfsDialog->hdSectorSize();
        parameters.WBFSSectorSize = wbfsDialog->wbfsSectorSize();
        parameters.Recover = wbfsDialog->recover();
        parameters.Inode = wbfsDialog->inode();
        parameters.Test = wbfsDialog->test();

        QtConcurrent::run(wiTools, &WiTools::createWBFS, parameters);
    }
}

void WiiBaFu::on_menuTools_UpdateTitles_triggered() {
    QtConcurrent::run(common, &Common::updateTitles);
}

void WiiBaFu::on_filesTab_tableView_doubleClicked(QModelIndex) {
    setGameInfo(ui->filesTab_tableView, filesListModel);
}

void WiiBaFu::on_wbfsTab_tableView_doubleClicked(QModelIndex) {
    setGameInfo(ui->wbfsTab_tableView, wbfsListModel);
}

void WiiBaFu::on_filesTab_pushButton_Load_clicked() {
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open directory"), WiiBaFuSettings.value("Main/LastFilesPath", QVariant(QDir::homePath()).toString()).toString(), QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        emit startBusy();

        WiiBaFuSettings.setValue("Main/LastFilesPath", directory);
        QtConcurrent::run(wiTools, &WiTools::requestFilesGameListModel, filesListModel, directory);
    }
}

void WiiBaFu::on_filesTab_pushButton_SelectAll_clicked() {
    if (ui->filesTab_tableView->model()) {
        if (ui->filesTab_tableView->selectionModel()->selectedRows(0).count() != ui->filesTab_tableView->model()->rowCount()) {
            ui->filesTab_tableView->selectAll();
        }
        else {
            ui->filesTab_tableView->selectionModel()->clearSelection();
        }
    }
}

void WiiBaFu::on_filesTab_pushButton_TransferToWBFS_clicked() {
    if (!ui->filesTab_pushButton_TransferToWBFS->text().contains(tr("Cancel transfering"))) {
        if (ui->filesTab_tableView->selectionModel() && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            ui->filesTab_pushButton_TransferToWBFS->setText(tr("Cancel transfering"));

            QtConcurrent::run(wiTools, &WiTools::transferGamesToWBFS, ui->filesTab_tableView->selectionModel()->selectedRows(10), wbfsPath());
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_filesTab_pushButton_TransferToImage_clicked() {
    if (!ui->filesTab_pushButton_TransferToImage->text().contains(tr("Cancel transfering"))) {
        if (ui->filesTab_tableView->model() && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted) {
                QDir path = wiibafudialog->imageDirectory();
                QString format = wiibafudialog->imageFormat();
                QString compression = wiibafudialog->compression();

                if (!path.exists()) {
                    QMessageBox::warning(this, tr("Warning"), tr("The directory doesn't exists!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
                else {
                    ui->filesTab_pushButton_TransferToImage->setText(tr("Cancel transfering"));
                    QtConcurrent::run(wiTools, &WiTools::convertGameImages, ui->filesTab_tableView->selectionModel()->selectedRows(10), format, compression, path.absolutePath());
                }
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_filesTab_pushButton_ShowInfo_clicked() {
    setGameInfo(ui->filesTab_tableView, filesListModel);
}

void WiiBaFu::on_dvdTab_pushButton_Load_clicked() {
    emit startBusy();

    QString dvdPath = WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/cdrom")).toString();
    QtConcurrent::run(wiTools, &WiTools::requestDVDGameListModel, dvdListModel, dvdPath);
}

void WiiBaFu::on_dvdTab_pushButton_TransferToWBFS_clicked() {
    if (!ui->dvdTab_pushButton_TransferToWBFS->text().contains(tr("Cancel transfering"))) {
        if (dvdListModel->rowCount() != 0) {
            ui->dvdTab_pushButton_TransferToWBFS->setText(tr("Cancel transfering"));

            QtConcurrent::run(wiTools, &WiTools::transferGameFromDVDToWBFS, dvdListModel->index(15, 0).data().toString(), wbfsPath());
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_dvdTab_pushButton_TransferToImage_clicked() {
    if (!ui->dvdTab_pushButton_TransferToImage->text().contains(tr("Cancel transfering"))) {
        if (dvdListModel->rowCount() != 0) {
            wiibafudialog->setOpenFile();

            if (wiibafudialog->exec() == QDialog::Accepted) {
                QString filePath = wiibafudialog->imageFilePath();
                QString format = wiibafudialog->imageFormat();
                QString compression = wiibafudialog->compression();

                ui->dvdTab_pushButton_TransferToImage->setText(tr("Cancel transfering"));
                QString dvdPath = WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/dev/sr0")).toString();

                QtConcurrent::run(wiTools, &WiTools::transferGameFromDVDToImage, dvdPath, format, compression, filePath);
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_wbfsTab_pushButton_Load_clicked() {
    emit startBusy();

    QtConcurrent::run(wiTools, &WiTools::requestWBFSGameListModel, wbfsListModel, wbfsPath());
}

void WiiBaFu::on_wbfsTab_pushButton_SelectAll_clicked() {
    if (ui->wbfsTab_tableView->model()) {
        if (ui->wbfsTab_tableView->selectionModel()->selectedRows(0).count() != ui->wbfsTab_tableView->model()->rowCount()) {
            ui->wbfsTab_tableView->selectAll();
        }
        else {
            ui->wbfsTab_tableView->selectionModel()->clearSelection();
        }
    }
}

void WiiBaFu::on_wbfsTab_pushButton_Transfer_clicked() {
    if (!ui->wbfsTab_pushButton_Transfer->text().contains(tr("Cancel transfering"))) {
        if (ui->wbfsTab_tableView->model() && !ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted) {
                QDir path = wiibafudialog->imageDirectory();
                QString format = wiibafudialog->imageFormat();

                if (!path.exists()) {
                    QMessageBox::warning(this, tr("Warning"), tr("The directory doesn't exists!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
                else if (format.contains("wia")) {
                    QMessageBox::critical(this, tr("Error"), tr("Transfer WBFS to image doesn't support 'Wii ISO Archive' format!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
                else {
                    ui->wbfsTab_pushButton_Transfer->setText(tr("Cancel transfering"));
                    QtConcurrent::run(wiTools, &WiTools::transferGamesToImage, ui->wbfsTab_tableView->selectionModel()->selectedRows(0), wbfsPath(), format, path.absolutePath());
                }
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_wbfsTab_pushButton_Remove_clicked() {
    if (ui->wbfsTab_tableView->model() && !ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
        int result = QMessageBox::question(this, tr("Remove games"), tr("Are you sure that you want to delete the selected games?"), QMessageBox::Ok, QMessageBox::Cancel);
        if (result == QMessageBox::Ok) {
            QtConcurrent::run(wiTools, &WiTools::removeGamesFromWBFS, ui->wbfsTab_tableView->selectionModel()->selectedRows(0), wbfsPath());
        }
    }
}

void WiiBaFu::on_wbfsTab_pushButton_ShowInfo_clicked() {
    setGameInfo(ui->wbfsTab_tableView, wbfsListModel);
}

void WiiBaFu::on_wbfsTab_pushButton_Check_clicked() {
    int result = QMessageBox::question(this, tr("Check/Repair WBFS"), tr("Are you sure that you want to check/repair the wbfs?"), QMessageBox::Ok, QMessageBox::Cancel);
    if (result == QMessageBox::Ok) {
        QtConcurrent::run(wiTools, &WiTools::checkWBFS, wbfsPath());
    }
}

void WiiBaFu::on_infoTab_pushButton_Load3DCover_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty())
        ui->infoTab_label_GameCover->clear();
        common->requestGameCover(ui->infoTab_lineEdit_ID->text(), currentCoverLanguage(), Common::ThreeD);
}

void WiiBaFu::on_infoTab_pushButton_LoadFullHQCover_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty())
        common->requestGameCover(ui->infoTab_lineEdit_ID->text(), currentCoverLanguage(), Common::HighQuality);
}

void WiiBaFu::on_infoTab_pushButton_Reset_clicked() {
    ui->infoTab_lineEdit_ID->clear();
    ui->infoTab_lineEdit_Name->clear();
    ui->infoTab_lineEdit_Title->clear();
    ui->infoTab_lineEdit_Region->clear();
    ui->infoTab_lineEdit_Size->clear();
    ui->infoTab_lineEdit_UsedBlocks->clear();
    ui->infoTab_lineEdit_Insertion->clear();
    ui->infoTab_lineEdit_LastModification->clear();
    ui->infoTab_lineEdit_LastStatusChange->clear();
    ui->infoTab_lineEdit_LastAccess->clear();
    ui->infoTab_lineEdit_Type->clear();
    ui->infoTab_lineEdit_WBFSSlot->clear();
    ui->infoTab_lineEdit_Source->clear();

    ui->infoTab_label_GameCover->clear();
}

void WiiBaFu::on_logTab_pushButton_Clear_clicked() {
    ui->logTab_plainTextEdit_Log->clear();
}

void WiiBaFu::on_logTab_pushButton_Copy_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->logTab_plainTextEdit_Log->toPlainText());
}

void WiiBaFu::on_logTab_pushButton_Save_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save log file"), QDir::homePath().append("/WiiBaFu.log"), tr("WiiBaFu log file (*.log)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream textStream(&file);
            textStream << ui->logTab_plainTextEdit_Log->toPlainText();
        }

        file.close();
    }
}

void WiiBaFu::setFilesGameListModel() {
    if (filesListModel->rowCount() > 0 ) {
        ui->filesTab_tableView->setModel(filesListModel);
        ui->tabWidget->setTabText(0, QString("Files (%1)").arg(ui->filesTab_tableView->model()->rowCount()));

        ui->filesTab_tableView->horizontalHeader()->restoreState(WiiBaFuSettings.value("GameLists/Files_HeaderStates").toByteArray());

        setFilesColumns();
        if (WiiBaFuSettings.value("GameLists/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->filesTab_tableView, filesListModel, tr("Title"), tr("Name"));
        }

        emit stopBusy();
        setStatusBarText(tr("Ready."));
    }
}

void WiiBaFu::setDVDGameListModel() {
    if (dvdListModel->rowCount() > 0) {
        QString dvdPath = WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/cdrom")).toString();

        ui->dvdTab_tableView->setModel(dvdListModel);
        ui->tabWidget->setTabText(1, QString("DVD (%1)").arg(dvdPath));

        emit stopBusy();
        common->requestGameCover(dvdListModel->item(0, 0)->text(), QString("EN"), Common::Disc);
    }
}

void WiiBaFu::setWBFSGameListModel() {
    if (wbfsListModel->rowCount() > 0) {
        ui->wbfsTab_tableView->setModel(wbfsListModel);
        ui->tabWidget->setTabText(2, QString("WBFS (%1)").arg(ui->wbfsTab_tableView->model()->rowCount()));

        ui->wbfsTab_tableView->horizontalHeader()->restoreState(WiiBaFuSettings.value("GameLists/WBFS_HeaderStates").toByteArray());

        setWBFSColumns();
        if (WiiBaFuSettings.value("GameLists/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->wbfsTab_tableView, wbfsListModel, tr("Title"), tr("Name"));
        }

        emit stopBusy();
        setStatusBarText(tr("Ready."));
    }
}

void WiiBaFu::updateTitles() {
    if (filesListModel->rowCount() > 0) {
        for (int i = 0; i < filesListModel->rowCount(); i++) {
            filesListModel->item(i, 2)->setText(common->titleFromDB(filesListModel->item(i, 0)->text()));
        }

        ui->filesTab_tableView->update();
    }

    if (wbfsListModel->rowCount() > 0) {
        for (int i = 0; i < wbfsListModel->rowCount(); i++) {
            wbfsListModel->item(i, 2)->setText(common->titleFromDB(wbfsListModel->item(i, 0)->text()));
        }

        ui->wbfsTab_tableView->update();
    }
}

void WiiBaFu::setToolTips(QTableView *tableView, QStandardItemModel *model, QString firstColumnName, QString secondColumnName) {
    if (tableView->isColumnHidden(headerIndex(model, firstColumnName, Qt::Horizontal))) {
        for (int i = 0; i < model->rowCount(); i++) {
            QString toolTip = model->item(i, headerIndex(model, firstColumnName, Qt::Horizontal))->text();
            model->item(i, headerIndex(model, firstColumnName, Qt::Horizontal))->setToolTip("");
            model->item(i, headerIndex(model, secondColumnName, Qt::Horizontal))->setToolTip(toolTip);
        }
    }
    else if (tableView->isColumnHidden(headerIndex(model, secondColumnName, Qt::Horizontal))) {
        for (int i = 0; i < model->rowCount(); i++) {
            QString toolTip = model->item(i, headerIndex(model, secondColumnName, Qt::Horizontal))->text();
            model->item(i, headerIndex(model, secondColumnName, Qt::Horizontal))->setToolTip("");
            model->item(i, headerIndex(model, firstColumnName, Qt::Horizontal))->setToolTip(toolTip);
        }
    }
    else {
        for (int i = 0; i < model->rowCount(); i++) {
            model->item(i, headerIndex(model, firstColumnName, Qt::Horizontal))->setToolTip("");
            model->item(i, headerIndex(model, secondColumnName, Qt::Horizontal))->setToolTip("");
        }
    }
}

void WiiBaFu::setGameInfo(QTableView *tableView, QStandardItemModel *model) {
    if (tableView->selectionModel() && !tableView->selectionModel()->selectedRows(0).isEmpty()) {

        if (!ui->infoTab_lineEdit_ID->text().contains(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text())) {
            if (model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text().contains("PAL") || model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text().contains("RF")) {
                ui->infoTab_comboBox_CoverLanguages->setCurrentIndex(WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt());
            }
            else if (model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text().contains("NTSC")) {
                ui->infoTab_comboBox_CoverLanguages->setCurrentIndex(1);
            }

            ui->infoTab_label_GameCover->clear();
            common->requestGameCover(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text(), currentCoverLanguage(), Common::ThreeD);
        }

        if (tableView == ui->wbfsTab_tableView) {
            ui->infoTab_lineEdit_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
            ui->infoTab_lineEdit_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
            ui->infoTab_lineEdit_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
            ui->infoTab_lineEdit_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
            ui->infoTab_lineEdit_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());
            ui->infoTab_lineEdit_UsedBlocks->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            ui->infoTab_lineEdit_Insertion->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
            ui->infoTab_lineEdit_LastModification->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->infoTab_lineEdit_LastStatusChange->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->text());
            ui->infoTab_lineEdit_LastAccess->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->text());
            ui->infoTab_lineEdit_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
            ui->infoTab_lineEdit_WBFSSlot->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(11).first())->text());
            ui->infoTab_lineEdit_Source->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(12).first())->text());
        }
        else {
            ui->infoTab_lineEdit_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
            ui->infoTab_lineEdit_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
            ui->infoTab_lineEdit_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
            ui->infoTab_lineEdit_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
            ui->infoTab_lineEdit_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());
            ui->infoTab_lineEdit_UsedBlocks->setText("--");
            ui->infoTab_lineEdit_Insertion->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            ui->infoTab_lineEdit_LastModification->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
            ui->infoTab_lineEdit_LastStatusChange->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->infoTab_lineEdit_LastAccess->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->text());
            ui->infoTab_lineEdit_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->text());
            ui->infoTab_lineEdit_WBFSSlot->setText("--");
            ui->infoTab_lineEdit_Source->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
        }

        ui->tabWidget->setCurrentIndex(3);
    }
}

void WiiBaFu::showGameDiscCover(QImage *gameCover) {
    ui->dvdTab_label_DiscCover->setPixmap(QPixmap::fromImage(*gameCover, Qt::AutoColor));
}

void WiiBaFu::showGame3DCover(QImage *gameCover) {
    ui->infoTab_label_GameCover->setPixmap(QPixmap::fromImage(*gameCover, Qt::AutoColor));
}

void WiiBaFu::showGameFullHQCover(QImage *gameFullHQCover) {
    coverViewDialog->setCover(gameFullHQCover, ui->infoTab_lineEdit_ID->text());
    coverViewDialog->show();
}

void WiiBaFu::transferGamesToWBFSsuccesfully() {
    ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
    on_wbfsTab_pushButton_Load_clicked();
}

void WiiBaFu::transferGamesToWBFScanceled(bool discExitst) {
    if (discExitst) {
        ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Disc already exists!"), WiTools::Info);
        setStatusBarText(tr("Disc already exists!"));
    }
    else {
        ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Transfer canceled!"), WiTools::Info);
        setStatusBarText(tr("Transfer canceled!"));
    }
}

void WiiBaFu::transferGameFromDVDToWBFSsuccesfully() {
    ui->dvdTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
    on_wbfsTab_pushButton_Load_clicked();
}

void WiiBaFu::transferGameFromDVDToWBFScanceled(bool discExitst) {
    if (discExitst) {
        ui->dvdTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Disc already exists!"), WiTools::Info);
        setStatusBarText(tr("Disc already exists!"));
    }
    else {
        ui->dvdTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Transfer canceled!"), WiTools::Info);
        setStatusBarText(tr("Transfer canceled!"));
    }
}

void WiiBaFu::transferGameFromDVDToImageSuccesfully() {
    ui->dvdTab_pushButton_TransferToImage->setText(tr("Transfer to image"));
    setStatusBarText(tr("Ready."));
}

void WiiBaFu::transferGameFromDVDToImageCanceled(bool discExitst) {
    if (discExitst) {
        ui->dvdTab_pushButton_TransferToImage->setText(tr("Transfer to image"));
        addEntryToLog(tr("Disc already exists!"), WiTools::Info);
        setStatusBarText(tr("Disc already exists!"));
    }
    else {
        ui->dvdTab_pushButton_TransferToImage->setText(tr("Transfer to image"));
        addEntryToLog(tr("Transfer canceled!"), WiTools::Info);
        setStatusBarText(tr("Transfer canceled!"));
    }
}

void WiiBaFu::transferGamesToImageSuccesfully() {
    if (ui->wbfsTab_pushButton_Transfer->text().contains(tr("Cancel transfering"))) {
        ui->wbfsTab_pushButton_Transfer->setText(tr("Transfer"));
    }
    else if (ui->filesTab_pushButton_TransferToImage->text().contains(tr("Cancel transfering"))) {
        ui->filesTab_pushButton_TransferToImage->setText(tr("Transfer to image"));
    }

    setStatusBarText(tr("Ready."));
}

void WiiBaFu::transferGamesToImageCanceled() {
    if (ui->wbfsTab_pushButton_Transfer->text().contains(tr("Cancel transfering"))) {
        ui->wbfsTab_pushButton_Transfer->setText(tr("Transfer"));
    }
    else if (ui->filesTab_pushButton_TransferToImage->text().contains(tr("Cancel transfering"))) {
        ui->filesTab_pushButton_TransferToImage->setText(tr("Transfer to image"));
    }

    addEntryToLog(tr("Transfer canceled!"), WiTools::Info);
    setStatusBarText(tr("Transfer canceled!"));
}

void WiiBaFu::setWBFSInfoText(QString text) {
    ui->wbfsTab_label_Info->setText(text);
}

void WiiBaFu::setWBFSProgressBar(int min, int max, int value, QString format) {
    ui->wbfsTab_progressBar->setMinimum(min);
    ui->wbfsTab_progressBar->setMaximum(max);
    ui->wbfsTab_progressBar->setValue(value);
    ui->wbfsTab_progressBar->setFormat(format);
}

void WiiBaFu::setMainProgressBar(int value, QString format) {
    progressBar_Main->setValue(value);
    progressBar_Main->setFormat(format);
}

void WiiBaFu::startMainProgressBarBusy() {
    timer->start(3000);
}

void WiiBaFu::showMainProgressBarBusy() {
    setMainProgressBarVisible(true);

    progressBar_Main->setMinimum(0);
    progressBar_Main->setMaximum(0);
    progressBar_Main->setValue(0);
}

void WiiBaFu::stopMainProgressBarBusy() {
    if (timer->isActive()) {
        timer->stop();
    }

    setMainProgressBarVisible(false);

    progressBar_Main->setMinimum(0);
    progressBar_Main->setMaximum(100);
    progressBar_Main->setValue(0);
}

void WiiBaFu::setStatusBarText(QString text) {
    ui->statusBar->showMessage(text);
}

void WiiBaFu::addEntryToLog(QString entry, WiTools::LogType type) {
    switch (WiiBaFuSettings.value("Main/Logging", QVariant(0)).toInt()) {
    case 0:
            ui->logTab_plainTextEdit_Log->appendPlainText(entry);
            break;
    case 1:
            if (type == WiTools::Error) {
                ui->logTab_plainTextEdit_Log->appendPlainText(entry);
            }
            break;
    }
}

QString WiiBaFu::currentCoverLanguage() {
    switch (ui->infoTab_comboBox_CoverLanguages->currentIndex()) {
        case 0:  return "EN";
                 break;
        case 1:  return "US";
                 break;
        case 2:  return "FR";
                 break;
        case 3:  return "DE";
                 break;
        case 4:  return "ES";
                 break;
        case 5:  return "IT";
                 break;
        case 6:  return "NL";
                 break;
        case 7:  return "PT";
                 break;
        case 8:  return "SE";
                 break;
        case 9:  return "DK";
                 break;
        case 10:  return "NO";
                 break;
        case 11: return "FI";
                 break;
        case 12: return "RU";
                 break;
        case 13: return "JA";
                 break;
        case 14: return "KO";
                 break;
        case 15: return "ZH-tw";
                 break;
        case 16: return "ZH-cn";
                 break;
        default: return "EN";
    }
}

QString WiiBaFu::wbfsPath() {
    if (WiiBaFuSettings.value("WIT/Auto", QVariant(true)).toBool()) {
        return QString("");
    }
    else {
        return WiiBaFuSettings.value("WIT/WBFSPath", QVariant("")).toString();
    }
}

int WiiBaFu::headerIndex(QAbstractItemModel *model, QString text, Qt::Orientation orientation) {
    for (int i = 0; i < model->columnCount(); i++) {
        if (model->headerData(i, orientation).toString().contains(text)) {
            return i;
        }
    }

    return -1;
}

void WiiBaFu::saveMainWindowGeometry() {
    WiiBaFuSettings.setValue("MainWindow/x", this->geometry().x());
    WiiBaFuSettings.setValue("MainWindow/y", this->geometry().y());
    WiiBaFuSettings.setValue("MainWindow/width", this->geometry().width());
    WiiBaFuSettings.setValue("MainWindow/height", this->geometry().height());
}

void WiiBaFu::saveGameListHeaderStates() {
    if (ui->filesTab_tableView->horizontalHeader()->count() > 0 ) {
        WiiBaFuSettings.setValue("GameLists/Files_HeaderStates", ui->filesTab_tableView->horizontalHeader()->saveState());
    }

    if (ui->wbfsTab_tableView->horizontalHeader()->count() > 0 ) {
        WiiBaFuSettings.setValue("GameLists/WBFS_HeaderStates", ui->wbfsTab_tableView->horizontalHeader()->saveState());
    }
}

void WiiBaFu::setFilesColumns() {
    if (WiiBaFuSettings.value("FilesToWBFS/columnID", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(0);
    }
    else {
        ui->filesTab_tableView->hideColumn(0);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnName", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(1);
    }
    else {
        ui->filesTab_tableView->hideColumn(1);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnTitle", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(2);
    }
    else {
        ui->filesTab_tableView->hideColumn(2);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnRegion", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(3);
    }
    else {
        ui->filesTab_tableView->hideColumn(3);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnSize", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(4);
    }
    else {
        ui->filesTab_tableView->hideColumn(4);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnInsertion", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(5);
    }
    else {
        ui->filesTab_tableView->hideColumn(5);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnLastModification", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(6);
    }
    else {
        ui->filesTab_tableView->hideColumn(6);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnLastStatusChange", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(7);
    }
    else {
        ui->filesTab_tableView->hideColumn(7);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnLastAccess", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(8);
    }
    else {
        ui->filesTab_tableView->hideColumn(8);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnType", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(9);
    }
    else {
        ui->filesTab_tableView->hideColumn(9);
    }

    if (WiiBaFuSettings.value("FilesToWBFS/columnSource", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(10);
    }
    else {
        ui->filesTab_tableView->hideColumn(10);
    }
}

void WiiBaFu::setWBFSColumns() {
    if (WiiBaFuSettings.value("WBFStoFiles/columnID", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(0);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(0);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnName", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(1);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(1);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnTitle", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(2);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(2);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnRegion", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(3);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(3);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnSize", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(4);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(4);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnUsedBlocks", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(5);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(5);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnInsertion", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(6);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(6);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnLastModification", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(7);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(7);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnLastStatusChange", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(8);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(8);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnLastAccess", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(9);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(9);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnType", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(10);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(10);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnWBFSSlot", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(11);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(11);
    }

    if (WiiBaFuSettings.value("WBFStoFiles/columnSource", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(12);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(12);
    }
}

void WiiBaFu::on_menuHelp_About_triggered() {
    QMessageBox::about(this, tr("About Wii Backup Fusion"),
        QString("<h2>Wii Backup Fusion %1</h2>").arg(QCoreApplication::applicationVersion()) +
        tr("<p><b><i>The complete and simply to use backup solution for Wii games</b></i>"
        "<p>Copyright &copy; 2010 Kai Heitkamp"
        "<p><a href='mailto:dynup<dynup@ymail.com>?subject=WiiBaFu%20feedback'>dynup@ymail.com</a>"
        " | <a href='http://wiibafu.codeplex.com'>wiibafu.codeplex.com</a>"
        "<p><font color='red'>I don't support piracy! If you copy games with this software,"
        "<br>you must have the original and it's for your private use only!</font color>"
        "<p>Big thanks to the trolls at Trolltech Norway for his excellent Qt toolkit"
        "<br>and the guys at Nokia for the continuation, thanks to Dirk Clemens (Wiimm) for his great ISO tools!"
        "<p><i>Dedicated in memory of my father G&uuml;nter Heitkamp (28.07.1935 - 06.10.2009)</i>"));
}

WiiBaFu::~WiiBaFu() {
    saveMainWindowGeometry();
    saveGameListHeaderStates();

    delete wiTools;
    delete common;
    delete settings;
    delete wiibafudialog;
    delete coverViewDialog;
    delete wbfsDialog;
    delete filesListModel;
    delete dvdListModel;
    delete wbfsListModel;
    delete progressBar_Main;
    delete timer;
    delete ui;
}
