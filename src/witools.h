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
    explicit WiTools(QObject *parent = 0);

    QStandardItemModel *getFilesGameListModel(QStandardItemModel *model, QString path);
    QStandardItemModel *getDVDGameListModel(QStandardItemModel *model, QString path);
    QStandardItemModel *getWBFSGameListModel(QStandardItemModel *model, QString wbfsPath);

    void transferGamesToWBFS(QModelIndexList indexList, QString wbfsPath);
    void transferGameFromDVDToWBFS(QString drivePath, QString wbfsPath);
    void removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath);
    void transferGamesFromWBFS(QModelIndexList indexList, QString wbfsPath, QString format, QString directory);
    void checkWBFS(QString wbfsPath);

private:
    QProcess *wwtADDProcess;
    QProcess *wwtEXTRACTProcess;
    QProcess *wwtADDDVDProcess;

signals:
    void setMainProgressBarVisible(bool visible);
    void setMainProgressBar(int value, QString format);
    void setProgressBarWBFS(int min, int max, int value, QString text);

    void newLogEntry(QString entry);
    void newStatusBarMessage(QString message);

    void transferGamesToWBFScanceled(bool discExitst);
    void transferGamesToWBFSsuccessfully();

    void transferGameFromDVDToWBFScanceled(bool discExitst);
    void transferGameFromDVDToWBFSsuccessfully();

    void transferGamesFromWBFScanceled();
    void transferGamesFromWBFSsuccessfully();

    void removeGamesFromWBFS_successfully();

private slots:
    void transferGamesToWBFS_readyReadStandardOutput();
    void transferGamesToWBFS_readyReadStandardError();
    void transferGamesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void transferGamesToWBFS_cancel();

    void transferGameFromDVDToWBFS_readyReadStandardOutput();
    void transferGameFromDVDToWBFS_readyReadStandardError();
    void transferGameFromDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void transferGameFromDVDToWBFS_cancel();

    void transferGamesFromWBFS_readyReadStandardOutput();
    void transferGamesFromWBFS_readyReadStandardError();
    void transferGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void transferGamesFromWBFS_cancel();

    void removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // WITOOLS_H
