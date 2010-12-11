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
#include "common.h"

WiTools::WiTools(QObject *parent) : QObject(parent) {
    setWit();
}

void WiTools::requestFilesGameListModel(QStandardItemModel *model, QString path) {
    emit showStatusBarMessage(tr("Loading games..."));
    emit newLogEntry(tr("Loading games from images...\n"), Info);

    witModel = model;
    totalGameSize = 0;

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(requestFilesGameListModel_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(requestFilesGameListModel_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(requestFilesGameListModel_finished(int, QProcess::ExitStatus)));

    QStringList arguments = QStringList() << "LIST" << "--titles" << witTitlesPath() << "--recurse" << path << "--section" << "--progress";
    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess->start(wit, arguments);

    if (!witProcess->waitForFinished(-1)) {
        if (witProcess->errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms ISO Tool not found!"));
            emit newLogEntry(tr("Wiimms ISO Tool not found!"), Error);
            emit loadingGamesFailed(NoSuchFileOrDirectory);
        }
        else {
            emit showStatusBarMessage(tr("Loading games failed!"));
            emit newLogEntry(tr("Loading games failed! (status: %1, code: %2,  %3)").arg(QString::number(witProcess->exitStatus()), QString::number(witProcess->exitCode()), witProcess->errorString()), Error);
            emit loadingGamesFailed(UnknownError);
        }

        return;
    }
}

void WiTools::requestFilesGameListModel_readyReadStandardOutput() {
    QString line = QString(witProcess->readLine().constData()).remove("\r").remove("\n");

    if (!line.isEmpty() && line.contains("scanned")) {
        emit showStatusBarMessage(line);
        emit newLogEntry(line, Info);
    }
    else if (line.contains("total-size=")) {
        totalGameSize = line.section("=", 1).toDouble();
    }
}

void WiTools::requestFilesGameListModel_readyReadStandardError() {
    emit showStatusBarMessage(tr("Loading games failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::requestFilesGameListModel_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        emit newLogEntry(tr("Loading games canceled!"), Info);
        emit loadingGamesCanceled();
    }
    else if (exitCode != 0) {
        emit newLogEntry(tr("Loading failed!"), Error);
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QStandardItem *item = 0;
        QTextStream textStream(witProcess);
        textStream.setCodec("UTF-8");
        QList<QStandardItem *> ids, names, titles, regions, sizes, itimes, mtimes, ctimes, atimes, filetypes, filenames;

        while (!textStream.atEnd()) {
            QString line = textStream.readLine();

            emit newLogEntry(line, Info);

            if (line.isEmpty()) {
                continue;
            }
            else if (line.contains("total-discs=0")) {
                emit showStatusBarMessage(tr("No games found!"));
                emit loadingGamesFailed(NoGamesFound);
                return;
            }
            else if (line.contains("total-size=")) {
                totalGameSize = line.section("=", 1).toDouble();
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
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                itimes.append(item);
                continue;
            }
            else if (line.startsWith("mtime=")) {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                mtimes.append(item);
                continue;
            }
            else if (line.startsWith("ctime=")) {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                ctimes.append(item);
                continue;
            }
            else if (line.startsWith("atime=")) {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                atimes.append(item);
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

        witModel->clear();
        witModel->appendColumn(ids);
        witModel->appendColumn(names);
        witModel->appendColumn(titles);
        witModel->appendColumn(regions);
        witModel->appendColumn(sizes);
        witModel->appendColumn(itimes);
        witModel->appendColumn(mtimes);
        witModel->appendColumn(ctimes);
        witModel->appendColumn(atimes);
        witModel->appendColumn(filetypes);
        witModel->appendColumn(filenames);

        witModel->setHeaderData(0, Qt::Horizontal, tr("ID (0)"));
        witModel->setHeaderData(1, Qt::Horizontal, tr("Name (%1 GB)").arg(QString::number((totalGameSize / 1073741824), 'f', 2)));
        witModel->setHeaderData(2, Qt::Horizontal, tr("Title"));
        witModel->setHeaderData(3, Qt::Horizontal, tr("Region"));
        witModel->setHeaderData(4, Qt::Horizontal, tr("Size"));
        witModel->setHeaderData(5, Qt::Horizontal, tr("Insertion"));
        witModel->setHeaderData(6, Qt::Horizontal, tr("Last modification"));
        witModel->setHeaderData(7, Qt::Horizontal, tr("Last status change"));
        witModel->setHeaderData(8, Qt::Horizontal, tr("Last access"));
        witModel->setHeaderData(9, Qt::Horizontal, tr("Type"));
        witModel->setHeaderData(10, Qt::Horizontal, tr("Source"));

        emit newFilesGameListModel();
    }

    delete witProcess;
}

void WiTools::requestDVDGameListModel(QStandardItemModel *model, QString path) {
    emit showStatusBarMessage(tr("Loading disc..."));
    emit newLogEntry(tr("Loading disc...\n"), Info);

    QProcess dvdRead;

    QStringList arguments = QStringList() << "LIST-LL" << path << "--titles" << witTitlesPath() << "--section";
    emit newWitCommandLineLogEntry("wit", arguments);

    dvdRead.start(wit, arguments);

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

    QStandardItem *item = 0;
    QList<QStandardItem *> game;
    QTextStream textStream(&dvdRead);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        QString line = textStream.readLine();

        emit newLogEntry(line, Info);

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
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                game.append(item);
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("mtime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                game.append(item);
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("ctime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                game.append(item);
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("atime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                item = new QStandardItem();
                item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
                game.append(item);
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

    emit newDVDGameListModel();
}

void WiTools::requestWBFSGameListModel(QStandardItemModel *model, QString wbfsPath) {
    emit showStatusBarMessage(tr("Loading games..."));
    emit newLogEntry(tr("Loading games from WBFS...\n"), Info);

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

    emit newWitCommandLineLogEntry("wwt", arguments);

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

    int current = 0, max = 0;
    QStandardItem *item = 0;
    QTextStream textStream(&wbfsRead);
    textStream.setCodec("UTF-8");
    QString file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB;
    QList<QStandardItem *> ids, names, titles, regions, sizes, usedblocks, itimes, mtimes, ctimes, atimes, filetypes, wbfsslots, sources;

    while (!textStream.atEnd()) {
        QString line = textStream.readLine();

        emit newLogEntry(line, Info);

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
            item = new QStandardItem();
            item->setData(QVariant(line.section("=", 1).toInt()), Qt::DisplayRole);
            usedblocks.append(item);
            continue;
        }
        else if (line.startsWith("itime=")) {
            item = new QStandardItem();
            item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            itimes.append(item);
            continue;
        }
        else if (line.startsWith("mtime=")) {
            item = new QStandardItem();
            item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            mtimes.append(item);
            continue;
        }
        else if (line.startsWith("ctime=")) {
            item = new QStandardItem();
            item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            ctimes.append(item);
            continue;
        }
        else if (line.startsWith("atime=")) {
            item = new QStandardItem();
            item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            atimes.append(item);
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("wbfs_slot=")) {
            item = new QStandardItem();
            item->setData(QVariant(line.section("=", 1).toInt()), Qt::DisplayRole);
            wbfsslots.append(item);
            continue;
        }
        else if (line.startsWith("source=")) {
            sources.append(new QStandardItem(line.section("=", 1)));
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
    model->appendColumn(sources);

    model->setHeaderData(0, Qt::Horizontal, tr("ID (0)"));
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

    #ifdef Q_OS_MACX
        emit setProgressBarWBFS(0, max, current, "%p%");
        emit setInfoTextWBFS(QString("%1 - %2 - %3 - %4 (%5%) - %6 - %7").arg(file, usedDiscs, totalDiscs, usedMB, QString::number(current * 100 / max, 'f', 0), freeMB, totalMB));
    #else
        emit setProgressBarWBFS(0, max, current, QString("%1 - %2 - %3 - %4 (%p%) - %5 - %6").arg(file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB));
    #endif

    emit newWBFSGameListModel();
}

void WiTools::transferFilesToWBFS(QModelIndexList indexList, QString wbfsPath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));
    emit newLogEntry(tr("Preparing transfer files to WBFS.\n"), Info);

    QStringList arguments;
    arguments.append("ADD");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    foreach (QModelIndex index, indexList) {
        arguments.append(index.data().toString());
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Newer", QVariant(false)).toBool()) {
        arguments.append("--newer");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferFilesToWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferFilesToWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferFilesToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferFilesToWBFS_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("ADD")) {
        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3)));
        #endif
    }
    else if (line.contains("% copied")) {
        emit setMainProgressBar(line.left(line.indexOf("%")).remove(" ").toInt(), line);

        #ifdef Q_OS_MACX
            emit showStatusBarMessage(QString("%1%2").arg(gameCountText, line));
        #endif
    }
    else if (line.contains("already exists")) {
        emit newLogEntry(line, Error);
    }
    else if (line.contains("copied") && !line.contains("%")) {
        emit newLogEntry(line.remove(0, 5), Info);
    }
    else if (line.contains("disc added.") || line.contains("discs added.")) {
        emit newLogEntry(line, Info);
    }
}

void WiTools::transferFilesToWBFS_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferFilesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit newLogEntry(tr("Transfer successfully!"), Info);
            emit transferFilesToWBFS_finished(WiTools::Ok);
        }
        else if (exitCode == 4) {
            emit showStatusBarMessage(tr("Disc already exists!"));
            emit transferFilesToWBFS_finished(WiTools::DiscAlreadyExists);
        }
        else {
            emit newLogEntry(tr("Error %1: %2").arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
            emit transferFilesToWBFS_finished(WiTools::UnknownError);
        }
    }
    else if (exitStatus == QProcess::CrashExit){
        emit newLogEntry(tr("Transfer canceled!"), Error);
        emit showStatusBarMessage(tr("Transfer canceled!"));
        emit transferFilesToWBFS_finished(WiTools::TransferCanceled);
    }
    else {
        emit newLogEntry(tr("Error %1: %2").arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit transferFilesToWBFS_finished(WiTools::UnknownError);
    }

    delete witProcess;
}

void WiTools::transferFilesToImage(QModelIndexList indexList, QString format, QString compression, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!compression.isEmpty()) {
        emit newLogEntry(tr("Starting transfer files to image in format '%1' with compression '%2'.\n").arg(format, compression), Info);
    }
    else {
        emit newLogEntry(tr("Starting transfer files to image in format '%1'.\n").arg(format), Info);
    }

    QStringList arguments;
    arguments.append("COPY");

    foreach (QModelIndex index, indexList) {
        arguments.append(index.data().toString());
    }

    if (format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(compression));
        }
    }
    else {
        arguments.append(QString("--").append(format));
    }

    arguments.append("--dest");
    arguments.append(directory);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferFilesToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferFilesToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferFilesToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferFilesToImage_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("COPY")) {
        QString file = line.mid(line.indexOf(":") + 1, line.indexOf("->") - line.indexOf(":") - 2);
        QString from = file.mid(file.lastIndexOf("/") + 1, file.length() - file.lastIndexOf("/"));
        QString to = line.mid(line.indexOf("->") + 3, line.lastIndexOf(":") - line.indexOf("->") - 3);

        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1 -> %2...").arg(Common::fromUtf8(from), to);
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1 -> %2...").arg(Common::fromUtf8(from), to));
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
}

