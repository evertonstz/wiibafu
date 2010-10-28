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
    hddListModel = new QStandardItemModel(this);

    ui->setupUi(this);
    setWindowTitle("Wii Backup Fusion " + QCoreApplication::applicationVersion());
    setStatusBarText(tr("Ready."));
    setupConnections();

    setGameListAttributes(ui->tableView_Files);
    setGameListAttributes(ui->tableView_HDD);
}

void WiiBaFu::setupConnections() {
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QImage>("QImage");

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(wiTools, SIGNAL(newFilesLabelTotalDiscs(QString)), this, SLOT(setFilesLabelTotalDiscs(QString)));
    connect(wiTools, SIGNAL(newFilesLabelTotalSize(QString)), this, SLOT(setFilesLabelTotalSize(QString)));
    connect(wiTools, SIGNAL(setProgressBarHDD(int, int, int, QString)), this, SLOT(setHDDProgressBar(int, int, int, QString)));
    connect(wiTools, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(wiTools, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));

    connect(common, SIGNAL(newGame3DCover(QImage*)), this, SLOT(showGame3DCover(QImage*)));
    connect(common, SIGNAL(newGameFullHQCover(QImage*)), this, SLOT(showGameFullHQCover(QImage*)));
    connect(common, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(common, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));

    connect(ui->pushButton_Files_Add, SIGNAL(clicked()), this, SLOT(filesGameList_Add()));
    connect(ui->pushButton_Files_SelectAll, SIGNAL(clicked()), this, SLOT(filesGameList_SelectAll()));
    connect(ui->pushButton_Files_ShowInfo, SIGNAL(clicked()), this, SLOT(filesGameList_ShowInfo()));

    connect(ui->pushButton_HDD_List, SIGNAL(clicked()), this, SLOT(hddGameList_List()));
    connect(ui->pushButton_HDD_SelectAll, SIGNAL(clicked()), this, SLOT(hddGameList_SelectAll()));
    connect(ui->pushButton_HDD_ShowInfo, SIGNAL(clicked()), this, SLOT(hddGameList_ShowInfo()));

    connect(ui->pushButton_Info_Load3DCover, SIGNAL(clicked()), this, SLOT(infoGame_Load3DCover()));
    connect(ui->pushButton_Info_LoadFullHQCover, SIGNAL(clicked()), this, SLOT(infoGame_LoadFullHQCover()));

    connect(ui->pushButton_Log_Clear, SIGNAL(clicked()), this, SLOT(log_Clear()));
    connect(ui->pushButton_Log_Copy, SIGNAL(clicked()), this, SLOT(log_Copy()));
    connect(ui->pushButton_Log_Save, SIGNAL(clicked()), this, SLOT(log_Save()));
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

void WiiBaFu::filesGameList_Add() {
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        filesListModel->clear();

        QFuture<QStandardItemModel*> future = QtConcurrent::run(wiTools, &WiTools::getFilesGameListModel, filesListModel, directory);
        ui->tableView_Files->setModel(future.result());
    }
}

void WiiBaFu::filesGameList_SelectAll() {
    ui->tableView_Files->selectAll();
}

void WiiBaFu::filesGameList_ShowInfo() {
    setGameInfo(ui->tableView_Files, filesListModel);
}

void WiiBaFu::hddGameList_List() {
    hddListModel->clear();

    QFuture<QStandardItemModel*> future = QtConcurrent::run(wiTools, &WiTools::getHDDGameListModel, hddListModel);
    ui->tableView_HDD->setModel(future.result());
}

void WiiBaFu::hddGameList_SelectAll() {
    ui->tableView_HDD->selectAll();
}

void WiiBaFu::hddGameList_ShowInfo() {
    setGameInfo(ui->tableView_HDD, hddListModel);
}

