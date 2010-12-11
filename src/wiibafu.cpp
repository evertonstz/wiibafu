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
        addEntryToLog(tr("Titles not found!\n"), WiTools::Info);
    }
    else {
        addEntryToLog(QDir::toNativeSeparators(tr("Titles found in: %1\n").arg(witPath)), WiTools::Info);
    }
}

void WiiBaFu::setupConnections() {
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<WiTools::LogType>("WiTools::LogType");
    qRegisterMetaType<WiTools::WitStatus>("WiTools::WitStatus");

    connect(this, SIGNAL(cancelTransfer()), wiTools, SLOT(cancelTransfer()));
    connect(this, SIGNAL(cancelLoading()), wiTools, SLOT(cancelLoading()));
    connect(this, SIGNAL(cancelVerifying()), wiTools, SLOT(cancelVerifying()));

    connect(this, SIGNAL(startBusy()), this, SLOT(startMainProgressBarBusy()));
    connect(this, SIGNAL(stopBusy()), this, SLOT(stopMainProgressBarBusy()));
    connect(timer, SIGNAL(timeout()), this, SLOT(showMainProgressBarBusy()));

    connect(wiTools, SIGNAL(setMainProgressBar(int, QString)), this, SLOT(setMainProgressBar(int,QString)));
    connect(wiTools, SIGNAL(setMainProgressBarVisible(bool)), this, SLOT(setMainProgressBarVisible(bool)));

    connect(wiTools, SIGNAL(setInfoTextWBFS(QString)), this, SLOT(setWBFSInfoText(QString)));
    connect(wiTools, SIGNAL(setProgressBarWBFS(int, int, int, QString)), this, SLOT(setWBFSProgressBar(int, int, int, QString)));
    connect(wiTools, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(wiTools, SIGNAL(newLogEntry(QString, WiTools::LogType)), this, SLOT(addEntryToLog(QString, WiTools::LogType)));
    connect(wiTools, SIGNAL(newLogEntries(QStringList,WiTools::LogType)), this, SLOT(addEntriesToLog(QStringList,WiTools::LogType)));
    connect(wiTools, SIGNAL(newWitCommandLineLogEntry(QString,QStringList)), this, SLOT(addWitCommandLineToLog(QString,QStringList)));

    connect(wiTools, SIGNAL(newFilesGameListModel()), this, SLOT(setFilesGameListModel()));
    connect(wiTools, SIGNAL(loadingGamesCanceled()), this, SLOT(loadingGamesCanceled()));
    connect(wiTools, SIGNAL(loadingGamesFailed(WiTools::WitStatus)), this, SLOT(loadingGamesFailed(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(newDVDGameListModel()), this, SLOT(setDVDGameListModel()));
    connect(wiTools, SIGNAL(newWBFSGameListModel()), this, SLOT(setWBFSGameListModel()));

    connect(wiTools, SIGNAL(transferFilesToWBFS_finished(WiTools::WitStatus)), this, SLOT(transferFilesToWBFS_finished(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(transferFilesToImage_finished(WiTools::WitStatus)), this, SLOT(transferFilesToImage_finished(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(extractImage_finished(WiTools::WitStatus)), this, SLOT(extractImage_finished(WiTools::WitStatus)));

    connect(wiTools, SIGNAL(transferDVDToWBFS_finished(WiTools::WitStatus)), this, SLOT(transferDVDToWBFS_finished(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(transferDVDToImage_finished(WiTools::WitStatus)), this, SLOT(transferDVDToImage_finished(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(extractDVD_finished(WiTools::WitStatus)), this, SLOT(extractDVD_finished(WiTools::WitStatus)));

    connect(wiTools, SIGNAL(transferWBFSToImage_finished(WiTools::WitStatus)), this, SLOT(transferWBFSToImage_finished(WiTools::WitStatus)));
    connect(wiTools, SIGNAL(extractWBFS_finished(WiTools::WitStatus)), this, SLOT(extractWBFS_finished(WiTools::WitStatus)));

    connect(wiTools, SIGNAL(removeGamesFromWBFS_successfully()), this, SLOT(on_wbfsTab_pushButton_Load_clicked()));

    connect(wiTools, SIGNAL(verifyGame_finished(WiTools::WitStatus)), this, SLOT(verifyGame_finished(WiTools::WitStatus)));

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
    progressBar_Main->setObjectName("progressBarMain");
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
    gameTableView->setShowGrid(WiiBaFuSettings.value("GameListBehavior/ShowGrid", QVariant(false)).toBool());
    gameTableView->setAlternatingRowColors(WiiBaFuSettings.value("GameListBehavior/AlternatingRowColors", QVariant(true)).toBool());
    gameTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    gameTableView->setHorizontalScrollMode((QHeaderView::ScrollMode)WiiBaFuSettings.value("GameListBehavior/ScrollMode", QVariant(QHeaderView::ScrollPerPixel)).toInt());
    gameTableView->setVerticalScrollMode((QHeaderView::ScrollMode)WiiBaFuSettings.value("GameListBehavior/ScrollMode", QVariant(QHeaderView::ScrollPerPixel)).toInt());

    if (gameTableView != ui->dvdTab_tableView) {
        gameTableView->verticalHeader()->hide();
        gameTableView->horizontalHeader()->setMovable(true);
        gameTableView->verticalHeader()->setDefaultSectionSize(20);
        gameTableView->horizontalHeader()->setResizeMode((QHeaderView::ResizeMode)WiiBaFuSettings.value("GameListBehavior/ResizeMode", QVariant(QHeaderView::ResizeToContents)).toInt());
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

    gameTableView->setLocale(locale());
}

void WiiBaFu::on_menuOptions_Settings_triggered() {
    int language = WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt();

    if (settings->exec() == QDialog::Accepted) {
        setGameListAttributes(ui->filesTab_tableView);
        setGameListAttributes(ui->dvdTab_tableView);
        setGameListAttributes(ui->wbfsTab_tableView);

        setFilesColumns();
        setWBFSColumns();

        if (WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt() != language) {
            updateTitles();

            if (!ui->infoTab_lineEdit_ID->text().isEmpty()) {
                if (ui->infoTab_lineEdit_UsedBlocks->text().contains("--")) {
                    setGameInfoDateTimes(ui->filesTab_tableView, filesListModel);
                }
                else {
                    setGameInfoDateTimes(ui->wbfsTab_tableView, wbfsListModel);
                }
            }
        }

        if (WiiBaFuSettings.value("GameListBehavior/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->filesTab_tableView, filesListModel, tr("Title"), tr("Name"));
            setToolTips(ui->wbfsTab_tableView, wbfsListModel, tr("Title"), tr("Name"));
            ui->filesTab_tableView->update();
            ui->wbfsTab_tableView->update();
        }

        ui->infoTab_comboBox_GameLanguages->setCurrentIndex(WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt());
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

void WiiBaFu::on_menuTools_VerifyGame_triggered() {
    if (!ui->menuTools_VerifyGame->text().contains(tr("&Cancel verifying"))) {
        QString game;

        switch (ui->tabWidget->currentIndex()) {
            case 0:
                    if (filesListModel->rowCount() > 0 && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
                        game = filesListModel->itemFromIndex(ui->filesTab_tableView->selectionModel()->selectedRows(10).first())->text();
                    }
                    break;
            case 1:
                    if (dvdListModel->rowCount() > 0) {
                        game = dvdListModel->index(15, 0).data().toString();
                    }
                    break;
            case 2:
                    if (wbfsListModel->rowCount() > 0 && !ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
                        game = wbfsListModel->itemFromIndex(ui->wbfsTab_tableView->selectionModel()->selectedRows(0).first())->text();
                    }
                    break;
        }

        if (!game.isEmpty()) {
            emit startBusy();
            ui->menuTools_VerifyGame->setText(tr("&Cancel verifying"));
            QtConcurrent::run(wiTools, &WiTools::verifyGame, ui->tabWidget->currentIndex(), wbfsPath(), game);
        }
        else {
            setStatusBarText(tr("Error: No game!"));
            addEntryToLog(tr("Verify error: No game!"), WiTools::Error);
        }
    }
    else {
        emit cancelVerifying();
    }
}

void WiiBaFu::on_menuTools_Compare_triggered() {
    if ((filesListModel->rowCount() > 0 && wbfsListModel->rowCount() > 0) && (ui->tabWidget->currentIndex() == 0 || ui->tabWidget->currentIndex() == 2)) {
        QTableView *tableView;
        QStandardItemModel *sourceModel, *targetModel;


        switch (ui->tabWidget->currentIndex()) {
            case 0:
                    tableView = ui->filesTab_tableView;
                    sourceModel = filesListModel;
                    targetModel = wbfsListModel;
                    break;
            case 2:
                    tableView = ui->wbfsTab_tableView;
                    sourceModel = wbfsListModel;
                    targetModel = filesListModel;
                    break;
            default:
                    return;
        }

        tableView->setSelectionMode(QAbstractItemView::MultiSelection);

        for (int i = 0; sourceModel->rowCount() > i; ++i) {
            if (!gameInList(targetModel, sourceModel->item(i, 0)->text())) {
                tableView->selectRow(i);
            }
        }

        tableView->setFocus();
        tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
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
    if (!ui->filesTab_pushButton_Load->text().contains(tr("&Cancel loading"))) {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Open directory"), WiiBaFuSettings.value("Main/LastFilesPath", QVariant(QDir::homePath()).toString()).toString(), QFileDialog::ShowDirsOnly);

        if (!directory.isEmpty()) {
            emit startBusy();

            ui->filesTab_pushButton_Load->setText(tr("&Cancel loading"));
            WiiBaFuSettings.setValue("Main/LastFilesPath", directory);
            QtConcurrent::run(wiTools, &WiTools::requestFilesGameListModel, filesListModel, directory);
        }
    }
    else {
        emit cancelLoading();
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
    if (!ui->filesTab_pushButton_TransferToWBFS->text().contains(tr("&Cancel transfering"))) {
        if (ui->filesTab_tableView->selectionModel() && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            ui->filesTab_pushButton_TransferToWBFS->setText(tr("&Cancel transfering"));

            QtConcurrent::run(wiTools, &WiTools::transferFilesToWBFS, ui->filesTab_tableView->selectionModel()->selectedRows(10), wbfsPath());
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_filesTab_pushButton_TransferToImage_clicked() {
    if (!ui->filesTab_pushButton_TransferToImage->text().contains(tr("&Cancel transfering"))) {
        if (ui->filesTab_tableView->model() && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenImageDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->directory().isEmpty()) {
                QDir path = wiibafudialog->directory();
                QString format = wiibafudialog->imageFormat();
                QString compression = wiibafudialog->compression();

                if (!path.exists()) {
                    QMessageBox::warning(this, tr("Warning"), tr("The directory doesn't exists!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
                else {
                    ui->filesTab_pushButton_TransferToImage->setText(tr("&Cancel transfering"));
                    QtConcurrent::run(wiTools, &WiTools::transferFilesToImage, ui->filesTab_tableView->selectionModel()->selectedRows(10), format, compression, path.absolutePath());
                }
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_filesTab_pushButton_ExtractImage_clicked() {
    if (!ui->filesTab_pushButton_ExtractImage->text().contains(tr("&Cancel extracting"))) {
        if (filesListModel->rowCount() > 0 && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->directory().isEmpty()) {
                ui->filesTab_pushButton_ExtractImage->setText(tr("&Cancel extracting"));
                QtConcurrent::run(wiTools, &WiTools::extractImage, ui->filesTab_tableView->selectionModel()->selectedRows(10), buildPath(wiibafudialog->directory(), filesListModel, ui->filesTab_tableView));
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
    if (!ui->dvdTab_pushButton_TransferToWBFS->text().contains(tr("&Cancel transfering"))) {
        if (dvdListModel->rowCount() > 0) {
            ui->dvdTab_pushButton_TransferToWBFS->setText(tr("&Cancel transfering"));

            QtConcurrent::run(wiTools, &WiTools::transferDVDToWBFS, dvdListModel->index(15, 0).data().toString(), wbfsPath());
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_dvdTab_pushButton_TransferToImage_clicked() {
    if (!ui->dvdTab_pushButton_TransferToImage->text().contains(tr("&Cancel transfering"))) {
        if (dvdListModel->rowCount() > 0) {
            wiibafudialog->setOpenFile();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->filePath().isEmpty()) {
                QString filePath = wiibafudialog->filePath();
                QString format = wiibafudialog->imageFormat();
                QString compression = wiibafudialog->compression();
                QString dvdPath = WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/dev/sr0")).toString();

                ui->dvdTab_pushButton_TransferToImage->setText(tr("&Cancel transfering"));
                QtConcurrent::run(wiTools, &WiTools::transferDVDToImage, dvdPath, format, compression, filePath);
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_dvdTab_pushButton_Extract_clicked() {
    if (!ui->dvdTab_pushButton_Extract->text().contains(tr("&Cancel extracting"))) {
        if (dvdListModel->rowCount() > 0) {
            wiibafudialog->setOpenDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->directory().isEmpty()) {
                QString directory = buildPath(wiibafudialog->directory(), dvdListModel, ui->dvdTab_tableView);
                QString dvdPath = WiiBaFuSettings.value("WIT/DVDDrivePath", QVariant("/dev/sr0")).toString();

                ui->dvdTab_pushButton_Extract->setText(tr("&Cancel extracting"));
                QtConcurrent::run(wiTools, &WiTools::extractDVD, dvdPath, directory);
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
    if (!ui->wbfsTab_pushButton_Transfer->text().contains(tr("&Cancel transfering"))) {
        if (ui->wbfsTab_tableView->model() && !ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenImageDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->directory().isEmpty()) {
                QDir path = wiibafudialog->directory();
                QString format = wiibafudialog->imageFormat();
                QString compression = wiibafudialog->compression();

                if (!path.exists() && !wiibafudialog->directory().isEmpty()) {
                    QMessageBox::warning(this, tr("Warning"), tr("The directory doesn't exists!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
                else {
                    ui->wbfsTab_pushButton_Transfer->setText(tr("&Cancel transfering"));
                    QtConcurrent::run(wiTools, &WiTools::transferWBFSToImage, ui->wbfsTab_tableView->selectionModel()->selectedRows(0), wbfsPath(), format, compression, path.absolutePath());
                }
            }
        }
    }
    else {
        emit cancelTransfer();
    }
}

void WiiBaFu::on_wbfsTab_pushButton_Extract_clicked() {
    if (!ui->wbfsTab_pushButton_Extract->text().contains(tr("&Cancel extracting"))) {
        if (wbfsListModel->rowCount() > 0 && !ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            wiibafudialog->setOpenDirectory();

            if (wiibafudialog->exec() == QDialog::Accepted && !wiibafudialog->directory().isEmpty()) {
                ui->wbfsTab_pushButton_Extract->setText(tr("&Cancel extracting"));
                QtConcurrent::run(wiTools, &WiTools::extractWBFS, ui->wbfsTab_tableView->selectionModel()->selectedRows(0), wbfsPath(), buildPath(wiibafudialog->directory(), wbfsListModel, ui->wbfsTab_tableView));
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
    if (!ui->infoTab_lineEdit_ID->text().isEmpty()) {
        ui->infoTab_label_GameCover->clear();
        common->requestGameCover(ui->infoTab_lineEdit_ID->text(), currentGameLanguage(), Common::ThreeD);
    }
}

void WiiBaFu::on_infoTab_pushButton_LoadFullHQCover_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty()) {
        common->requestGameCover(ui->infoTab_lineEdit_ID->text(), currentGameLanguage(), Common::HighQuality);
    }
}

void WiiBaFu::on_infoTab_pushButton_viewInBrowser_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty()) {
        common->viewInBrowser(ui->infoTab_lineEdit_ID->text());
    }
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

        ui->filesTab_tableView->horizontalHeader()->restoreState(WiiBaFuSettings.value("FilesGameList/HeaderStates").toByteArray());

        setFilesColumns();
        if (WiiBaFuSettings.value("GameListBehavior/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->filesTab_tableView, filesListModel, tr("Title"), tr("Name"));
        }

        emit stopBusy();
        ui->filesTab_pushButton_Load->setText(tr("&Load"));
        ui->filesTab_pushButton_Load->setShortcut(tr("Ctrl+L"));
        setStatusBarText(tr("Ready."));
    }
}

void WiiBaFu::loadingGamesCanceled() {
    ui->filesTab_pushButton_Load->setText(tr("&Load"));
    ui->filesTab_pushButton_Load->setShortcut(tr("Ctrl+L"));

    setStatusBarText(tr("Loading canceled!"));
    emit stopBusy();
}

void WiiBaFu::loadingGamesFailed(WiTools::WitStatus) {
    emit stopBusy();
    ui->filesTab_pushButton_Load->setText(tr("&Load"));
    ui->filesTab_pushButton_Load->setShortcut(tr("Ctrl+L"));
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

        ui->wbfsTab_tableView->horizontalHeader()->restoreState(WiiBaFuSettings.value("WBFSGameList/HeaderStates").toByteArray());

        setWBFSColumns();
        if (WiiBaFuSettings.value("GameListBehavior/ToolTips", QVariant(false)).toBool()) {
            setToolTips(ui->wbfsTab_tableView, wbfsListModel, tr("Title"), tr("Name"));
        }

        emit stopBusy();
        setStatusBarText(tr("Ready."));
    }
}

void WiiBaFu::transferFilesToWBFS_finished(WiTools::WitStatus status) {
    if (status == WiTools::Ok) {
        on_wbfsTab_pushButton_Load_clicked();
    }

    ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to &WBFS"));
}

void WiiBaFu::transferFilesToImage_finished(WiTools::WitStatus) {
    ui->filesTab_pushButton_TransferToImage->setText(tr("Transfer to &image"));
}

void WiiBaFu::extractImage_finished(WiTools::WitStatus) {
    ui->filesTab_pushButton_ExtractImage->setText(tr("E&xtract image"));
}

void WiiBaFu::transferDVDToWBFS_finished(WiTools::WitStatus status) {
    if (status == WiTools::Ok) {
        on_wbfsTab_pushButton_Load_clicked();
    }

    ui->dvdTab_pushButton_TransferToWBFS->setText(tr("Transfer to &WBFS"));
}

void WiiBaFu::transferDVDToImage_finished(WiTools::WitStatus) {
    ui->dvdTab_pushButton_TransferToImage->setText(tr("Transfer to &image"));
}

void WiiBaFu::extractDVD_finished(WiTools::WitStatus) {
    ui->dvdTab_pushButton_Extract->setText(tr("E&xtract"));
}

void WiiBaFu::transferWBFSToImage_finished(WiTools::WitStatus) {
    ui->wbfsTab_pushButton_Transfer->setText(tr("&Transfer"));
}

void WiiBaFu::extractWBFS_finished(WiTools::WitStatus) {
    ui->wbfsTab_pushButton_Extract->setText(tr("E&xtract"));
}

void WiiBaFu::verifyGame_finished(WiTools::WitStatus) {
    emit stopBusy();
    ui->menuTools_VerifyGame->setText(tr("&Verify game"));
    ui->tabWidget->setCurrentIndex(4);
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

QString WiiBaFu::buildPath(QString directory, QStandardItemModel *model, QTableView *tableView) {
    QString path;
    QString gameId;
    QString gameTitle;
    QDir dir(directory);

    if (dir.exists()) {
        if (tableView != ui->dvdTab_tableView) {
            gameId = model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text();
            gameTitle = model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text();
        }
        else {
            gameId = model->index(0, 0).data(Qt::DisplayRole).toString();
            gameTitle = model->index(2, 0).data(Qt::DisplayRole).toString();
        }

        path = dir.path().append("/").append(gameTitle).append(" [").append(gameId).append("]");
    }
    else {
        path = dir.path();
    }

    return path;
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
                ui->infoTab_comboBox_GameLanguages->setCurrentIndex(WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt());
            }
            else if (model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text().contains("NTSC")) {
                ui->infoTab_comboBox_GameLanguages->setCurrentIndex(1);
            }

            ui->infoTab_label_GameCover->clear();
            common->requestGameCover(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text(), currentGameLanguage(), Common::ThreeD);
        }

        if (tableView == ui->wbfsTab_tableView) {
            ui->infoTab_lineEdit_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
            ui->infoTab_lineEdit_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
            ui->infoTab_lineEdit_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
            ui->infoTab_lineEdit_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
            ui->infoTab_lineEdit_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());
            ui->infoTab_lineEdit_UsedBlocks->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            setGameInfoDateTimes(ui->wbfsTab_tableView, wbfsListModel);
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
            setGameInfoDateTimes(ui->filesTab_tableView, filesListModel);
            ui->infoTab_lineEdit_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->text());
            ui->infoTab_lineEdit_WBFSSlot->setText("--");
            ui->infoTab_lineEdit_Source->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
        }

        ui->tabWidget->setCurrentIndex(3);
    }
}

void WiiBaFu::setGameInfoDateTimes(QTableView *tableView, QStandardItemModel *model) {
    QDateTime dateTime;
    QString dateTimeFormat = locale().dateTimeFormat(QLocale::ShortFormat);

    if (tableView == ui->wbfsTab_tableView) {
        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_Insertion->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastModification->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastStatusChange->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastAccess->setText(dateTime.toString(dateTimeFormat));
    }
    else {
        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_Insertion->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastModification->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastStatusChange->setText(dateTime.toString(dateTimeFormat));

        dateTime = model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->data(Qt::DisplayRole).value<QDateTime>();
        ui->infoTab_lineEdit_LastAccess->setText(dateTime.toString(dateTimeFormat));
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
    progressBar_Main->setValue(-1);
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

void WiiBaFu::addEntriesToLog(QStringList entries, WiTools::LogType type) {
    QString tmpstr;

    foreach (QString entry, entries) {
        tmpstr.append(entry);
        tmpstr.append(" ");
    }

    addEntryToLog(tmpstr, type);
}

void WiiBaFu::addWitCommandLineToLog(QString wit, QStringList arguments) {
    if (WiiBaFuSettings.value("Main/LogWitCommandLine", QVariant(true)).toBool()) {
        addEntriesToLog(QStringList() << tr("WIT command line:\n%1").arg(wit) << arguments << "\n" , WiTools::Info);
    }
}

QString WiiBaFu::currentGameLanguage() {
    switch (ui->infoTab_comboBox_GameLanguages->currentIndex()) {
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
        case 10: return "NO";
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

QLocale WiiBaFu::locale() {
    QLocale locale;

    switch (WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt()) {
        case 0:  locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
                 break;
        case 1:  locale = QLocale(QLocale::English, QLocale::UnitedStates);
                 break;
        case 2:  locale = QLocale(QLocale::French, QLocale::France);
                 break;
        case 3:  locale = QLocale(QLocale::German, QLocale::Germany);
                 break;
        case 4:  locale = QLocale(QLocale::Spanish, QLocale::Spain);
                 break;
        case 5:  locale = QLocale(QLocale::Italian, QLocale::Italy);
                 break;
        case 6:  locale = QLocale(QLocale::Dutch, QLocale::Netherlands);
                 break;
        case 7:  locale = QLocale(QLocale::Portuguese, QLocale::Portugal);
                 break;
        case 8:  locale = QLocale(QLocale::Swedish, QLocale::Sweden);
                 break;
        case 9:  locale = QLocale(QLocale::Danish, QLocale::Denmark);
                 break;
        case 10: locale = QLocale(QLocale::NorthernSami, QLocale::Norway);
                 break;
        case 11: locale = QLocale(QLocale::Finnish, QLocale::Finland);
                 break;
        case 12: locale = QLocale(QLocale::Russian, QLocale::RussianFederation);
                 break;
        case 13: locale = QLocale(QLocale::Japanese, QLocale::Japan);
                 break;
        case 14: locale = QLocale(QLocale::Korean, QLocale::DemocraticRepublicOfKorea);
                 break;
        case 15: locale = QLocale(QLocale::Chinese, QLocale::Taiwan);
                 break;
        case 16: locale = QLocale(QLocale::Chinese, QLocale::China);
                 break;
        default: locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    }

    return locale;
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

bool WiiBaFu::gameInList(QStandardItemModel *model, QString gameId) {
    for (int i = 0; model->rowCount() > i; ++i) {
        if (model->item(i, 0)->text().contains(gameId)) {
            return true;
        }
    }

    return false;
}

void WiiBaFu::saveMainWindowGeometry() {
    WiiBaFuSettings.setValue("MainWindow/x", this->geometry().x());
    WiiBaFuSettings.setValue("MainWindow/y", this->geometry().y());
    WiiBaFuSettings.setValue("MainWindow/width", this->geometry().width());
    WiiBaFuSettings.setValue("MainWindow/height", this->geometry().height());
}

void WiiBaFu::saveGameListHeaderStates() {
    if (ui->filesTab_tableView->horizontalHeader()->count() > 0 ) {
        WiiBaFuSettings.setValue("FilesGameList/HeaderStates", ui->filesTab_tableView->horizontalHeader()->saveState());
    }

    if (ui->wbfsTab_tableView->horizontalHeader()->count() > 0 ) {
        WiiBaFuSettings.setValue("WBFSGameList/HeaderStates", ui->wbfsTab_tableView->horizontalHeader()->saveState());
    }
}

void WiiBaFu::setFilesColumns() {
    if (WiiBaFuSettings.value("FilesGameList/columnID", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(0);
    }
    else {
        ui->filesTab_tableView->hideColumn(0);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnName", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(1);
    }
    else {
        ui->filesTab_tableView->hideColumn(1);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnTitle", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(2);
    }
    else {
        ui->filesTab_tableView->hideColumn(2);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnRegion", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(3);
    }
    else {
        ui->filesTab_tableView->hideColumn(3);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnSize", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(4);
    }
    else {
        ui->filesTab_tableView->hideColumn(4);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnInsertion", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(5);
    }
    else {
        ui->filesTab_tableView->hideColumn(5);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnLastModification", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(6);
    }
    else {
        ui->filesTab_tableView->hideColumn(6);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnLastStatusChange", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(7);
    }
    else {
        ui->filesTab_tableView->hideColumn(7);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnLastAccess", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(8);
    }
    else {
        ui->filesTab_tableView->hideColumn(8);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnType", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(9);
    }
    else {
        ui->filesTab_tableView->hideColumn(9);
    }

    if (WiiBaFuSettings.value("FilesGameList/columnSource", QVariant(true)).toBool()) {
        ui->filesTab_tableView->showColumn(10);
    }
    else {
        ui->filesTab_tableView->hideColumn(10);
    }
}

void WiiBaFu::setWBFSColumns() {
    if (WiiBaFuSettings.value("WBFSGameList/columnID", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(0);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(0);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnName", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(1);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(1);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnTitle", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(2);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(2);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnRegion", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(3);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(3);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnSize", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(4);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(4);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnUsedBlocks", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(5);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(5);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnInsertion", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(6);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(6);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnLastModification", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(7);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(7);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnLastStatusChange", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(8);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(8);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnLastAccess", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(9);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(9);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnType", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(10);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(10);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnWBFSSlot", QVariant(true)).toBool()) {
        ui->wbfsTab_tableView->showColumn(11);
    }
    else {
        ui->wbfsTab_tableView->hideColumn(11);
    }

    if (WiiBaFuSettings.value("WBFSGameList/columnSource", QVariant(true)).toBool()) {
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
