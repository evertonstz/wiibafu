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

#include "global.h"
#include "witools.h"
#include "common.h"
#include "settings.h"
#include "wiibafudialog.h"
#include "coverviewdialog.h"
#include "wbfsdialog.h"

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
    Settings *settings;
    WiiBaFuDialog *wiibafudialog;
    CoverViewDialog *coverViewDialog;
    WBFSDialog *wbfsDialog;

    QTimer *timer;
    QStringList filesGamePaths;
    QProgressBar *progressBar_Main;
    QStandardItemModel *filesListModel;
    QStandardItemModel *dvdListModel;
    QStandardItemModel *wbfsListModel;

    void setupConnections();
    void setupMainProgressBar();
    void setupGeometry();
    void setGameListAttributes(QTableView *gameTableView);
    void setGameInfo(QTableView *tableView, QStandardItemModel *model);
    void setToolTips(QTableView *tableView, QStandardItemModel *model, QString firstColumnName, QString secondColumnName);

    QString getCurrentCoverLanguage();
    QString wbfsPath();
    int headerIndex(QAbstractItemModel *model, QString text, Qt::Orientation orientation);

    void saveMainWindowGeometry();
    void saveGameListHeaderStates();
    void setFilesColumns();
    void setWBFSColumns();

signals:
    void cancelTransfer();
    void startBusy();
    void stopBusy();

private slots:
    void on_menuOptions_Settings_triggered();
    void on_menuTools_CheckWBFS_triggered();
    void on_menuTools_CreateWBFS_triggered();
    void on_menuTools_UpdateTitles_triggered();

    void on_filesTab_pushButton_Load_clicked();
    void on_filesTab_pushButton_SelectAll_clicked();
    void on_filesTab_pushButton_TransferToWBFS_clicked();
    void on_filesTab_pushButton_TransferToImage_clicked();
    void on_filesTab_pushButton_ShowInfo_clicked();

    void on_dvdTab_pushButton_Load_clicked();
    void on_dvdTab_pushButton_TransferToWBFS_clicked();
    void on_dvdTab_pushButton_TransferToImage_clicked();

    void on_wbfsTab_pushButton_Load_clicked();
    void on_wbfsTab_pushButton_SelectAll_clicked();
    void on_wbfsTab_pushButton_Transfer_clicked();
    void on_wbfsTab_pushButton_Remove_clicked();
    void on_wbfsTab_pushButton_Check_clicked();
    void on_wbfsTab_pushButton_ShowInfo_clicked();

    void on_infoTab_pushButton_Load3DCover_clicked();
    void on_infoTab_pushButton_LoadFullHQCover_clicked();
    void on_infoTab_pushButton_Reset_clicked();

    void on_logTab_pushButton_Clear_clicked();
    void on_logTab_pushButton_Copy_clicked();
    void on_logTab_pushButton_Save_clicked();

    void setFilesGameListModel();
    void setDVDGameListModel();
    void setWBFSGameListModel();

    void showGameDiscCover(QImage *gameCover);
    void showGame3DCover(QImage *gameCover);
    void showGameFullHQCover(QImage *gameFullHQCover);

    void transferGamesToWBFSsuccesfully();
    void transferGamesToWBFScanceled(bool discExitst);

    void transferGameFromDVDToWBFSsuccesfully();
    void transferGameFromDVDToWBFScanceled(bool discExitst);
    void transferGameFromDVDToImageSuccesfully();
    void transferGameFromDVDToImageCanceled(bool discExitst);

    void transferGamesToImageSuccesfully();
    void transferGamesToImageCanceled();

    void setWBFSInfoText(QString text);
    void setWBFSProgressBar(int min, int max, int value, QString format);

    void setMainProgressBarVisible(bool visible);
    void setMainProgressBar(int value, QString format);
    void showMainProgressBarBusy();
    void startMainProgressBarBusy();
    void stopMainProgressBarBusy();

    void setStatusBarText(QString text);
    void addEntryToLog(QString entry, WiTools::LogType type);

    void on_menuHelp_About_triggered();
};

#endif // WIIBAFU_H
