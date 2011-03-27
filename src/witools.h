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

    struct GamePatchParameters {
        bool Patch;
        QString ID;
        QString Name;
        QString Region;
        QString IOS;
        QString Modify;
        QString EncodingMode;
        QString CommonKey;
    };

    struct TransferFilesToImageParameters {
        QModelIndexList IndexList;
        QString Format;
        QString Compression;
        QString Directory;
        QString SplitSize;
        WiTools::GamePatchParameters PatchParameters;
    };

    explicit WiTools(QObject *parent = 0);

    void requestFilesGameListModel(QStandardItemModel *model, const QString path, const int recurseDepth);
    void requestDVDGameListModel(QStandardItemModel *model, const QString path);
    void requestWBFSGameListModel(QStandardItemModel *model, const QString wbfsPath);

    void transferFilesToWBFS(const QModelIndexList indexList, const QString wbfsPath, const WiTools::GamePatchParameters parameters);
    void transferFilesToImage(WiTools::TransferFilesToImageParameters transferParameters);
    void extractImage(const QModelIndexList indexList, const QString destination, const WiTools::GamePatchParameters patchParameters);

    void transferDVDToWBFS(const QString dvdPath, const QString wbfsPath, const WiTools::GamePatchParameters patchParameters);
    void transferDVDToImage(const QString dvdPath, const WiTools::TransferFilesToImageParameters transferParameters);
    void extractDVD(const QString dvdPath, const QString destination, const WiTools::GamePatchParameters patchParameters);

    void transferWBFSToImage(const QString wbfsPath, const WiTools::TransferFilesToImageParameters transferParameters);
    void extractWBFS(const QModelIndexList indexList, const QString wbfsPath, const QString destination, const WiTools::GamePatchParameters patchParameters);

    void removeGamesFromWBFS(const QModelIndexList indexList, const QString wbfsPath);
    void checkWBFS(const QString wbfsPath);
    void dumpWBFS(const QString wbfsPath);
    void createWBFS(const CreateWBFSParameters parameters);
    void verifyGame(const int index, const QString wbfsPath, const QString game);

    void patchGameImage(const QString filePath, const WiTools::GamePatchParameters parameters);

    QString witVersion();
    QString wwtVersion();
    QString witTitlesPath();

private:
    QString wit, wwt;
    QProcess *witProcess;
    WitStatus witProcessStatus;
    QStandardItemModel *witModel;

    double totalGameSize;
    QStandardItem *fgl_item;
    QList<QStandardItem *> fgl_ids, fgl_names, fgl_titles, fgl_regions, fgl_sizes, fgl_itimes, fgl_mtimes, fgl_ctimes, fgl_atimes, fgl_filetypes, fgl_filenames;

    QString n_scanned;
    QString n_directories;
    QString n_found;

    QString job_counter;
    QString job_total;
    QString source_path;
    QString dest_path;
    QString dest_type;

    QString percent;
    QString elapsed_text;
    QString eta_text;
    QString mib_total;
    QString mib_per_sec;

    void setWit();
    void resetProgressBarVariables();

signals:
    void stopBusy();

    void setMainProgressBarVisible(const bool visible);
    void setMainProgressBar(const int value, const QString format);
    void setInfoTextWBFS(const QString text);
    void setProgressBarWBFS(const int min, const int max, const int value, const QString text);

    void newLogEntry(const QString entry, const WiTools::LogType type);
    void newLogEntries(const QStringList entries, const WiTools::LogType type);
    void newWitCommandLineLogEntry(const QString wit, const QStringList arguments);
    void showStatusBarMessage(const QString message);

    void newFilesGameListModel();
    void newDVDGameListModel();
    void newWBFSGameListModel();
    void loadingGamesCanceled();
    void loadingGamesFailed(const WiTools::WitStatus);

    void transferFilesToWBFS_finished(const WiTools::WitStatus);
    void transferFilesToImage_finished(const WiTools::WitStatus);
    void extractImage_finished(const WiTools::WitStatus);

    void transferDVDToWBFS_finished(const WiTools::WitStatus);
    void transferDVDToImage_finished(const WiTools::WitStatus);
    void extractDVD_finished(const WiTools::WitStatus);

    void transferWBFSToImage_finished(const WiTools::WitStatus);
    void extractWBFS_finished(const WiTools::WitStatus);

    void removeGamesFromWBFS_successfully();

    void verifyGame_finished(const WiTools::WitStatus);

    void patchGameImage_finished(const WiTools::WitStatus);

public slots:
    void cancelTransfer();
    void cancelLoading();
    void cancelVerifying();

private slots:
    void requestFilesGameListModel_readyReadStandardOutput();
    void requestFilesGameListModel_readyReadStandardError();
    void requestFilesGameListModel_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void transferFilesToWBFS_readyReadStandardOutput();
    void transferFilesToWBFS_readyReadStandardError();
    void transferFilesToWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void transferFilesToImage_readyReadStandardOutput();
    void transferFilesToImage_readyReadStandardError();
    void transferFilesToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void extractImage_readyReadStandardOutput();
    void extractImage_readyReadStandardError();
    void extractImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void transferDVDToWBFS_readyReadStandardOutput();
    void transferDVDToWBFS_readyReadStandardError();
    void transferDVDToWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void transferDVDToImage_readyReadStandardOutput();
    void transferDVDToImage_readyReadStandardError();
    void transferDVDToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void extractDVD_readyReadStandardOutput();
    void extractDVD_readyReadStandardError();
    void extractDVD_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void transferWBFSToImage_readyReadStandardOutput();
    void transferWBFSToImage_readyReadStandardError();
    void transferWBFSToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void extractWBFS_readyReadStandardOutput();
    void extractWBFS_readyReadStandardError();
    void extractWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void removeGamesFromWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus);

    void verifyGame_readyReadStandardOutput();
    void verifyGame_readyReadStandardError();
    void verifyGame_finished(const int exitCode, const QProcess::ExitStatus exitStatus);
};

#endif // WITOOLS_H