void WiTools::transferFilesToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferFilesToImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit showStatusBarMessage(tr("Ready."));
        emit transferFilesToImage_finished(WiTools::Ok);
    }
    else {
        emit newLogEntry(tr("Transfer canceled!"), Error);
        emit showStatusBarMessage(tr("Transfer canceled!"));
        emit transferFilesToImage_finished(WiTools::TransferCanceled);
    }

    delete witProcess;
}

void WiTools::extractImage(QModelIndexList indexList, QString destination) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing extraction..."));
    emit newLogEntry(tr("Starting image extraction.\n"), Info);

    QStringList arguments;
    arguments.append("EXTRACT");

    arguments.append(indexList.first().data().toString());

    arguments.append("--dest");
    arguments.append(destination);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::extractImage_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("EXTRACT")) {
        QString tmp = line.mid(line.indexOf(":/") + 1, line.indexOf("\n") - line.indexOf(":/")).remove("\n");
        QString source = tmp.mid(0, tmp.indexOf("->") - 1);
        QString from = source.mid(source.lastIndexOf("/") + 1, source.length());
        QString to = tmp.mid(tmp.indexOf("->") + 3, tmp.length() - tmp.indexOf("->"));

        #ifdef Q_OS_MACX
            gameCountText = tr("Extracting game %1 -> %2...").arg(Common::fromUtf8(from), Common::fromUtf8(to));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Extracting game %1 -> %2...").arg(Common::fromUtf8(from), Common::fromUtf8(to)));
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
}

