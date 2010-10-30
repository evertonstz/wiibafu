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

#ifndef WIIBAFU_H
#define WIIBAFU_H

#include <QtGui>
#include <QMetaType>

#include"witools.h"
#include "common.h"

namespace Ui {
    class WiiBaFu;
}

class WiiBaFu : public QMainWindow {
    Q_OBJECT

public:
    explicit WiiBaFu(QWidget *parent = 0);
    ~WiiBaFu();

private:
    Ui::WiiBaFu *ui;
    WiTools *wiTools;
    Common *common;

    QStringList filesGamePaths;
    QProgressBar *progressBar_Main;
    QStandardItemModel *filesListModel;
    QStandardItemModel *wbfsListModel;

    void setupConnections();
    void setupMainProgressBar();
    void setGameListAttributes(QTableView *gameTableView);
    void setGameInfo(QTableView *tableView, QStandardItemModel *model);

private slots:
    void setMainProgressBarVisible(bool visible);

    void filesGameList_Add();
    void filesGameList_SelectAll();
    void filesGameList_TransferToWBFS();
    void filesGameList_ShowInfo();

    void wbfsGameList_List();
    void wbfsGameList_SelectAll();
    void wbfsGameList_ShowInfo();

    void infoGame_Load3DCover();
    void infoGame_LoadFullHQCover();

    void log_Clear();
    void log_Copy();
    void log_Save();

    void showGame3DCover(QImage *gameCover);
    void showGameFullHQCover(QImage *gameFullHQCover);

    QString getCurrentCoverLanguage();

    void setWBFSProgressBar(int min, int max, int value, QString format);
    void setMainProgressBar(int value, QString format);
    void setStatusBarText(QString text);
    void addEntryToLog(QString entry);
    void showAboutBox();
};

#endif // WIIBAFU_H
