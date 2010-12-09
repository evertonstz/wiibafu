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
        VerificationCanceled = 0x03,
        DiscAlreadyExists = 0x04,
        FileAlreadyExists = 0x05,
        DestinationAlreadyExists = 0x06,
        NoSuchFileOrDirectory = 0x07,
        NoGamesFound = 0x08
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
    void extractImage(QModelIndexList indexList, QString destination);

    void transferDVDToWBFS(QString dvdPath, QString wbfsPath);
    void transferDVDToImage(QString dvdPath, QString format, QString compression, QString directory);
    void extractDVD(QString dvdPath, QString destination);

    void transferWBFSToImage(QModelIndexList indexList, QString wbfsPath, QString format, QString compression, QString directory);
    void extractWBFS(QModelIndexList indexList, QString wbfsPath, QString destination);

    void removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath);
    void checkWBFS(QString wbfsPath);
    void createWBFS(CreateWBFSParameters parameters);
    void verifyGame(int index, QString wbfsPath, QString game);

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
    void newLogEntries(QStringList entries, WiTools::LogType type);
    void newWitCommandLineLogEntry(QString wit, QStringList arguments);
    void showStatusBarMessage(QString message);

    void newFilesGameListModel();
    void newDVDGameListModel();
    void newWBFSGameListModel();
    void loadingGamesCanceled();
    void loadingGamesFailed(WiTools::WitStatus);

    void transferFilesToWBFS_finished(WiTools::WitStatus);
    void transferFilesToImage_finished(WiTools::WitStatus);
    void extractImage_finished(WiTools::WitStatus);

    void transferDVDToWBFS_finished(WiTools::WitStatus);
    void transferDVDToImage_finished(WiTools::WitStatus);
    void extractDVD_finished(WiTools::WitStatus);

    void transferWBFSToImage_finished(WiTools::WitStatus);
    void extractWBFS_finished(WiTools::WitStatus);

    void removeGamesFromWBFS_successfully();

    void verifyGame_finished(WiTools::WitStatus);

public slots:
    void cancelTransfer();
    void cancelLoading();
    void cancelVerifying();

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

    void extractImage_readyReadStandardOutput();
    void extractImage_readyReadStandardError();
    void extractImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferDVDToWBFS_readyReadStandardOutput();
    void transferDVDToWBFS_readyReadStandardError();
    void transferDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferDVDToImage_readyReadStandardOutput();
    void transferDVDToImage_readyReadStandardError();
    void transferDVDToImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void extractDVD_readyReadStandardOutput();
    void extractDVD_readyReadStandardError();
    void extractDVD_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transferWBFSToImage_readyReadStandardOutput();
    void transferWBFSToImage_readyReadStandardError();
    void transferWBFSToImage_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void extractWBFS_readyReadStandardOutput();
    void extractWBFS_readyReadStandardError();
    void extractWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void verifyGame_readyReadStandardOutput();
    void verifyGame_readyReadStandardError();
    void verifyGame_finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WITOOLS_H