void WiTools::extractImage_readyReadStandardError() {
    QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractImage_finished(WiTools::DestinationAlreadyExists);

        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        if ((exitCode == 0 && witProcess->error() == 1) || exitCode == 62097) {
            emit newLogEntry(tr("Extraction canceled!"), Error);
            emit showStatusBarMessage(tr("Extraction canceled!"));
            emit extractImage_finished(WiTools::TransferCanceled);
        }
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0 && witProcess->error() == 5) {
        if (witProcessStatus != DestinationAlreadyExists) {
            emit newLogEntry(tr("Extraction successfully!"), Error);
            emit showStatusBarMessage(tr("Ready."));
            emit extractImage_finished(WiTools::Ok);
        }

        witProcessStatus = Ok;
    }
    else {
        emit newLogEntry(QString(tr("Error %1: %2")).arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit showStatusBarMessage(tr("Extraction failed!"));
        emit extractImage_finished(WiTools::UnknownError);
    }

    delete witProcess;
}

void WiTools::transferDVDToWBFS(QString dvdPath, QString wbfsPath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));
    emit newLogEntry(tr("Starting transfer DVD to WBFS.\n"), Info);

    QStringList arguments;
    arguments.append("ADD");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append(dvdPath);

    if (WiiBaFuSettings.value("TransferToWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Newer", QVariant(false)).toBool()) {
        arguments.append("--newer");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferDVDToWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferDVDToWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferDVDToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferDVDToWBFS_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("ADD")) {
        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3)));
        #endif
    }
    else if (line.contains("% copied")) {
        emit setMainProgressBar(line.left(line.indexOf("%")).remove(" ").toInt(), line);

        #ifdef Q_OS_MACX
            emit showStatusBarMessage(QString("%1%2").arg(gameCountText, line));
        #endif
    }
    else if (line.contains("already exists")) {
        emit newLogEntry(line, Error);
    }
    else if (line.contains("copied") && !line.contains("%")) {
        emit newLogEntry(line.remove(0, 5), Info);
    }
    else if (line.contains("disc added.") || line.contains("discs added.")) {
        emit newLogEntry(line, Info);
    }
}

