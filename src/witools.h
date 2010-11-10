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

class WiTools : public QObject {
    Q_OBJECT

public:
    enum LogType {
        Info = 0x0,
        Error = 0x1
    };

    explicit WiTools(QObject *parent = 0);

    void requestFilesGameListModel(QStandardItemModel *model, QString path);
    void requestDVDGameListModel(QStandardItemModel *model, QString path);
    void requestWBFSGameListModel(QStandardItemModel *model, QString wbfsPath);

    void transferGamesToWBFS(QModelIndexList indexList, QString wbfsPath);
    void transferGamesFromWBFS(QModelIndexList indexList, QString wbfsPath, QString format, QString directory);
    void transferGameFromDVDToWBFS(QString drivePath, QString wbfsPath);

    void removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath);
    void checkWBFS(QString wbfsPath);

    QString witVersion();
    QString wwtVersion();

private:
    QString wit, wwt;
    QProcess *witProcess;

    #ifdef Q_OS_MAC
        QString gameCountText;
    #endif

    void setWit();
    QString witTitlesPath();

signals:
    void setMainProgressBarVisible(bool visible);
    void setMainProgressBar(int value, QString format);
    void setInfoTextWBFS(QString text);
    void setProgressBarWBFS(int min, int max, int value, QString text);

    void newLogEntry(QString entry, WiTools::LogType type);
    void newStatusBarMessage(QString message);

    void newFilesGameListModel();
    void newDVDGameListModel();
    void newWBFSGameListModel();

    void transferGamesToWBFScanceled(bool discExitst);
    void transferGamesToWBFSsuccessfully();

    void transferGamesFromWBFScanceled();
    void transferGamesFromWBFSsuccessfully();

    void transferGameFromDVDToWBFScanceled(bool discExitst);
    void transferGameFromDVDToWBFSsuccessfully();

    void removeGamesFromWBFS_successfully();

private slots:
    void transferGamesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void transferGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void transferGameFromDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void transfer_readyReadStandardOutput();
    void transfer_readyReadStandardError();
    void transfer_cancel();

    void removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WITOOLS_H
