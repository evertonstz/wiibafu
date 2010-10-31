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

    filesListModel = new QStandardItemModel(this);
    wbfsListModel = new QStandardItemModel(this);

    ui->setupUi(this);
    setWindowTitle("Wii Backup Fusion " + QCoreApplication::applicationVersion());
    setStatusBarText(tr("Ready."));
    setupMainProgressBar();
    setupConnections();

    setGameListAttributes(ui->filesTab_tableView);
    setGameListAttributes(ui->wbfsTab_tableView);
}

void WiiBaFu::setupConnections() {
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");

    connect(this, SIGNAL(cancelTransferGamesToWBFS()), wiTools, SLOT(transferGamesToWBFS_cancel()));

    connect(wiTools, SIGNAL(setMainProgressBar(int, QString)), this, SLOT(setMainProgressBar(int,QString)));
    connect(wiTools, SIGNAL(setMainProgressBarVisible(bool)), this, SLOT(setMainProgressBarVisible(bool)));
    connect(wiTools, SIGNAL(setProgressBarWBFS(int, int, int, QString)), this, SLOT(setWBFSProgressBar(int, int, int, QString)));
    connect(wiTools, SIGNAL(newStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(wiTools, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));
    connect(wiTools, SIGNAL(transferToWBFSsuccessfully()), this, SLOT(transferToWBFSsuccesfully()));
    connect(wiTools, SIGNAL(transferToWBFScanceled(bool)), this, SLOT(transferToWBFScanceled(bool)));
    connect(wiTools, SIGNAL(removeGamesFromWBFS_successfully()), this, SLOT(on_wbfsTab_pushButton_List_clicked()));

    connect(common, SIGNAL(newGame3DCover(QImage*)), this, SLOT(showGame3DCover(QImage*)));
    connect(common, SIGNAL(newGameFullHQCover(QImage*)), this, SLOT(showGameFullHQCover(QImage*)));
    connect(common, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(common, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
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
}

void WiiBaFu::setMainProgressBarVisible(bool visible) {
    progressBar_Main->setVisible(visible);
}

void WiiBaFu::setGameListAttributes(QTableView *gameTableView) {
        gameTableView->setShowGrid(false);
        gameTableView->setAlternatingRowColors(true);
        gameTableView->verticalHeader()->hide();
        gameTableView->verticalHeader()->setDefaultSectionSize(20);
        gameTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        gameTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        gameTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        gameTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        QHeaderView *headerView = gameTableView->horizontalHeader();
        QHeaderView::ResizeMode resizeMode = QHeaderView::ResizeToContents;
        headerView->setResizeMode(resizeMode);
}

void WiiBaFu::on_filesTab_pushButton_Add_clicked() {
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        filesListModel->clear();

        QFuture<QStandardItemModel*> future = QtConcurrent::run(wiTools, &WiTools::getFilesGameListModel, filesListModel, directory);
        ui->filesTab_tableView->setModel(future.result());
        ui->tabWidget->setTabText(0, QString("Files (%1)").arg(ui->filesTab_tableView->model()->rowCount()));
    }
}

void WiiBaFu::on_filesTab_pushButton_SelectAll_clicked() {
    ui->filesTab_tableView->selectAll();
}

void WiiBaFu::on_filesTab_pushButton_TransferToWBFS_clicked() {
    if (ui->filesTab_pushButton_TransferToWBFS->text() != tr("Cancel transfering")) {
        if (ui->filesTab_tableView->selectionModel() && !ui->filesTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
            ui->filesTab_pushButton_TransferToWBFS->setText(tr("Cancel transfering"));
            QtConcurrent::run(wiTools, &WiTools::transferToWBFS, ui->filesTab_tableView->selectionModel()->selectedRows(10), QString("/home/kai/wii.wbfs")); //TODO: User sets the wbfs path!
        }
    }
    else {
        emit cancelTransferGamesToWBFS();
    }
}

void WiiBaFu::on_filesTab_pushButton_ShowInfo_clicked() {
    setGameInfo(ui->filesTab_tableView, filesListModel);
}

void WiiBaFu::on_wbfsTab_pushButton_List_clicked() {
    setStatusBarText(tr("Loading games..."));

    QFuture<QStandardItemModel*> future = QtConcurrent::run(wiTools, &WiTools::getWBFSGameListModel, wbfsListModel);

    wbfsListModel->clear();
    ui->wbfsTab_tableView->setModel(future.result());
    ui->tabWidget->setTabText(2, QString("WBFS (%1)").arg(ui->wbfsTab_tableView->model()->rowCount()));

    setStatusBarText(tr("Ready."));
}

void WiiBaFu::on_wbfsTab_pushButton_SelectAll_clicked() {
    ui->wbfsTab_tableView->selectAll();
}

void WiiBaFu::on_wbfsTab_pushButton_Remove_clicked() {
    if (!ui->wbfsTab_tableView->selectionModel()->selectedRows(0).isEmpty()) {
        int result = QMessageBox::question(this, tr("Remove games"), tr("Are you sure that you want to delete the selected games?"), QMessageBox::Ok, QMessageBox::Cancel);
        if (result == QMessageBox::Ok) {
            QtConcurrent::run(wiTools, &WiTools::removeGamesFromWBFS, ui->wbfsTab_tableView->selectionModel()->selectedRows(0), QString("/home/kai/wii.wbfs")); //TODO: User sets the wbfs path!
        }
    }
}

void WiiBaFu::on_wbfsTab_pushButton_ShowInfo_clicked() {
    setGameInfo(ui->wbfsTab_tableView, wbfsListModel);
}

void WiiBaFu::setGameInfo(QTableView *tableView, QStandardItemModel *model) {
    if (tableView->selectionModel() && !tableView->selectionModel()->selectedRows(0).isEmpty()) {
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
            ui->infoTab_lineEdit_Container->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(11).first())->text());
            ui->infoTab_lineEdit_WBFSSlot->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(12).first())->text());
            ui->infoTab_lineEdit_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(13).first())->text());
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
            ui->infoTab_lineEdit_Container->setText("--");
            ui->infoTab_lineEdit_WBFSSlot->setText("--");
            ui->infoTab_lineEdit_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
        }

        on_infoTab_pushButton_Load3DCover_clicked();
        ui->tabWidget->setCurrentIndex(3);
    }
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