void WiTools::transferDVDToWBFS_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferDVDToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit newLogEntry(tr("Transfer successfully!"), Info);
            emit transferDVDToWBFS_finished(WiTools::Ok);
        }
        else if (exitCode == 4) {
            emit showStatusBarMessage(tr("Disc already exists!"));
            emit transferDVDToWBFS_finished(WiTools::DiscAlreadyExists);
        }
        else {
            emit newLogEntry(tr("Error %1: %2").arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
            emit transferDVDToWBFS_finished(WiTools::UnknownError);
        }
    }
    else if (exitStatus == QProcess::CrashExit){
        emit newLogEntry(tr("Transfer canceled!"), Error);
        emit showStatusBarMessage(tr("Transfer canceled!"));
        emit transferDVDToWBFS_finished(WiTools::TransferCanceled);
    }
    else {
        emit newLogEntry(tr("Error %1: %2").arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit transferDVDToWBFS_finished(WiTools::UnknownError);
    }

    delete witProcess;
}

void WiTools::transferDVDToImage(QString dvdPath, QString format, QString compression, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!compression.isEmpty()) {
        emit newLogEntry(tr("Starting transfer DVD to image in format '%1' with compression '%2'.\n").arg(format, compression), Info);
    }
    else {
        emit newLogEntry(tr("Starting transfer DVD to image in format '%1'.\n").arg(format), Info);
    }

    QStringList arguments;
    arguments.append("COPY");

    arguments.append(dvdPath);

    if (format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(compression));
        }
    }
    else {
        arguments.append(QString("--").append(format));
    }

    arguments.append("--dest");
    arguments.append(directory);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferDVDToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferDVDToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferDVDToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferDVDToImage_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("COPY")) {
        QString from = line.mid(line.indexOf(":/") + 1, line.indexOf("->") - line.indexOf(":/") - 2);
        QString to = line.mid(line.indexOf("->") + 3, line.lastIndexOf(":") - line.indexOf("->") - 3);

        #ifdef Q_OS_MACX
            gameCountText = tr("Transfering game %1 -> %2...").arg(from, to);
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Transfering game %1 -> %2...").arg(from, to));
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
}

