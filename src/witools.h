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

#ifndef WITOOLS_H
#define WITOOLS_H

#include <QObject>
#include <QtGui>

#include "global.h"

class WiTools : public QObject {
    Q_OBJECT

public:
    enum WitStatus {
        Ok = 0x0,
        UnknownError = 0x01,
        TransferCanceled = 0x02,
        DiscAlreadyExists = 0x03,
        FileAlreadyExists = 0x04,
        DestinationAlreadyExists = 0x05,
        NoSuchFileOrDirectory = 0x06,
        NoGamesFound = 0x07
    };

    enum LogType {
        Info = 0x0,
        Error = 0x1
    };

    struct CreateWBFSParameters {
        QString Path;
        QString Size;
        QString SplitSize;
        QString HDSectorSize;
        QString WBFSSectorSize;
        bool Recover;
        bool Inode;
        bool Test;
    };

    explicit WiTools(QObject *parent = 0);

    void requestFilesGameListModel(QStandardItemModel *model, QString path);
    void requestDVDGameListModel(QStandardItemModel *model, QString path);
    void requestWBFSGameListModel(QStandardItemModel *model, QString wbfsPath);

    void transferFilesToWBFS(QModelIndexList indexList, QString wbfsPath);
    void transferFilesToImage(QModelIndexList indexList, QString format, QString compression, QString directory);
    void transferFilesToFileSystem(QModelIndexList indexList, QString destination);

    void transferDVDToWBFS(QString dvdPath, QString wbfsPath);
    void transferDVDToImage(QString dvdPath, QString format, QString compression, QString directory);
    void transferDVDToFileSystem(QString dvdPath, QString destination);

    void transferWBFSToImage(QModelIndexList indexList, QString wbfsPath, QString format, QString compression, QString directory);

    void removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath);
    void checkWBFS(QString wbfsPath);
    void createWBFS(CreateWBFSParameters parameters);

    QString witVersion();
    QString wwtVersion();
    QString witTitlesPath();

private:
    QString wit, wwt;
    QProcess *witProcess;
    WitStatus witProcessStatus;
    QStandardItemModel *witModel;

    double totalGameSize;

    #ifdef Q_OS_MACX
        QString gameCountText;
    #endif

    void setWit();

signals:
    void stopBusy();

    void setMainProgressBarVisible(bool visible);
    void setMainProgressBar(int value, QString format);
    void setInfoTextWBFS(QString text);
    void setProgressBarWBFS(int min, int max, int value, QString text);

    void newLogEntry(QString entry, WiTools::LogType type);
    void showStatusBarMessage(QString message);

    void newFilesGameListModel();
    void newDVDGameListModel();
    void newWBFSGameListModel();
    void loadingGamesCanceled();
    void loadingGamesFailed(WiTools::WitStatus);

    void transferFilesToWBFS_finished(WiTools::WitStatus);
    void transferFilesToImage_finished(WiTools::WitStatus);
    void transferFilesToFileSystem_finished(WiTools::WitStatus);

    void transferDVDToWBFS_finished(WiTools::WitStatus);
    void transferDVDToImage_finished(WiTools::WitStatus);
    void transferDVDToFileSystem_finished(WiTools::WitStatus);

    void transferWBFSToImage_finished(WiTools::WitStatus);

    void removeGamesFromWBFS_successfully();

public slots:
    void cancelTransfer();
    void cancelLoading();

private slots:
    void requestFilesGameListModel_readyReadStandardOutput();
    void requestFilesGameListModel_readyReadStandardError();
    void requestFilesGameListModel_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferFilesToWBFS_readyReadStandardOutput();
    void transferFilesToWBFS_readyReadStandardError();
    void transferFilesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferFilesToImage_readyReadStandardOutput();
    void transferFilesToImage_readyReadStandardError();
    void transferFilesToImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferFilesToFileSystem_readyReadStandardOutput();
    void transferFilesToFileSystem_readyReadStandardError();
    void transferFilesToFileSystem_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferDVDToWBFS_readyReadStandardOutput();
    void transferDVDToWBFS_readyReadStandardError();
    void transferDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferDVDToImage_readyReadStandardOutput();
    void transferDVDToImage_readyReadStandardError();
    void transferDVDToImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferDVDToFileSystem_readyReadStandardOutput();
    void transferDVDToFileSystem_readyReadStandardError();
    void transferDVDToFileSystem_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferWBFSToImage_readyReadStandardOutput();
    void transferWBFSToImage_readyReadStandardError();
    void transferWBFSToImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WITOOLS_H
