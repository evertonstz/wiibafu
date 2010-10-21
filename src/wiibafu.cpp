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
#include "witools.h"

WiiBaFu::WiiBaFu(QWidget *parent) : QMainWindow(parent), ui(new Ui::WiiBaFu) {
    ui->setupUi(this);
    this->setWindowTitle("Wii Backup Fusion " + QCoreApplication::applicationVersion());
    this->setupConnections();

    setGameListAttributes(ui->tableView_Files);
}

void WiiBaFu::setupConnections() {
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->pushButton_Files_Add, SIGNAL(clicked()), this, SLOT(files_Add()));
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

void WiiBaFu::files_Add() {
    QString directory;
    QFileDialog *dialog = new QFileDialog(this);
    directory = dialog->getExistingDirectory(this, tr("Open directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);

    if (directory != "") {
        WiTools *wiTools = new WiTools(this);
        ui->tableView_Files->setModel(wiTools->getFilesModel(filesModel, directory));
    }
}

void WiiBaFu::showAboutBox() {
    QMessageBox::about(this, tr("About Wii Backup Fusion"),
        QString("<h2>Wii Backup Fusion %1</h2>").arg(QCoreApplication::applicationVersion()) +
        tr("<p><b><i>Wii backup solution</b></i>"
        "<p>Copyright &copy; 2010 dynup"
        "<p>Mail: <a href='mailto:DynUp<dynup@ymail.com>?subject=WiiBaFu%20feedback'>dynup@ymail.com</a>"
        "<br>Web: <a href='http://wiibafu.codeplex.com'>wiibafu.codeplex.com</a>"
        "<p>Big thanks to the trolls for the excellent Qt toolkit! ;)"));
}

WiiBaFu::~WiiBaFu() {
    delete ui;
}
