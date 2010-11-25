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

#include "witools.h"

WiTools::WiTools(QObject *parent) : QObject(parent) {
    setWit();
}

void WiTools::requestFilesGameListModel(QStandardItemModel *model, QString path) {
    emit showStatusBarMessage(tr("Loading games..."));

    QProcess filesRead;
    filesRead.start(wit, QStringList() << "LIST" << "--titles" << witTitlesPath() << "--section" << "--recurse" << path);

    if (!filesRead.waitForFinished(-1)) {
        if (filesRead.errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms ISO Tool not found!"));
            emit newLogEntry(tr("Wiimms ISO Tool not found!"), Error);
        }
        else {
            emit showStatusBarMessage(tr("Loading games failed!"));
            emit newLogEntry(tr("Loading games failed! (status: %1, code: %2,  %3)").arg(QString::number(filesRead.exitStatus()), QString::number(filesRead.exitCode()), filesRead.errorString()), Error);
        }

        return;
    }

    QByteArray bytes = filesRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes), Info);

    double count = 0;
    QList<QStandardItem *> ids, names, titles, regions, sizes, itimes, mtimes, ctimes, atimes, filetypes, filenames;

    foreach (QString line, lines) {
        line.remove("\r");

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("total-discs=0")) {
            emit showStatusBarMessage(tr("No games found!"));
            emit stopBusy();
            return;
        }
        else if (line.contains("total-size=")) {
            count += line.section("=", 1).toDouble();
            continue;
        }
        else if (line.startsWith("id=")) {
            ids.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            names.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            titles.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            regions.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            sizes.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            continue;
        }
        else if (line.startsWith("itime=")) {
            itimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("ctime=")) {
            ctimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("atime=")) {
            atimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            filenames.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->clear();
    model->appendColumn(ids);
    model->appendColumn(names);
    model->appendColumn(titles);
    model->appendColumn(regions);
    model->appendColumn(sizes);
    model->appendColumn(itimes);
    model->appendColumn(mtimes);
    model->appendColumn(ctimes);
    model->appendColumn(atimes);
    model->appendColumn(filetypes);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Name (%1 / %2 GB)").arg(QString::number(ids.count()), QString::number((count / 1073741824), 'f', 2)));
    model->setHeaderData(2, Qt::Horizontal, tr("Title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(6, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(9, Qt::Horizontal, tr("Type"));
    model->setHeaderData(10, Qt::Horizontal, tr("Source"));

    ids.clear();
    names.clear();
    titles.clear();
    regions.clear();
    sizes.clear();
    itimes.clear();
    mtimes.clear();
    ctimes.clear();
    atimes.clear();
    filetypes.clear();
    filenames.clear();

    emit newFilesGameListModel();
}

void WiTools::requestDVDGameListModel(QStandardItemModel *model, QString path) {
    emit showStatusBarMessage(tr("Loading disc..."));

    QProcess dvdRead;
    dvdRead.start(wit, QStringList() << "LIST-LL" << path << "--titles" << witTitlesPath() << "--section"); //Windows: 0 games found, in admin mode too!?

    if (!dvdRead.waitForFinished(-1)) {
        if (dvdRead.errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms ISO Tool not found!"));
            emit newLogEntry(tr("Wiimms ISO Tool not found!"), Error);
        }
        else {
            emit showStatusBarMessage(tr("Loading game disc failed!"));
            emit newLogEntry(tr("Loading game disc failed! (status: %1, code: %2,  %3)").arg(QString::number(dvdRead.exitStatus()), QString::number(dvdRead.exitCode()), dvdRead.errorString()), Error);
        }

        return;
    }

    QByteArray bytes = dvdRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes), Info);

    QList<QStandardItem *> game;

    foreach (QString line, lines) {
        line.remove("\r");

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("name=CAN'T OPEN FILE")) {
            emit showStatusBarMessage(tr("Can't open file!"));
            emit stopBusy();
            return;
        }
        else if (line.contains("total-discs=0")) {
            emit showStatusBarMessage(tr("No game found!"));
            emit stopBusy();
            return;
        }
        else if (line.startsWith("id=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            game.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            continue;
        }
        else if (line.startsWith("itime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("mtime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("ctime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("atime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("filetype=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("container=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("disctype=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("n-partitions=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("partition-info=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("wbfs_slot=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("source=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->clear();
    model->appendColumn(game);

    model->setHeaderData(0, Qt::Vertical, tr("ID:"));
    model->setHeaderData(1, Qt::Vertical, tr("Name:"));
    model->setHeaderData(2, Qt::Vertical, tr("Title:"));
    model->setHeaderData(3, Qt::Vertical, tr("Region:"));
    model->setHeaderData(4, Qt::Vertical, tr("Size:"));
    model->setHeaderData(5, Qt::Vertical, tr("Insertion:"));
    model->setHeaderData(6, Qt::Vertical, tr("Last modification:"));
    model->setHeaderData(7, Qt::Vertical, tr("Last status change:"));
    model->setHeaderData(8, Qt::Vertical, tr("Last access:"));
    model->setHeaderData(9, Qt::Vertical, tr("Type:"));
    model->setHeaderData(10, Qt::Vertical, tr("Container:"));
    model->setHeaderData(11, Qt::Vertical, tr("Disc type:"));
    model->setHeaderData(12, Qt::Vertical, tr("N partitions:"));
    model->setHeaderData(13, Qt::Vertical, tr("Partition info:"));
    model->setHeaderData(14, Qt::Vertical, tr("WBFS slot:"));
    model->setHeaderData(15, Qt::Vertical, tr("Source:"));

    game.clear();

    emit newDVDGameListModel();
}

void WiTools::requestWBFSGameListModel(QStandardItemModel *model, QString wbfsPath) {
    emit showStatusBarMessage(tr("Loading games..."));

    QStringList arguments;
    arguments.append("LIST-L");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append("--titles");
    arguments.append(witTitlesPath());

    arguments.append("--section");

    QProcess wbfsRead;
    wbfsRead.start(wwt, arguments);

    if (!wbfsRead.waitForFinished(-1)) {
        if (wbfsRead.errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms WBFS Tool not found!"));
            emit newLogEntry(tr("Wiimms WBFS Tool not found!"), Error);
        }
        else {
            emit showStatusBarMessage(tr("Loading games failed!"));
            emit newLogEntry(tr("Loading games failed! (status: %1, code: %2,  %3)").arg(QString::number(wbfsRead.exitStatus()), QString::number(wbfsRead.exitCode()), wbfsRead.errorString()), Error);
        }

        return;
    }

    QByteArray bytes = wbfsRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes), Info);

    int current = 0, max = 0;
    QString file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB;
    QList<QStandardItem *> ids, names, titles, regions, sizes, usedblocks, itimes, mtimes, ctimes, atimes, filetypes, wbfsslots, filenames;

    foreach (QString line, lines) {
        line.remove("\r");

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("name=NO WBFS FOUND")) {
            emit showStatusBarMessage(tr("No WBFS partitions found!"));
            emit stopBusy();
            return;
        }
        else if (line.contains("used_discs=0")) {
            emit showStatusBarMessage(tr("No games found!"));
            emit stopBusy();
            return;
        }
        else if (line.startsWith("file=")) {
            file = tr("%1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("used_discs=")) {
            usedDiscs = tr("Used discs: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("total_discs=")) {
            totalDiscs = tr("Total discs: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("used_mib")) {
            usedMB = tr("Used MiB: %1").arg(line.section("=", 1));
            current = line.section("=", 1).toInt();
            continue;
        }
        else if (line.startsWith("free_mib=")) {
            freeMB = tr("Free MiB: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("total_mib=")) {
            totalMB = tr("Total MiB: %1").arg(line.section("=", 1));
            max = line.section("=", 1).toInt();
            continue;
        }
        else if (line.startsWith("id=")) {
            ids.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            names.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            titles.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            regions.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            sizes.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            continue;
        }
        else if (line.startsWith("used_blocks=")) {
            usedblocks.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("itime=")) {
            itimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("ctime=")) {
            ctimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("atime=")) {
            atimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("wbfs_slot=")) {
            wbfsslots.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            filenames.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->clear();
    model->appendColumn(ids);
    model->appendColumn(names);
    model->appendColumn(titles);
    model->appendColumn(regions);
    model->appendColumn(sizes);
    model->appendColumn(usedblocks);
    model->appendColumn(itimes);
    model->appendColumn(mtimes);
    model->appendColumn(ctimes);
    model->appendColumn(atimes);
    model->appendColumn(filetypes);
    model->appendColumn(wbfsslots);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, tr("Title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Used blocks"));
    model->setHeaderData(6, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(9, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(10, Qt::Horizontal, tr("Type"));
    model->setHeaderData(11, Qt::Horizontal, tr("WBFS slot"));
    model->setHeaderData(12, Qt::Horizontal, tr("Source"));

    ids.clear();
    names.clear();
    titles.clear();
    regions.clear();
    sizes.clear();
    usedblocks.clear();
    itimes.clear();
    mtimes.clear();
    ctimes.clear();
    atimes.clear();
    filetypes.clear();
    wbfsslots.clear();
    filenames.clear();

    #ifdef Q_OS_MACX
        emit setProgressBarWBFS(0, max, current, "%p%");
        emit setInfoTextWBFS(QString("%1 - %2 - %3 - %4 (%5%) - %6 - %7").arg(file, usedDiscs, totalDiscs, usedMB, QString::number(current * 100 / max, 'f', 0), freeMB, totalMB));
    #else
        emit setProgressBarWBFS(0, max, current, QString("%1 - %2 - %3 - %4 (%p%) - %5 - %6").arg(file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB));
    #endif

    emit newWBFSGameListModel();
}

void WiTools::transferGamesToWBFS(QModelIndexList indexList, QString wbfsPath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    QStringList paths;
    foreach (QModelIndex index, indexList) {
        paths.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("ADD");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append(paths);

    if (WiiBaFuSettings.value("FilesToWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("FilesToWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("FilesToWBFS/Newer", QVariant(false)).toBool()) {
        arguments.append("--newer");
    }

    if (WiiBaFuSettings.value("FilesToWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("FilesToWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transfer_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transfer_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGamesToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferGamesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit transferGamesToWBFSsuccessfully();
        }
        else if (exitCode == 4) {
            emit transferGamesToWBFScanceled(true);
        }
    }
    else {
        emit transferGamesToWBFScanceled(false);
    }

    delete witProcess;
}

void WiTools::transferGamesToImage(QModelIndexList indexList, QString wbfsPath, QString format, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    QStringList paths;
    foreach (QModelIndex index, indexList) {
        paths.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("EXTRACT");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append(paths);
    arguments.append(QString("--").append(format));
    arguments.append("--dest");
    arguments.append(directory);

    if (WiiBaFuSettings.value("FilesFromWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("FilesFromWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("FilesFromWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("FilesFromWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transfer_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transfer_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGamesToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferGamesToImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit transferGamesToImageSuccessfully();
    }
    else {
        emit transferGamesToImageCanceled();
    }

    delete witProcess;
}

void WiTools::transferGameFromDVDToWBFS(QString drivePath, QString wbfsPath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    QStringList arguments;
    arguments.append("ADD");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append(drivePath);

    if (WiiBaFuSettings.value("DVDtoWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("DVDtoWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("DVDtoWBFS/Newer", QVariant(false)).toBool()) {
        arguments.append("--newer");
    }

    if (WiiBaFuSettings.value("DVDtoWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("DVDtoWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transfer_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transfer_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGameFromDVDToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferGameFromDVDToImage(QString drivePath, QString format, QString filePath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    QStringList arguments;
    arguments.append("COPY");
    arguments.append(drivePath);
    arguments.append(filePath);
    arguments.append(QString("--").append(format));

    if (WiiBaFuSettings.value("GamesToImage/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("GamesToImage/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("GamesToImage/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transfer_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transfer_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGameFromDVDToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferGameFromDVDToImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit transferGameFromDVDToImageSuccessfully();
        }
        else if (exitCode == 4) {
            emit transferGameFromDVDToImageCanceled(true);
        }
    }
    else {
        emit transferGameFromDVDToImageCanceled(false);
    }

    delete witProcess;
}

void WiTools::transferGameFromDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit transferGameFromDVDToWBFSsuccessfully();
        }
        else if (exitCode == 4) {
            emit transferGameFromDVDToWBFScanceled(true);
        }
    }
    else {
        emit transferGameFromDVDToWBFScanceled(false);
    }

    delete witProcess;
}

void WiTools::transfer_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("ADD")) {
        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3)));
        #endif
    }
    else if (line.contains("COPY")) {
        if (line.contains("/dev/")) {
            #ifdef Q_OS_MACX
                gameCountText = tr("Transfering game %1...").arg(line.mid(line.indexOf("ISO:") + 4, line.lastIndexOf(":") - line.indexOf(":") - 1));
                emit showStatusBarMessage(gameCountText);
            #else
                emit showStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("ISO:") + 4, line.lastIndexOf(":") - line.indexOf(":") - 1)));
            #endif
        }
        else {
            QString file = line.mid(line.indexOf(":") + 1, line.indexOf("->") - line.indexOf(":") - 2);
            QString from = file.mid(file.lastIndexOf("/") + 1, file.length() - file.lastIndexOf("/"));
            QString to = line.mid(line.indexOf("->") + 3, line.lastIndexOf(":") - line.indexOf("->") - 3);

            #ifdef Q_OS_MACX
                gameCountText = tr("Transfering game %1 -> %2...").arg(from, to);
                emit showStatusBarMessage(gameCountText);
            #else
                emit showStatusBarMessage(tr("Transfering game %1 -> %2...").arg(from, to));
            #endif
        }
    }
    else if (line.contains("EXTRACT")) {
        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1...").arg(line.mid(line.indexOf("EXTRACT") + 8, line.lastIndexOf(":") - line.indexOf("EXTRACT") - 8));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("EXTRACT") + 8, line.lastIndexOf(":") - line.indexOf("EXTRACT") - 8)));
        #endif
    }
    else if (line.contains("% copied")) {
        emit setMainProgressBar(line.left(line.indexOf("%")).remove(" ").toInt(), line);

        #ifdef Q_OS_MACX
            emit showStatusBarMessage(QString("%1%2").arg(gameCountText, line));
        #endif
    }
    else if (line.contains("copied") && !line.contains("%")) {
        emit newLogEntry(line.remove(0, 5), Info);
    }
    else if (line.contains("disc added.") || line.contains("discs added.")) {
        emit newLogEntry(line, Info);
    }
    else if (line.contains("disc extracted.") || line.contains("discs extracted.")) {
        emit newLogEntry(line, Info);
    }
}

void WiTools::transfer_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transfer_cancel() {
    witProcess->kill();
}

void WiTools::convertGameImages(QModelIndexList indexList, QString format, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    QStringList paths;
    foreach (QModelIndex index, indexList) {
        paths.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("COPY");

    arguments.append(paths);
    arguments.append(QString("--").append(format));
    arguments.append("--dest");
    arguments.append(directory);

    if (WiiBaFuSettings.value("GamesToImage/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("GamesToImage/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("GamesToImage/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transfer_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transfer_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGamesToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath) {
    QStringList ids;
    foreach (QModelIndex index, indexList) {
        ids.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("REMOVE");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append(ids);

    if (WiiBaFuSettings.value("FilesToWBFS/Force", QVariant(false)).toBool())
        arguments.append("--force");
    if (WiiBaFuSettings.value("FilesToWBFS/Test", QVariant(false)).toBool())
        arguments.append("--test");

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(removeGamesFromWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    delete witProcess;
}

void WiTools::removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QString message(tr("Games removed successfully!"));
        emit showStatusBarMessage(message);
        emit newLogEntry(message, Info);
        emit removeGamesFromWBFS_successfully();
    }
    else {
        QString message(tr("Games removed failed!"));
        emit showStatusBarMessage(message);
        emit newLogEntry(message, Info);
    }
}

void WiTools::checkWBFS(QString wbfsPath) {
    if (WiiBaFuSettings.value("CheckWBFS/Repair", QVariant(true)).toBool()) {
        emit showStatusBarMessage(tr("Checking and repairing WBFS..."));
    }
    else {
        emit showStatusBarMessage(tr("Checking WBFS..."));
    }

    QStringList arguments;
    arguments.append("CHECK");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    if (WiiBaFuSettings.value("CheckWBFS/Repair", QVariant(true)).toBool()) {
        arguments.append("--repair");

        QString repairModes;

        if (WiiBaFuSettings.value("RepairWBFS/FBT", QVariant(false)).toBool()) {
            repairModes.append("FBT,");
        }
        if (WiiBaFuSettings.value("RepairWBFS/INODES", QVariant(false)).toBool()) {
            repairModes.append("INODES,");
        }
        if (WiiBaFuSettings.value("RepairWBFS/RM-EMTPY", QVariant(false)).toBool()) {
            repairModes.append("RM-EMPTY,");
        }
        if (WiiBaFuSettings.value("RepairWBFS/RM-FREE", QVariant(false)).toBool()) {
            repairModes.append("RM-FREE,");
        }
        if (WiiBaFuSettings.value("RepairWBFS/RM-INVALID", QVariant(false)).toBool()) {
            repairModes.append("RM-INVALID,");
        }
        if (WiiBaFuSettings.value("RepairWBFS/RM-OVERLAP", QVariant(false)).toBool()) {
            repairModes.append("RM-OVERLAP,");
        }

        if (repairModes.endsWith(",")) {
            repairModes.remove(repairModes.length() - 1, 1);
        }

        arguments.append(repairModes);
    }

    if (WiiBaFuSettings.value("CheckWBFS/Test", QVariant(true)).toBool()) {
        arguments.append("--test");
    }

    arguments.append("--long");

    witProcess = new QProcess();
    witProcess->start(wwt, arguments);

    if (!witProcess->waitForFinished(-1)) {
        if (witProcess->errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms WBFS Tool not found!"));
            emit newLogEntry(tr("Wiimms WBFS Tool not found!"), Error);
        }
        else {
            emit showStatusBarMessage(tr("WBFS check failed!"));
            emit newLogEntry(tr("WBFS check failed! (status: %1, code: %2,  %3)").arg(QString::number(witProcess->exitStatus()), QString::number(witProcess->exitCode()), witProcess->errorString()), Error);
        }
    }
    else {
        emit showStatusBarMessage(tr("WBFS check successfully!"));
    }

    emit newLogEntry(QString(witProcess->readAll().data()), Info);

    arguments.clear();
    delete witProcess;
}

void WiTools::createWBFS(CreateWBFSParameters parameters) {
    emit showStatusBarMessage(tr("Creating WBFS..."));

    QStringList arguments;
    arguments.append("FORMAT");
    arguments.append(parameters.Path);
    arguments.append("--size");
    arguments.append(parameters.Size);

    if (!parameters.SplitSize.isEmpty()) {
        arguments.append("--split-size");
        arguments.append(parameters.SplitSize);
    }

    if (!parameters.HDSectorSize.isEmpty()) {
        arguments.append("--hss");
        arguments.append(parameters.HDSectorSize);
    }

    if (!parameters.WBFSSectorSize.isEmpty()) {
        arguments.append("--wss");
        arguments.append(parameters.WBFSSectorSize);
    }

    if (parameters.Recover) {
        arguments.append("--recover");
    }

    if (parameters.Inode) {
        arguments.append("--inode");
    }

    if (parameters.Test) {
        arguments.append("--test");
    }

    arguments.append("--force");
    arguments.append("--verbose");

    witProcess = new QProcess();
    witProcess->start(wwt, arguments);

    if (!witProcess->waitForFinished(-1)) {
        emit showStatusBarMessage(tr("Create WBFS failed!"));
        emit newLogEntry(witProcess->readAllStandardError(), Error);
    }
    else {
        emit showStatusBarMessage(tr("Create WBFS successfully!"));
    }

    emit newLogEntry(QString(witProcess->readAll()), Info);

    arguments.clear();
    delete witProcess;

    emit stopBusy();
}

void WiTools::setWit() {
    if (WiiBaFuSettings.value("Main/PathToWIT", QVariant("")).toString().isEmpty()) {
        #ifdef Q_OS_LINUX
            QDir::setSearchPaths("wit", QStringList() << QDir::currentPath().append("/wit") << QString(getenv("PATH")).split(":"));
            wit = QFile("wit:wit").fileName();
            wwt = QFile("wit:wwt").fileName();
        #endif

        #ifdef Q_OS_WIN32
            QDir::setSearchPaths("wit", QStringList() << QDir::currentPath().append("/wit") << QString(getenv("PATH")).split(";"));
            wit = QFile("wit:wit.exe").fileName();
            wwt = QFile("wit:wwt.exe").fileName();
        #endif

        #ifdef Q_OS_MACX
            QDir::setSearchPaths("wit", QStringList() << QDir::currentPath().remove("MacOS").append("wit") << QDir::currentPath().append("/Wii Backup Fusion.app/Contents/wit") << QString(getenv("PATH")).split(":"));
            wit = QFile("wit:wit").fileName();
            wwt = QFile("wit:wwt").fileName();
        #endif
    }
    else {
        #ifdef Q_OS_WIN32
            wit = WiiBaFuSettings.value("Main/PathToWIT").toString().append("/wit.exe");
            wwt = WiiBaFuSettings.value("Main/PathToWIT").toString().append("/wwt.exe");
        #else
            wit = WiiBaFuSettings.value("Main/PathToWIT").toString().append("/wit");
            wwt = WiiBaFuSettings.value("Main/PathToWIT").toString().append("/wwt");
        #endif
    }
}

QString WiTools::witVersion() {
    QProcess witCheckProcess;
    witCheckProcess.start(wit, QStringList() << "--version");

    if (!witCheckProcess.waitForFinished(-1)) {
        if (witCheckProcess.errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms ISO Tool not found!"));
            return QString(tr("Wiimms ISO Tool not found!"));
        }
        else {
            return QString(witCheckProcess.errorString());
        }
    }

    #ifdef Q_OS_WIN32
        return QString(witCheckProcess.readLine()).remove("\r\n");
    #else
        return QString(witCheckProcess.readLine()).remove("\n");
    #endif
}

QString WiTools::wwtVersion() {
    QProcess wwtCheckProcess;
    wwtCheckProcess.start(wwt, QStringList() << "--version");

    if (!wwtCheckProcess.waitForFinished(-1)) {
        if (wwtCheckProcess.errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms WBFS Tool not found!"));
            return QString(tr("Wiimms WBFS Tool not found!"));
        }
        else {
            return QString(wwtCheckProcess.errorString());
        }
    }

    #ifdef Q_OS_WIN32
        return QString(wwtCheckProcess.readLine()).remove("\r\n");
    #else
        return QString(wwtCheckProcess.readLine()).remove("\n");
    #endif
}

QString WiTools::witTitlesPath() {
    QString titles;

    switch (WiiBaFuSettings.value("Main/Language", QVariant(0)).toInt()) {
        case 0:  titles = "titles";
                 break;
        case 1:  titles = "titles";
                 break;
        case 2:  titles = "titles-fr";
                 break;
        case 3:  titles = "titles-de";
                 break;
        case 4:  titles = "titles-es";
                 break;
        case 5:  titles = "titles-it";
                 break;
        case 6:  titles = "titles-nl";
                 break;
        case 7:  titles = "titles-pt";
                 break;
        case 8:  titles = "titles-se";
                 break;
        case 9:  titles = "titles-dk";
                 break;
        case 10:  titles = "titles-no";
                 break;
        case 11: titles = "titles-fi";
                 break;
        case 12: titles = "titles-ru";
                 break;
        case 13: titles = "titles-ja";
                 break;
        case 14: titles = "titles-ko";
                 break;
        case 15: titles = "titles-zhtw";
                 break;
        case 16: titles = "titles-zhcn";
                 break;
        default: titles = "titles";
    }

    if (WiiBaFuSettings.value("Main/PathToWIT", QVariant("")).toString().isEmpty()) {
        #ifdef Q_OS_LINUX
            QDir::setSearchPaths("witTitles", QStringList() << QDir::currentPath().append("/wit") << QString(getenv("PATH")).split(":") << "/usr/local/share/wit");
            return QFile(QString("witTitles:%1.txt").arg(titles)).fileName();
        #endif

        #ifdef Q_OS_WIN32
            QDir::setSearchPaths("witTitles", QStringList() << QDir::currentPath().append("/wit") << QString(getenv("PATH")).split(";"));
            return QFile(QString("witTitles:%1.txt").arg(titles)).fileName();
        #endif

        #ifdef Q_OS_MACX
            QDir::setSearchPaths("witTitles", QStringList() << QDir::currentPath().remove("MacOS").append("wit") << QDir::currentPath().append("/Wii Backup Fusion.app/Contents/wit") << QString(getenv("PATH")).split(":") << QString(getenv("WIT-TITLES")).split(":") << "/usr/local/share/wit");
            return QFile(QString("witTitles:%1.txt").arg(titles)).fileName();
        #endif
    }
    else {
        return WiiBaFuSettings.value("Main/PathToWIT").toString().append(QString("/%1.txt").arg(titles));
    }
}