void WiiBaFu::setGameInfo(QTableView *tableView, QStandardItemModel *model) {
    if (tableView->selectionModel() && !tableView->selectionModel()->selectedRows(0).isEmpty()) {
        if (tableView == ui->tableView_HDD) {
            ui->lineEdit_info_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
            ui->lineEdit_info_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
            ui->lineEdit_info_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
            ui->lineEdit_info_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
            ui->lineEdit_info_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());
            ui->lineEdit_info_UsedBlocks->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            ui->lineEdit_info_Insertion->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
            ui->lineEdit_info_LastModification->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->lineEdit_info_LastStatusChange->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->text());
            ui->lineEdit_info_LastAccess->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->text());
            ui->lineEdit_info_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
            ui->lineEdit_info_Container->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(11).first())->text());
            ui->lineEdit_info_WBFSSlot->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(12).first())->text());
            ui->lineEdit_info_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(13).first())->text());
        }
        else {
            ui->lineEdit_info_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
            ui->lineEdit_info_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
            ui->lineEdit_info_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
            ui->lineEdit_info_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
            ui->lineEdit_info_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());
            ui->lineEdit_info_UsedBlocks->setText("--");
            ui->lineEdit_info_Insertion->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            ui->lineEdit_info_LastModification->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
            ui->lineEdit_info_LastStatusChange->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->lineEdit_info_LastAccess->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(8).first())->text());
            ui->lineEdit_info_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(9).first())->text());
            ui->lineEdit_info_Container->setText("--");
            ui->lineEdit_info_WBFSSlot->setText("--");
            ui->lineEdit_info_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(10).first())->text());
        }

        infoGame_Load3DCover();
        ui->tabWidget->setCurrentIndex(3);
    }
}

void WiiBaFu::log_Clear() {
    ui->plainTextEdit_Log->clear();
}

void WiiBaFu::log_Copy() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->plainTextEdit_Log->toPlainText());
}

void WiiBaFu::log_Save() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save log file"), QDir::homePath().append("/WiiBaFu.log"), tr("WiiBaFu log file (*.log)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream textStream(&file);
            textStream << ui->plainTextEdit_Log->toPlainText();
        }

        file.close();
    }
}

void WiiBaFu::infoGame_Load3DCover() {
    if (!ui->lineEdit_info_ID->text().isEmpty())
        common->getGame3DCover(ui->lineEdit_info_ID->text(), getCurrentCoverLanguage());
}

void WiiBaFu::infoGame_LoadFullHQCover() {
    if (!ui->lineEdit_info_ID->text().isEmpty())
        common->getGameFullHQCover(ui->lineEdit_info_ID->text(), getCurrentCoverLanguage());
}

void WiiBaFu::showGame3DCover(QImage *gameCover) {
    ui->label_GameCover->setPixmap(QPixmap::fromImage(*gameCover, Qt::AutoColor));
}

void WiiBaFu::showGameFullHQCover(QImage *gameFullHQCover) {
    gameFullHQCover->save(QDir::tempPath().append("/WiiBaFu_gameCoverFullHQ.png"));
    QDesktopServices::openUrl(QDir::tempPath().append("/WiiBaFu_gameCoverFullHQ.png"));
}

void WiiBaFu::setFilesLabelTotalDiscs(QString totalDiscs) {
    ui->label_Files_TotalDiscs->setText(totalDiscs);
}

void WiiBaFu::setFilesLabelTotalSize(QString totalSize) {
    ui->label_Files_TotalSize->setText(totalSize);
}

void WiiBaFu::setHDDProgressBar(int min, int max, int value, QString text) {
    ui->progressBar_HDD->setMinimum(min);
    ui->progressBar_HDD->setMaximum(max);
    ui->progressBar_HDD->setValue(value);
    ui->progressBar_HDD->setFormat(text);
}

void WiiBaFu::setStatusBarText(QString text) {
    ui->statusBar->showMessage(text);
}

void WiiBaFu::addEntryToLog(QString entry) {
    ui->plainTextEdit_Log->appendPlainText(entry);
}

QString WiiBaFu::getCurrentCoverLanguage() {
    switch (ui->comboBox_CoverLanguages->currentIndex()) {
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
