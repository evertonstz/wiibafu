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
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(wiTools, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));
    connect(wiTools, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(common, SIGNAL(newGameCover(QImage*)), this, SLOT(showGameCover(QImage*)));

    connect(ui->pushButton_Files_Add, SIGNAL(clicked()), this, SLOT(filesGameList_Add()));
    connect(ui->pushButton_Files_SelectAll, SIGNAL(clicked()), this, SLOT(filesGameList_SelectAll()));
    connect(ui->pushButton_Files_ShowInfo, SIGNAL(clicked()), this, SLOT(filesGameList_ShowInfo()));

    connect(ui->pushButton_HDD_List, SIGNAL(clicked()), this, SLOT(hddGameList_List()));
    connect(ui->pushButton_HDD_SelectAll, SIGNAL(clicked()), this, SLOT(hddGameList_SelectAll()));
    connect(ui->pushButton_HDD_ShowInfo, SIGNAL(clicked()), this, SLOT(hddGameList_ShowInfo()));

    connect(ui->pushButton_Info_GetCover, SIGNAL(clicked()), this, SLOT(infoGame_GetCover()));

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

        QHeaderView *headerView = gameTableView->horizontalHeader();
        QHeaderView::ResizeMode resizeMode = QHeaderView::ResizeToContents;
        headerView->setResizeMode(resizeMode);
}

void WiiBaFu::filesGameList_Add() {
    QString directory;
    QFileDialog *dialog = new QFileDialog(this);

    directory = dialog->getExistingDirectory(this, tr("Open directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        filesListModel->clear();
        ui->tableView_Files->setModel(wiTools->getFilesGameListModel(filesListModel, directory));
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
    ui->tableView_HDD->setModel(wiTools->getHDDGameListModel(hddListModel));
}

void WiiBaFu::hddGameList_SelectAll() {
    ui->tableView_HDD->selectAll();
}

void WiiBaFu::hddGameList_ShowInfo() {
    setGameInfo(ui->tableView_HDD, hddListModel);
}

void WiiBaFu::setGameInfo(QTableView *tableView, QStandardItemModel *model) {
    if (tableView->selectionModel() && !tableView->selectionModel()->selectedRows(0).isEmpty()) {
        ui->lineEdit_info_ID->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());
        ui->lineEdit_info_Name->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(1).first())->text());
        ui->lineEdit_info_Title->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(2).first())->text());
        ui->lineEdit_info_Region->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(3).first())->text());
        ui->lineEdit_info_Size->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(4).first())->text());

        if (tableView == ui->tableView_HDD) {
            ui->lineEdit_info_Date->setText("--");
            ui->infolabel_Info_FileName->setText(tr("Partition:"));
            ui->lineEdit_info_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
            ui->lineEdit_info_FileName->setToolTip("");
            ui->lineEdit_info_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
        }
        else {
            ui->lineEdit_info_Date->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(5).first())->text());
            ui->infolabel_Info_FileName->setText(tr("File name:"));
            ui->lineEdit_info_FileName->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->lineEdit_info_FileName->setToolTip(model->itemFromIndex(tableView->selectionModel()->selectedRows(7).first())->text());
            ui->lineEdit_info_Type->setText(model->itemFromIndex(tableView->selectionModel()->selectedRows(6).first())->text());
        }

        common->getGameCover(model->itemFromIndex(tableView->selectionModel()->selectedRows(0).first())->text());

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


void WiiBaFu::infoGame_GetCover() {
    if (!ui->lineEdit_info_ID->text().isEmpty())
        common->getGameCover(ui->lineEdit_info_ID->text());
}

void WiiBaFu::showGameCover(QImage *gameCover) {
    if (!gameCover->isNull())
        ui->label_GameCover->setPixmap(QPixmap::fromImage(*gameCover, Qt::AutoColor));
    else
        ui->label_GameCover->setText(tr("No cover available\n\nfor this game!"));
}

void WiiBaFu::setStatusBarText(QString text) {
    ui->statusBar->showMessage(text);
}

void WiiBaFu::addEntryToLog(QString entry) {
    ui->plainTextEdit_Log->appendPlainText(entry);
}

void WiiBaFu::showAboutBox() {
    QMessageBox::about(this, tr("About Wii Backup Fusion"),
        QString("<h2>Wii Backup Fusion %1</h2>").arg(QCoreApplication::applicationVersion()) +
        tr("<p><b><i>The complete and simply to use backup solution for Wii games</b></i>"
        "<p>Copyright &copy; 2010 Kai Heitkamp"
        "<p>Mail: <a href='mailto:dynup<dynup@ymail.com>?subject=WiiBaFu%20feedback'>dynup@ymail.com</a>"
        "<br>Web: <a href='http://wiibafu.codeplex.com'>wiibafu.codeplex.com</a>"
        "<p>Big thanks to the trolls at Trolltech Norway for his excellent Qt toolkit"
        "<br>and the guys at Nokia for the continuation, thanks to Dirk Clemens (Wiimm) for his great ISO tools!"
        "<p><i>Dedicated to my father G&uuml;nter Heitkamp (28.07.1935 - 06.10.2009)</i>"));
}

WiiBaFu::~WiiBaFu() {
    delete ui;
}