void WiTools::transferDVDToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferDVDToImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit showStatusBarMessage(tr("Ready."));
        emit transferDVDToImage_finished(WiTools::Ok);
    }
    else {
        emit newLogEntry(tr("Transfer canceled!"), Error);
        emit showStatusBarMessage(tr("Transfer canceled!"));
        emit transferDVDToImage_finished(WiTools::TransferCanceled);
    }

    delete witProcess;
}

void WiTools::extractDVD(QString dvdPath, QString destination) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing extraction..."));
    emit newLogEntry(tr("Starting DVD extraction.\n"), Info);

    QStringList arguments;
    arguments.append("COPY");

    arguments.append(dvdPath);

    arguments.append("--fst");

    arguments.append("--dest");
    arguments.append(destination);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractDVD_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractDVD_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractDVD_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::extractDVD_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("EXTRACT")) {
        QString str = line.mid(line.indexOf(":/") + 1, line.indexOf("\n") - line.indexOf(":/")).remove("\n");

        #ifdef Q_OS_MACX
            gameCountText = tr("Extracting game %1...").arg(Common::fromUtf8(str));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Extracting game %1...").arg(Common::fromUtf8(str)));
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
}

void WiTools::extractDVD_readyReadStandardError() {
    QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractDVD_finished(WiTools::DestinationAlreadyExists);
        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractDVD_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        if ((exitCode == 0 && witProcess->error() == 1) || exitCode == 62097) {
            emit newLogEntry(tr("Extraction canceled!"), Error);
            emit showStatusBarMessage(tr("Extraction canceled!"));
            emit extractDVD_finished(WiTools::TransferCanceled);
        }
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0 && witProcess->error() == 5) {
        if (witProcessStatus != DestinationAlreadyExists) {
            emit newLogEntry(tr("Extraction successfully!"), Error);
            emit showStatusBarMessage(tr("Ready."));
            emit extractDVD_finished(WiTools::Ok);
        }

        witProcessStatus = Ok;
    }
    else {
        emit newLogEntry(QString(tr("Error %1: %2")).arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit showStatusBarMessage(tr("Extraction failed!"));
        emit extractDVD_finished(WiTools::UnknownError);
    }

    delete witProcess;
}

