/***************************************************************************
 *   Copyright (C) 2010-2011 Kai Heitkamp                                  *
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
    QFile logFile;
    bool logFindFirstTime;

    QTimer *timer;
    QStringList filesGamePaths;
    QProgressBar *progressBar_Main;
    QStandardItemModel *filesListModel;
    QStandardItemModel *dvdListModel;
    QStandardItemModel *wbfsListModel;

    QAction *action_filesGame_TransferToWBFS_Patch;
    QAction *action_filesGame_TransferToImage_Patch;
    QAction *action_filesGame_ExtractImage_Patch;
    QAction *action_filesGame_Patch;

    QAction *action_wbfsGame_Transfer_Patch;
    QAction *action_wbfsGame_Extract_Patch;

    void setupConnections();
    void setupContextMenus();
    void setupMainProgressBar();
    void setupGeometry();
    void setMacOSXStyle();

    void setGameListAttributes(QTableView *gameTableView);
    void setGameInfo(QTableView *tableView, QStandardItemModel *model);
    void setGameInfoDateTimes(QTableView *tableView, QStandardItemModel *model);
    void setToolTips(QTableView *tableView, QStandardItemModel *model, const QString firstColumnName, const QString secondColumnName);
    void updateTitles();

    QString buildPath(const QString directory, QStandardItemModel *model, QTableView *tableView);
    QString currentGameLanguage();
    QString wbfsPath();
    QLocale locale();
    int headerIndex(QAbstractItemModel *model, const QString text, const Qt::Orientation orientation);
    bool gameInList(QStandardItemModel *model, const QString gameId);

    void saveMainWindowGeometry();
    void saveGameListHeaderStates();
    void setFilesColumns();
    void setWBFSColumns();

    bool event(QEvent *event);

signals:
    void cancelTransfer();
    void cancelLoading();
    void cancelVerifying();
    void startBusy();
    void stopBusy();

private slots:
    void on_menuOptions_Settings_triggered();
    void on_menuTools_CheckWBFS_triggered();
    void on_menuTools_DumpWBFS_triggered();
    void on_menuTools_CreateWBFS_triggered();
    void on_menuTools_VerifyGame_triggered();
    void on_menuTools_Compare_triggered();
    void on_menuTools_UpdateTitles_triggered();

    void on_filesTab_tableView_doubleClicked(QModelIndex);
    void on_wbfsTab_tableView_doubleClicked(QModelIndex);

    void on_filesTab_pushButton_Load_clicked();
    void on_filesTab_pushButton_SelectAll_clicked();
    void on_filesTab_pushButton_TransferToWBFS_clicked();
    void on_filesTab_pushButton_TransferToImage_clicked();
    void on_filesTab_pushButton_ExtractImage_clicked();
    void on_filesTab_pushButton_ShowInfo_clicked();

    void on_dvdTab_pushButton_Load_clicked();
    void on_dvdTab_pushButton_TransferToWBFS_clicked();
    void on_dvdTab_pushButton_TransferToImage_clicked();
    void on_dvdTab_pushButton_Extract_clicked();

    void on_wbfsTab_pushButton_Load_clicked();
    void on_wbfsTab_pushButton_SelectAll_clicked();
    void on_wbfsTab_pushButton_Transfer_clicked();
    void on_wbfsTab_pushButton_Extract_clicked();
    void on_wbfsTab_pushButton_Remove_clicked();
    void on_wbfsTab_pushButton_Check_clicked();
    void on_wbfsTab_pushButton_ShowInfo_clicked();

    void on_infoTab_pushButton_Load3DCover_clicked();
    void on_infoTab_pushButton_LoadFullHQCover_clicked();
    void on_infoTab_pushButton_viewInBrowser_clicked();
    void on_infoTab_pushButton_Reset_clicked();

    void on_logTab_pushButton_Clear_clicked();
    void on_logTab_pushButton_Copy_clicked();
    void on_logTab_pushButton_Find_clicked();
    void on_logTab_pushButton_Save_clicked();

    void filesTab_ContextMenu_TransferToWBFS_Patch();
    void filesTab_ContextMenu_TransferToImage_Patch();
    void filesTab_ContextMenu_ExtractImage_Path();

    void filesTab_TransferToWBFS(const bool patch);
    void filesTab_TransferToImage(const bool patch);
    void filesTab_ExtractImage(const bool patch);

    void filesTableView_selectionChanged(const QItemSelection selected, const QItemSelection deselected);
    void wbfsTableView_selectionChanged(const QItemSelection selected, const QItemSelection deselected);

    void setFilesGameListModel();
    void loadingGamesCanceled();
    void loadingGamesFailed(WiTools::WitStatus);
    void setDVDGameListModel();
    void setWBFSGameListModel();

    void transferFilesToWBFS_finished(WiTools::WitStatus status);
    void transferFilesToImage_finished(WiTools::WitStatus);
    void extractImage_finished(WiTools::WitStatus);

    void transferDVDToWBFS_finished(WiTools::WitStatus status);
    void transferDVDToImage_finished(WiTools::WitStatus);
    void extractDVD_finished(WiTools::WitStatus);

    void transferWBFSToImage_finished(WiTools::WitStatus);
    void extractWBFS_finished(WiTools::WitStatus);

    void verifyGame_finished(WiTools::WitStatus);

    void filesGame_Patch();
    void filesGame_Patch_finished(WiTools::WitStatus status);

    void showGameDiscCover(const QImage gameCover);
    void showGame3DCover(const QImage gameCover);
    void showGameFullHQCover(const QImage gameFullHQCover);

    void setWBFSInfoText(const QString text);
    void setWBFSProgressBar(const int min, const int max, const int value, const QString format);

    void setMainProgressBarVisible(const bool visible);
    void setMainProgressBar(const int value, const QString format);
    void showMainProgressBarBusy();
    void startMainProgressBarBusy();
    void stopMainProgressBarBusy();

    void setStatusBarText(const QString text);
    void addEntryToLog(const QString entry, const WiTools::LogType type);
    void addEntriesToLog(const QStringList entries, const WiTools::LogType type);
    void addWitCommandLineToLog(const QString wit, const QStringList arguments);

    void on_menuHelp_About_triggered();
};

#endif // WIIBAFU_H
