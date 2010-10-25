/***************************************************************************
 *   Copyright (C) 2010 DynUp                                              *
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
    filesModel = new QStandardItemModel(this);

    ui->setupUi(this);
    setWindowTitle("Wii Backup Fusion " + QCoreApplication::applicationVersion());
    setStatusBarText(tr("Ready."));
    setupConnections();

    setGameListAttributes(ui->tableView_Files);
}

void WiiBaFu::setupConnections() {
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(wiTools, SIGNAL(newLogEntry(QString)), this, SLOT(addEntryToLog(QString)));
    connect(wiTools, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));
    connect(common, SIGNAL(newGameCover(QImage*)), this, SLOT(showGameCover(QImage*)));
    connect(common, SIGNAL(showStatusBarMessage(QString)), this, SLOT(setStatusBarText(QString)));

    connect(ui->pushButton_Files_Add, SIGNAL(clicked()), this, SLOT(filesGameList_Add()));
    connect(ui->pushButton_Files_Select, SIGNAL(clicked()), this, SLOT(filesGameList_SelectAll()));
    connect(ui->pushButton_Files_ShowInfo, SIGNAL(clicked()), this, SLOT(filesGameList_ShowInfo()));

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
        filesModel->clear();
        ui->tableView_Files->setModel(wiTools->getFilesGameListModel(filesModel, directory));
    }
}

void WiiBaFu::filesGameList_SelectAll() {
    ui->tableView_Files->selectAll();
}

void WiiBaFu::filesGameList_ShowInfo() {
    if (ui->tableView_Files->selectionModel() && !ui->tableView_Files->selectionModel()->selectedRows(0).isEmpty()) {
        WiiGame currentGame = getSelectedWiiGame();

        ui->lineEdit_info_ID->setText(currentGame.id);
        ui->lineEdit_info_Name->setText(currentGame.name);
        ui->lineEdit_info_Title->setText(currentGame.title);
        ui->lineEdit_info_Region->setText(currentGame.region);
        ui->lineEdit_info_Size->setText(currentGame.size);
        ui->lineEdit_info_Date->setText(currentGame.date);
        ui->lineEdit_info_Type->setText(currentGame.filetype);
        ui->lineEdit_info_FileName->setText(currentGame.filename);
        ui->lineEdit_info_FileName->setToolTip(currentGame.filename);

        common->getGameCover(filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(0).first())->text());

        ui->tabWidget->setCurrentIndex(3);
    }
}

WiiGame WiiBaFu::getSelectedWiiGame() {
    WiiGame currentGame;

    currentGame.id = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_id).first())->text();
    currentGame.name = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_name).first())->text();
    currentGame.title = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_title).first())->text();
    currentGame.region = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_region).first())->text();
    currentGame.size = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_size).first())->text();
    currentGame.date = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_date).first())->text();
    currentGame.filetype = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_filetype).first())->text();
    currentGame.filename = filesModel->itemFromIndex(ui->tableView_Files->selectionModel()->selectedRows(tablerow_filename).first())->text();

    return currentGame;
}

void WiiBaFu::infoGame_GetCover() {
    if (!ui->lineEdit_info_ID->text().isEmpty())
        common->getGameCover(ui->lineEdit_info_ID->text());
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