void WiTools::transferWBFSToImage(QModelIndexList indexList, QString wbfsPath, QString format, QString compression, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!compression.isEmpty()) {
        emit newLogEntry(tr("Starting transfer WBFS to image in format '%1' with compression '%2'.\n").arg(format, compression), Info);
    }
    else {
        emit newLogEntry(tr("Starting transfer WBFS to image in format '%1'.\n").arg(format), Info);
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

    foreach (QModelIndex index, indexList) {
        arguments.append(index.data().toString());
    }

    if (format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(compression));
        }
    }
    else {
        arguments.append(QString("--").append(format));
    }

    arguments.append("--dest");
    arguments.append(directory);

    if (WiiBaFuSettings.value("TransferFromWBFS/Force", QVariant(false)).toBool()) {
        arguments.append("--force");
    }

    if (WiiBaFuSettings.value("TransferFromWBFS/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferFromWBFS/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferFromWBFS/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferWBFSToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferWBFSToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferWBFSToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::transferWBFSToImage_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("EXTRACT")) {
        QString statusBarText = tr("Transfering game %1...").arg(line.mid(line.indexOf("EXTRACT") + 8, line.lastIndexOf(":") - line.indexOf("EXTRACT") - 8));

        #ifdef Q_OS_MACX
            gameCountText = statusBarText;
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(statusBarText);
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
    else if (line.contains("disc extracted.") || line.contains("discs extracted.")) {
        emit newLogEntry(line, Info);
    }
}

void WiTools::transferWBFSToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferWBFSToImage_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit showStatusBarMessage(tr("Ready."));
        emit transferWBFSToImage_finished(WiTools::Ok);
    }
    else if (exitCode == 22) {
        emit newLogEntry(tr("File already exists!"), Error);
        emit showStatusBarMessage(tr("File already exists!"));
        emit transferWBFSToImage_finished(WiTools::FileAlreadyExists);
    }
    else {
        emit newLogEntry(tr("Transfer canceled!"), Error);
        emit showStatusBarMessage(tr("Transfer canceled!"));
        emit transferWBFSToImage_finished(WiTools::TransferCanceled);
    }

    delete witProcess;
}

void WiTools::extractWBFS(QModelIndexList indexList, QString wbfsPath, QString destination) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit showStatusBarMessage(tr("Preparing extraction..."));
    emit newLogEntry(tr("Starting WBFS extraction.\n"), Info);

    QStringList arguments;
    arguments.append("EXTRACT");

    arguments.append("--part");
    arguments.append(wbfsPath);

    arguments.append(indexList.first().data().toString());

    arguments.append("--fst");

    arguments.append("--dest");
    arguments.append(destination);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    arguments.clear();
    emit setMainProgressBarVisible(false);
}