void WiiBaFu::on_infoTab_pushButton_Load3DCover_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty())
        common->getGame3DCover(ui->infoTab_lineEdit_ID->text(), getCurrentCoverLanguage());
}

void WiiBaFu::on_infoTab_pushButton_LoadFullHQCover_clicked() {
    if (!ui->infoTab_lineEdit_ID->text().isEmpty())
        common->getGameFullHQCover(ui->infoTab_lineEdit_ID->text(), getCurrentCoverLanguage());
}

void WiiBaFu::showGame3DCover(QImage *gameCover) {
    ui->label_GameCover->setPixmap(QPixmap::fromImage(*gameCover, Qt::AutoColor));
}

void WiiBaFu::showGameFullHQCover(QImage *gameFullHQCover) {
    gameFullHQCover->save(QDir::tempPath().append("/WiiBaFu_gameCoverFullHQ.png"));
    QDesktopServices::openUrl(QDir::tempPath().append("/WiiBaFu_gameCoverFullHQ.png"));
}

void WiiBaFu::transferToWBFSsuccesfully() {
    ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
    on_wbfsTab_pushButton_List_clicked();
}

void WiiBaFu::transferToWBFScanceled(bool discExitst) {
    if (discExitst) {
        ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Disc already exists!"));
        setStatusBarText(tr("Disc already exists!"));
    }
    else {
        ui->filesTab_pushButton_TransferToWBFS->setText(tr("Transfer to WBFS"));
        addEntryToLog(tr("Transfer canceled!"));
        setStatusBarText(tr("Transfer canceled!"));
    }
}

void WiiBaFu::setMainProgressBar(int value, QString format) {
    progressBar_Main->setValue(value);
    progressBar_Main->setFormat(format);
}

void WiiBaFu::setWBFSProgressBar(int min, int max, int value, QString format) {
    ui->wbfsTab_progressBar->setMinimum(min);
    ui->wbfsTab_progressBar->setMaximum(max);
    ui->wbfsTab_progressBar->setValue(value);
    ui->wbfsTab_progressBar->setFormat(format);
}

void WiiBaFu::setStatusBarText(QString text) {
    ui->statusBar->showMessage(text);
}

void WiiBaFu::addEntryToLog(QString entry) {
    ui->logTab_plainTextEdit_Log->appendPlainText(entry);
}

QString WiiBaFu::getCurrentCoverLanguage() {
    switch (ui->infoTab_comboBox_CoverLanguages->currentIndex()) {
        case 0:  return "EN";
                 break;
        case 1:  return "FR";
                 break;
        case 2:  return "DE";
                 break;
        case 3:  return "ES";
                 break;
        case 4:  return "IT";
                 break;
        case 5:  return "NL";
                 break;
        case 6:  return "PT";
                 break;
        case 7:  return "SE";
                 break;
        case 8:  return "DK";
                 break;
        case 9:  return "NO";
                 break;
        case 10: return "FI";
                 break;
        case 11: return "RU";
                 break;
        case 12: return "JA";
                 break;
        case 13: return "KO";
                 break;
        case 14: return "ZH-tw";
                 break;
        case 15: return "ZH-cn";
                 break;
        default: return "EN";
    }
}

void WiiBaFu::showAboutBox() {
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
        "<p><i>Dedicated to my father G&uuml;nter Heitkamp (28.07.1935 - 06.10.2009)</i>"));
}

WiiBaFu::~WiiBaFu() {
    delete ui;
}