void WiTools::extractWBFS_readyReadStandardOutput() {
    QString line = witProcess->readAllStandardOutput().constData();

    if (line.contains("EXTRACT")) {
        QString tmp = line.mid(line.indexOf("EXTRACT") + 8, line.indexOf("[") - line.indexOf("EXTRACT") + 8).remove("\n");

        #ifdef Q_OS_MACX
            gameCountText = tr("Extracting game %1...").arg(Common::fromUtf8(tmp));
            emit showStatusBarMessage(gameCountText);
        #else
            emit showStatusBarMessage(tr("Extracting game %1...").arg(Common::fromUtf8(tmp)));
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
}

void WiTools::extractWBFS_readyReadStandardError() {
    QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractWBFS_finished(WiTools::DestinationAlreadyExists);
        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        if ((exitCode == 0 && witProcess->error() == 1) || exitCode == 62097) {
            emit newLogEntry(tr("Extraction canceled!"), Error);
            emit showStatusBarMessage(tr("Extraction canceled!"));
            emit extractWBFS_finished(WiTools::TransferCanceled);
        }
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0 && witProcess->error() == 5) {
        if (witProcessStatus != DestinationAlreadyExists) {
            emit newLogEntry(tr("Extraction successfully!"), Error);
            emit showStatusBarMessage(tr("Ready."));
            emit extractWBFS_finished(WiTools::Ok);
        }

        witProcessStatus = Ok;
    }
    else {
        emit newLogEntry(QString(tr("Error %1: %2")).arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit showStatusBarMessage(tr("Extraction failed!"));
        emit extractWBFS_finished(WiTools::UnknownError);
    }

    delete witProcess;
}

void WiTools::cancelTransfer() {
    witProcess->kill();
}

void WiTools::cancelLoading() {
    witProcess->kill();
}

void WiTools::removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath) {
    QStringList arguments;
    arguments.append("REMOVE");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    foreach (QModelIndex index, indexList) {
        arguments.append(index.data().toString());
    }

    if (WiiBaFuSettings.value("RemoveFromWBFS/Force", QVariant(false)).toBool())
        arguments.append("--force");

    if (WiiBaFuSettings.value("RemoveFromWBFS/Test", QVariant(false)).toBool())
        arguments.append("--test");

    emit newWitCommandLineLogEntry("wwt", arguments);

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

    emit newWitCommandLineLogEntry("wwt", arguments);

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

    emit newWitCommandLineLogEntry("wwt", arguments);

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

void WiTools::verifyGame(int index, QString wbfsPath, QString game) {
    emit newLogEntry(tr("Starting game verification...\n"), Info);

    QString witApp;
    QStringList arguments;
    arguments.append("VERIFY");

    switch (index) {
        case 0:
                witApp = wit;
                emit showStatusBarMessage(tr("Verifying game %1...").arg(game.mid(game.lastIndexOf("/") + 1, game.length() - game.lastIndexOf("/"))));
                break;
        case 1:
                witApp = wit;
                emit showStatusBarMessage(tr("Verifying game on drive %1...").arg(game));
                break;
        case 2:
                if (wbfsPath.isEmpty()) {
                    arguments.append("--auto");
                }
                else {
                    arguments.append("--part");
                    arguments.append(wbfsPath);
                }

                witApp = wwt;
                emit showStatusBarMessage(tr("Verifying game %1 on WBFS...").arg(game));
                break;
    }

    arguments.append(game);

    emit newWitCommandLineLogEntry(witApp.mid(witApp.lastIndexOf("/") + 1, witApp.length() - witApp.lastIndexOf("/")), arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(verifyGame_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(verifyGame_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(verifyGame_finished(int, QProcess::ExitStatus)));

    witProcess->start(witApp, arguments);
    witProcess->waitForFinished(-1);

    delete witProcess;
}

void WiTools::verifyGame_readyReadStandardOutput() {
    emit newLogEntry(Common::fromUtf8(witProcess->readAllStandardOutput().constData()), Info);
}

void WiTools::verifyGame_readyReadStandardError() {
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::verifyGame_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        emit newLogEntry(tr("Verification canceled!"), Error);
        emit showStatusBarMessage(tr("Verification canceled!"));
        emit verifyGame_finished(WiTools::VerificationCanceled);
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Verification successfully!"), Error);
        emit showStatusBarMessage(tr("Ready."));
        emit verifyGame_finished(WiTools::Ok);
    }
    else {
        emit newLogEntry(QString(tr("Error %1: %2")).arg(QString::number(witProcess->error()), witProcess->errorString()), Error);
        emit showStatusBarMessage(tr("Verification failed!"));
        emit verifyGame_finished(WiTools::UnknownError);
    }
}

void WiTools::cancelVerifying() {
    witProcess->kill();
}

void WiTools::setWit() {
    if (WiiBaFuSettings.value("WIT/PathToWIT", QVariant("")).toString().isEmpty()) {
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
            wit = WiiBaFuSettings.value("WIT/PathToWIT").toString().append("/wit.exe");
            wwt = WiiBaFuSettings.value("WIT/PathToWIT").toString().append("/wwt.exe");
        #else
            wit = WiiBaFuSettings.value("WIT/PathToWIT").toString().append("/wit");
            wwt = WiiBaFuSettings.value("WIT/PathToWIT").toString().append("/wwt");
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

    switch (WiiBaFuSettings.value("Main/GameLanguage", QVariant(0)).toInt()) {
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

    if (WiiBaFuSettings.value("WIT/PathToWIT", QVariant("")).toString().isEmpty()) {
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
        return WiiBaFuSettings.value("WIT/PathToWIT").toString().append(QString("/%1.txt").arg(titles));
    }
}
