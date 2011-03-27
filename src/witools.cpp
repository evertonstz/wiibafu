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

#include "witools.h"
#include "common.h"

WiTools::WiTools(QObject *parent) : QObject(parent) {
    setWit();
}

void WiTools::requestFilesGameListModel(QStandardItemModel *model, const QString path, const int recurseDepth) {
    emit showStatusBarMessage(tr("Loading games..."));
    emit newLogEntry(tr("Loading games from images...\n"), Info);

    witModel = model;
    totalGameSize = 0;

    n_scanned = "0";
    n_directories = "0";
    n_found = "0";

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(requestFilesGameListModel_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(requestFilesGameListModel_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(requestFilesGameListModel_finished(int, QProcess::ExitStatus)));

    const QStringList arguments = QStringList() << "LIST-LLL" << "--titles" << witTitlesPath() << "--recurse" << path << "--rdepth" << QString::number(recurseDepth) << "--section" << "--verbose" << "--progress";
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
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty() || line.contains("progress:scan")  || line.contains("found-name")) {
            continue;
        }

        emit showStatusBarMessage(tr("%1 file(s) scanned, %2 directory(ies) and %3 game(s) found.").arg(n_scanned, n_directories, n_found));
        emit newLogEntry(line, Info);

        if (line.contains("n-scanned=")) {
            n_scanned = line.section("=", 1);
            continue;
        }
        else if (line.contains("n-directories=")) {
            n_directories = line.section("=", 1);
            continue;
        }
        else if (line.contains("n-found=")) {
            n_found = line.section("=", 1);
            continue;
        }
        else if (line.contains("total-discs=0")) {
            emit showStatusBarMessage(tr("No games found!"));
            emit loadingGamesFailed(NoGamesFound);
            return;
        }
        else if (line.contains("total-size=")) {
            totalGameSize = line.section("=", 1).toDouble();
            continue;
        }
        else if (line.startsWith("id=")) {
            if (line.section("=", 1).left(1) == "G") {
                fgl_ids.append(new QStandardItem(QIcon(":/images/gamecube.png"), line.section("=", 1)));
            }
            else {
                fgl_ids.append(new QStandardItem(QIcon(":/images/wii.png"), line.section("=", 1)));
            }

            continue;
        }
        else if (line.startsWith("name=")) {
            fgl_names.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            fgl_titles.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            fgl_regions.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            fgl_sizes.append(new QStandardItem(Common::calculateSize(line.section("=", 1))));
            continue;
        }
        else if (line.startsWith("itime=")) {
            fgl_item = new QStandardItem();
            fgl_item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            fgl_itimes.append(fgl_item);
            continue;
        }
        else if (line.startsWith("mtime=")) {
            fgl_item = new QStandardItem();
            fgl_item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            fgl_mtimes.append(fgl_item);
            continue;
        }
        else if (line.startsWith("ctime=")) {
            fgl_item = new QStandardItem();
            fgl_item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            fgl_ctimes.append(fgl_item);
            continue;
        }
        else if (line.startsWith("atime=")) {
            fgl_item = new QStandardItem();
            fgl_item->setData(QVariant(QDateTime::fromString(line.section("=", 1).section(" ", 1), "yyyy-MM-dd hh:mm:ss")), Qt::DisplayRole);
            fgl_atimes.append(fgl_item);
            continue;
        }
        else if (line.startsWith("filetype=")) {
            fgl_filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            fgl_filenames.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }
}

void WiTools::requestFilesGameListModel_readyReadStandardError() {
    emit showStatusBarMessage(tr("Loading games failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::requestFilesGameListModel_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        emit newLogEntry(tr("Loading games canceled!"), Info);
        emit loadingGamesCanceled();
    }
    else if (exitCode != 0) {
        emit newLogEntry(tr("Loading failed!"), Error);
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        #ifdef Q_OS_WIN32
            newLogEntry("\r\n", Info);
        #else
            newLogEntry("\n", Info);
        #endif

        emit newLogEntry(tr("%1 file(s) scanned, %2 directory(ies) and %3 game(s) found.\n").arg(n_scanned, n_directories, n_found), Info);

        witModel->clear();
        witModel->appendColumn(fgl_ids);
        witModel->appendColumn(fgl_names);
        witModel->appendColumn(fgl_titles);
        witModel->appendColumn(fgl_regions);
        witModel->appendColumn(fgl_sizes);
        witModel->appendColumn(fgl_itimes);
        witModel->appendColumn(fgl_mtimes);
        witModel->appendColumn(fgl_ctimes);
        witModel->appendColumn(fgl_atimes);
        witModel->appendColumn(fgl_filetypes);
        witModel->appendColumn(fgl_filenames);

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

    fgl_ids.clear();
    fgl_names.clear();
    fgl_titles.clear();
    fgl_regions.clear();
    fgl_sizes.clear();
    fgl_itimes.clear();
    fgl_mtimes.clear();
    fgl_ctimes.clear();
    fgl_atimes.clear();
    fgl_filetypes.clear();
    fgl_filenames.clear();

    delete witProcess;
}

void WiTools::requestDVDGameListModel(QStandardItemModel *model, const QString path) {
    emit showStatusBarMessage(tr("Loading disc..."));
    emit newLogEntry(tr("Loading disc...\n"), Info);

    QProcess dvdRead;

    const QStringList arguments = QStringList() << "LIST-LL" << path << "--titles" << witTitlesPath() << "--section";
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
        const QString line = textStream.readLine();

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
            game.append(new QStandardItem(Common::calculateSize(line.section("=", 1))));
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

void WiTools::requestWBFSGameListModel(QStandardItemModel *model, const QString wbfsPath) {
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
        const QString line = textStream.readLine();

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
            if (line.section("=", 1).left(1) == "G") {
                ids.append(new QStandardItem(QIcon(":/images/gamecube.png"), line.section("=", 1)));
            }
            else {
                ids.append(new QStandardItem(QIcon(":/images/wii.png"), line.section("=", 1)));
            }

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
            sizes.append(new QStandardItem(Common::calculateSize(line.section("=", 1))));
            continue;
        }
        else if (line.startsWith("used-blocks=")) {
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

void WiTools::transferFilesToWBFS(const QModelIndexList indexList, const QString wbfsPath, const WiTools::GamePatchParameters parameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (parameters.Patch) {
        emit newLogEntry(tr("Preparing transfer files to WBFS with patching.\n"), Info);
    }
    else {
        emit newLogEntry(tr("Preparing transfer files to WBFS.\n"), Info);
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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (parameters.Patch) {
        if (!parameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(parameters.ID);
        }

        if (!parameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(parameters.Name);
        }

        arguments.append("--region");
        arguments.append(parameters.Region);

        if (!parameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(parameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(parameters.Modify);

        arguments.append("--enc");
        arguments.append(parameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(parameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferFilesToWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferFilesToWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferFilesToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferFilesToWBFS_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("job-counter=")) {
            job_counter = line.section("=", 1);
            continue;
        }
        else if (line.contains("job-total=")) {
            job_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("source-path=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Transfering game [%1/%2] %3 -> WBFS...").arg(job_counter, job_total, source_path.mid(source_path.lastIndexOf("/") + 1, source_path.length() - source_path.lastIndexOf("/"))));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) copied in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::transferFilesToWBFS_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferFilesToWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit newLogEntry(tr("Transfer successfully!"), Info);
            emit newLogEntry(tr("%1 MiB in %2 copied (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::transferFilesToImage(WiTools::TransferFilesToImageParameters transferParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!transferParameters.Compression.isEmpty()) {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer files to image in format '%1' with compression '%2' and patching.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer files to image in format '%1' with compression '%2'.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
    }
    else {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer files to image in format '%1' with patching.\n").arg(transferParameters.Format), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer files to image in format '%1'.\n").arg(transferParameters.Format), Info);
        }
    }

    QStringList arguments;
    arguments.append("COPY");

    foreach (QModelIndex index, transferParameters.IndexList) {
        arguments.append(index.data().toString());
    }

    if (transferParameters.Format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (transferParameters.Compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(transferParameters.Compression));
        }
    }
    else {
        arguments.append(QString("--").append(transferParameters.Format));
    }

    arguments.append("--dest");
    arguments.append(transferParameters.Directory);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Diff", QVariant(false)).toBool()) {
        arguments.append("--diff");
    }

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (!transferParameters.SplitSize.isEmpty()) {
        arguments.append("--split");
        arguments.append("--split-size");
        arguments.append(transferParameters.SplitSize);
    }

    if (transferParameters.PatchParameters.Patch) {
        if (!transferParameters.PatchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(transferParameters.PatchParameters.ID);
        }

        if (!transferParameters.PatchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(transferParameters.PatchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(transferParameters.PatchParameters.Region);

        if (!transferParameters.PatchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(transferParameters.PatchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(transferParameters.PatchParameters.Modify);

        arguments.append("--enc");
        arguments.append(transferParameters.PatchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(transferParameters.PatchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--long");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferFilesToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferFilesToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferFilesToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferFilesToImage_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("job-counter=")) {
            job_counter = line.section("=", 1);
            continue;
        }
        else if (line.contains("job-total=")) {
            job_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("source-path=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Transfering game [%1/%2] %3 -> %4...").arg(job_counter, job_total, source_path.mid(source_path.lastIndexOf("/") + 1, source_path.length() - source_path.lastIndexOf("/")), dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) copied in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::transferFilesToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferFilesToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit newLogEntry(tr("%1 MiB in %2 copied (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::extractImage(const QModelIndexList indexList, const QString destination, const WiTools::GamePatchParameters patchParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing extraction..."));

    if (patchParameters.Patch) {
        emit newLogEntry(tr("Starting image extraction with patching.\n"), Info);
    }
    else {
        emit newLogEntry(tr("Starting image extraction.\n"), Info);
    }

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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (patchParameters.Patch) {
        if (!patchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(patchParameters.ID);
        }

        if (!patchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(patchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(patchParameters.Region);

        if (!patchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(patchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(patchParameters.Modify);

        arguments.append("--enc");
        arguments.append(patchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(patchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::extractImage_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("source-path=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Extracting game %1 -> %2...").arg(source_path.mid(source_path.lastIndexOf("/") + 1, source_path.length() - source_path.lastIndexOf("/")), dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) extracted in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::extractImage_readyReadStandardError() {
    const QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractImage_finished(WiTools::DestinationAlreadyExists);

        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        if ((exitCode == 0 && witProcess->error() == 1) || exitCode == 62097) {
            emit newLogEntry(tr("Extraction canceled!"), Error);
            emit showStatusBarMessage(tr("Extraction canceled!"));
            emit extractImage_finished(WiTools::TransferCanceled);
        }
    }
    else if (exitStatus == QProcess::NormalExit && exitCode == 0 && witProcess->error() == 5) {
        if (witProcessStatus != DestinationAlreadyExists) {
            emit newLogEntry(tr("%1 MiB in %2 extracted (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::transferDVDToWBFS(const QString dvdPath, const QString wbfsPath, const WiTools::GamePatchParameters patchParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (patchParameters.Patch) {
        emit newLogEntry(tr("Starting transfer DVD to WBFS with patching.\n"), Info);
    }
    else {
        emit newLogEntry(tr("Starting transfer DVD to WBFS.\n"), Info);
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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (patchParameters.Patch) {
        if (!patchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(patchParameters.ID);
        }

        if (!patchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(patchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(patchParameters.Region);

        if (!patchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(patchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(patchParameters.Modify);

        arguments.append("--enc");
        arguments.append(patchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(patchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferDVDToWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferDVDToWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferDVDToWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferDVDToWBFS_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("id=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Transfering game %1 -> WBFS...").arg(source_path));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) copied in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::transferDVDToWBFS_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferDVDToWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit newLogEntry(tr("Transfer successfully!"), Info);
            emit newLogEntry(tr("%1 MiB in %2 copied (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
            emit transferDVDToWBFS_finished(WiTools::Ok);
        }
        else if (exitCode == 4) {
            emit showStatusBarMessage(tr("Disc already exists!"));
            emit newLogEntry(tr("Disc already exists!"), Error);
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

void WiTools::transferDVDToImage(const QString dvdPath, const WiTools::TransferFilesToImageParameters transferParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!transferParameters.Compression.isEmpty()) {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer DVD to image in format '%1' with compression '%2' and patching.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer DVD to image in format '%1' with compression '%2'.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
    }
    else {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer DVD to image in format '%1' with patching.\n").arg(transferParameters.Format), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer DVD to image in format '%1'.\n").arg(transferParameters.Format), Info);
        }
    }

    QStringList arguments;
    arguments.append("COPY");

    arguments.append(dvdPath);

    if (transferParameters.Format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (transferParameters.Compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(transferParameters.Compression));
        }
    }
    else {
        arguments.append(QString("--").append(transferParameters.Format));
    }

    arguments.append("--dest");
    arguments.append(transferParameters.Directory);

    if (WiiBaFuSettings.value("TransferToImageFST/Test", QVariant(false)).toBool()) {
        arguments.append("--test");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Update", QVariant(false)).toBool()) {
        arguments.append("--update");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Overwrite", QVariant(false)).toBool()) {
        arguments.append("--overwrite");
    }

    if (WiiBaFuSettings.value("TransferToImageFST/Diff", QVariant(false)).toBool()) {
        arguments.append("--diff");
    }

    if (!transferParameters.SplitSize.isEmpty()) {
        arguments.append("--split");
        arguments.append("--split-size");
        arguments.append(transferParameters.SplitSize);
    }

    if (transferParameters.PatchParameters.Patch) {
        if (!transferParameters.PatchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(transferParameters.PatchParameters.ID);
        }

        if (!transferParameters.PatchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(transferParameters.PatchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(transferParameters.PatchParameters.Region);

        if (!transferParameters.PatchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(transferParameters.PatchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(transferParameters.PatchParameters.Modify);

        arguments.append("--enc");
        arguments.append(transferParameters.PatchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(transferParameters.PatchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferDVDToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferDVDToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferDVDToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferDVDToImage_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("source-path=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Transfering game %1 -> %2...").arg(source_path, dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) copied in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::transferDVDToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferDVDToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit newLogEntry(tr("%1 MiB in %2 copied (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::extractDVD(const QString dvdPath, const QString destination, const WiTools::GamePatchParameters patchParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing extraction..."));

    if (patchParameters.Patch) {
        emit newLogEntry(tr("Starting DVD extraction with patching.\n"), Info);
    }
    else {
        emit newLogEntry(tr("Starting DVD extraction.\n"), Info);
    }

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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (patchParameters.Patch) {
        if (!patchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(patchParameters.ID);
        }

        if (!patchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(patchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(patchParameters.Region);

        if (!patchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(patchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(patchParameters.Modify);

        arguments.append("--enc");
        arguments.append(patchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(patchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractDVD_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractDVD_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractDVD_finished(int, QProcess::ExitStatus)));

    witProcess->start(wit, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::extractDVD_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("source-path=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Extracting game %1 -> %2...").arg(source_path, dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) extracted in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::extractDVD_readyReadStandardError() {
    const QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractDVD_finished(WiTools::DestinationAlreadyExists);
        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractDVD_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
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
            emit newLogEntry(tr("%1 MiB in %2 extracted (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::transferWBFSToImage(const QString wbfsPath, const WiTools::TransferFilesToImageParameters transferParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing transfer..."));

    if (!transferParameters.Compression.isEmpty()) {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer WBFS to image in format '%1' with compression '%2' and patching.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer WBFS to image in format '%1' with compression '%2'.\n").arg(transferParameters.Format, transferParameters.Compression), Info);
        }
    }
    else {
        if (transferParameters.PatchParameters.Patch) {
            emit newLogEntry(tr("Starting transfer WBFS to image in format '%1' with patching.\n").arg(transferParameters.Format), Info);
        }
        else {
            emit newLogEntry(tr("Starting transfer WBFS to image in format '%1'.\n").arg(transferParameters.Format), Info);
        }
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

    foreach (QModelIndex index, transferParameters.IndexList) {
        arguments.append(index.data().toString());
    }

    if (transferParameters.Format.contains("wia")) {
        QString tmpstr = "--wia=";

        if (transferParameters.Compression.isEmpty()) {
            arguments.append(tmpstr.append("DEFAULT"));
        }
        else {
            arguments.append(tmpstr.append(transferParameters.Compression));
        }
    }
    else {
        arguments.append(QString("--").append(transferParameters.Format));
    }

    arguments.append("--dest");
    arguments.append(transferParameters.Directory);

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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (!transferParameters.SplitSize.isEmpty()) {
        arguments.append("--split");
        arguments.append("--split-size");
        arguments.append(transferParameters.SplitSize);
    }

    if (transferParameters.PatchParameters.Patch) {
        if (!transferParameters.PatchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(transferParameters.PatchParameters.ID);
        }

        if (!transferParameters.PatchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(transferParameters.PatchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(transferParameters.PatchParameters.Region);

        if (!transferParameters.PatchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(transferParameters.PatchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(transferParameters.PatchParameters.Modify);

        arguments.append("--enc");
        arguments.append(transferParameters.PatchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(transferParameters.PatchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferWBFSToImage_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferWBFSToImage_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferWBFSToImage_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferWBFSToImage_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("job-counter=")) {
            job_counter = line.section("=", 1);
            continue;
        }
        else if (line.contains("job-total=")) {
            job_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("id=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Transfering game [%1/%2] %3 -> %4...").arg(job_counter, job_total, source_path.mid(source_path.lastIndexOf("/") + 1, source_path.length() - source_path.lastIndexOf("/")), dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) copied in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::transferWBFSToImage_readyReadStandardError() {
    emit showStatusBarMessage(tr("Transfer failed!"));
    emit newLogEntry(witProcess->readAllStandardError().constData(), Error);
}

void WiTools::transferWBFSToImage_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit newLogEntry(tr("Transfer successfully!"), Info);
        emit newLogEntry(tr("%1 MiB in %2 copied (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::extractWBFS(const QModelIndexList indexList, const QString wbfsPath, const QString destination, const WiTools::GamePatchParameters patchParameters) {
    resetProgressBarVariables();

    emit setMainProgressBar(0, "%p%");
    emit setMainProgressBarVisible(true);
    emit showStatusBarMessage(tr("Preparing extraction..."));

    if (patchParameters.Patch) {
        emit newLogEntry(tr("Starting WBFS extraction with patching.\n"), Info);
    }
    else {
        emit newLogEntry(tr("Starting WBFS extraction.\n"), Info);
    }

    QStringList arguments;
    arguments.append("EXTRACT");

    arguments.append("--part");
    arguments.append(wbfsPath);

    foreach (QModelIndex index, indexList) {
        arguments.append(index.data().toString());
    }

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

    arguments.append("--psel");

    if (WiiBaFuSettings.value("Scrubbing/Raw", QVariant(false)).toBool()) {
        arguments.append("RAW");
    }
    else {
        QString pselModes;

        if (WiiBaFuSettings.value("Scrubbing/Data", QVariant(true)).toBool()) {
            pselModes.append("DATA,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Update", QVariant(true)).toBool()) {
            pselModes.append("UPDATE,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Channel", QVariant(true)).toBool()) {
            pselModes.append("CHANNEL,");
        }
        if (WiiBaFuSettings.value("Scrubbing/Whole", QVariant(false)).toBool()) {
            pselModes.append("WHOLE,");
        }

        if (pselModes.endsWith(",")) {
            pselModes.remove(pselModes.length() - 1, 1);
        }

        if (pselModes.isEmpty()) {
            pselModes.append("ALL");
        }

        arguments.append(pselModes);
    }

    if (patchParameters.Patch) {
        if (!patchParameters.ID.isEmpty()) {
            arguments.append("--id");
            arguments.append(patchParameters.ID);
        }

        if (!patchParameters.Name.isEmpty()) {
            arguments.append("--name");
            arguments.append(patchParameters.Name);
        }

        arguments.append("--region");
        arguments.append(patchParameters.Region);

        if (!patchParameters.IOS.isEmpty()) {
            arguments.append("--ios");
            arguments.append(patchParameters.IOS);
        }

        arguments.append("--modify");
        arguments.append(patchParameters.Modify);

        arguments.append("--enc");
        arguments.append(patchParameters.EncodingMode);

        arguments.append("--common-key");
        arguments.append(patchParameters.CommonKey);
    }

    arguments.append("--section");
    arguments.append("--progress");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(witProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(extractWBFS_readyReadStandardOutput()));
    connect(witProcess, SIGNAL(readyReadStandardError()), this, SLOT(extractWBFS_readyReadStandardError()));
    connect(witProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractWBFS_finished(int, QProcess::ExitStatus)));

    witProcess->start(wwt, arguments);
    witProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::extractWBFS_readyReadStandardOutput() {
    QTextStream textStream(witProcess);
    textStream.setCodec("UTF-8");

    while (!textStream.atEnd()) {
        const QString line = textStream.readLine();

        if (line.isEmpty()) {
            continue;
        }
        else if (line.contains("id=")) {
            source_path = line.section("=", 1);
            continue;
        }
        else if (line.contains("dest-type=")) {
            dest_type = line.section("=", 1);
            continue;
        }
        else if (line.contains("percent=")) {
            percent = line.section("=", 1);
            continue;
        }
        else if (line.contains("elapsed-text=")) {
            elapsed_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("eta-text=")) {
            eta_text = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-total=")) {
            mib_total = line.section("=", 1);
            continue;
        }
        else if (line.contains("mib-per-sec=")) {
            mib_per_sec = line.section("=", 1);
            continue;
        }
    }

    emit showStatusBarMessage(tr("Extracting game %1 -> %2...").arg(source_path.mid(source_path.lastIndexOf("/") + 1, source_path.length() - source_path.lastIndexOf("/")), dest_type));
    emit setMainProgressBar(percent.toInt(), tr("%p% (%1 MiB) extracted in %2 (%3 MiB/sec) -> ETA %4").arg(mib_total, elapsed_text, mib_per_sec, eta_text));
}

void WiTools::extractWBFS_readyReadStandardError() {
    const QString error = witProcess->readAllStandardError().constData();

    emit newLogEntry(error, Error);

    if (error.contains("Destination already exists")) {
        emit showStatusBarMessage(tr("Destination already exists!"));
        emit extractWBFS_finished(WiTools::DestinationAlreadyExists);
        witProcessStatus = DestinationAlreadyExists;
    }
}

void WiTools::extractWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
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
            emit newLogEntry(tr("%1 MiB in %2 extracted (%3 MiB/sec)").arg(mib_total, elapsed_text, mib_per_sec), Info);
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

void WiTools::removeGamesFromWBFS(const QModelIndexList indexList, const QString wbfsPath) {
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

    delete witProcess;
}

void WiTools::removeGamesFromWBFS_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
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

void WiTools::checkWBFS(const QString wbfsPath) {
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

    emit newLogEntry(QString(witProcess->readAll().constData()), Info);

    delete witProcess;
}

void WiTools::dumpWBFS(const QString wbfsPath) {
    QStringList arguments;
    arguments.append("DUMP");

    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }

    arguments.append("--all");
    arguments.append("--inode");
    arguments.append("-lll");

    emit newWitCommandLineLogEntry("wwt", arguments);

    witProcess = new QProcess();
    witProcess->start(wwt, arguments);

    if (!witProcess->waitForFinished(-1)) {
        if (witProcess->errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms WBFS Tool not found!"));
            emit newLogEntry(tr("Wiimms WBFS Tool not found!"), Error);
        }
        else {
            emit showStatusBarMessage(tr("WBFS dump failed!"));
            emit newLogEntry(tr("WBFS dump failed! (status: %1, code: %2,  %3)").arg(QString::number(witProcess->exitStatus()), QString::number(witProcess->exitCode()), witProcess->errorString()), Error);
        }
    }
    else {
        emit showStatusBarMessage(tr("WBFS dump successfully!"));
    }

    emit newLogEntry(QString(witProcess->readAll().constData()), Info);

    delete witProcess;
}

void WiTools::createWBFS(const CreateWBFSParameters parameters) {
    emit showStatusBarMessage(tr("Creating WBFS..."));

    QStringList arguments;
    arguments.append("FORMAT");
    arguments.append(parameters.Path);

    if (!parameters.Size.isEmpty()) {
        arguments.append("--size");
        arguments.append(parameters.Size);
    }

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

    emit newLogEntry(QString(witProcess->readAll().constData()), Info);

    delete witProcess;

    emit stopBusy();
}

void WiTools::verifyGame(const int index, const QString wbfsPath, const QString game) {
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

void WiTools::verifyGame_finished(const int exitCode, const QProcess::ExitStatus exitStatus) {
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

void WiTools::patchGameImage(const QString filePath, const WiTools::GamePatchParameters parameters) {
    emit newLogEntry(tr("Starting game patching...\n"), Info);

    QStringList arguments;
    arguments.append("EDIT");

    arguments.append(filePath);

    if (!parameters.ID.isEmpty()) {
        arguments.append("--id");
        arguments.append(parameters.ID);
    }

    if (!parameters.Name.isEmpty()) {
        arguments.append("--name");
        arguments.append(parameters.Name);
    }

    arguments.append("--region");
    arguments.append(parameters.Region);

    if (!parameters.IOS.isEmpty()) {
        arguments.append("--ios");
        arguments.append(parameters.IOS);
    }

    arguments.append("--modify");
    arguments.append(parameters.Modify);

    arguments.append("--enc");
    arguments.append(parameters.EncodingMode);

    arguments.append("--common-key");
    arguments.append(parameters.CommonKey);

    emit newWitCommandLineLogEntry("wit", arguments);

    witProcess = new QProcess();
    witProcess->start(wit, arguments);

    if (!witProcess->waitForFinished(-1)) {
        if (witProcess->errorString().contains("No such file or directory")) {
            emit showStatusBarMessage(tr("Wiimms ISO Tool not found!"));
            emit newLogEntry(tr("Wiimms ISO Tool not found!"), Error);
            emit patchGameImage_finished(WiTools::NoSuchFileOrDirectory);
        }
        else {
            emit showStatusBarMessage(tr("Patch game image failed!"));
            emit newLogEntry(tr("Patch game image failed! (status: %1, code: %2,  %3)").arg(QString::number(witProcess->exitStatus()), QString::number(witProcess->exitCode()), witProcess->errorString()), Error);
            emit patchGameImage_finished(WiTools::UnknownError);
        }
    }
    else {
        emit showStatusBarMessage(tr("Patch game image successfully!"));
        emit newLogEntry(tr("Patch game image successfully!"), Info);
        emit patchGameImage_finished(WiTools::Ok);
    }

    delete witProcess;
}

void WiTools::setWit() {
    QDir::setSearchPaths("wit", QStringList() << QDir::currentPath().append("/wit") << "/usr/local/share/wit" << WiiBaFuSettings.value("WIT/PathToWIT", QVariant(DEFAULT_WIT_PATH)).toString() << QDir::currentPath().remove("MacOS").append("wit") << QDir::currentPath().append("/Wii Backup Fusion.app/Contents/wit") << QString(getenv("PATH")).split(":"));

    #ifdef Q_OS_WIN32
        wit = QFile("wit:wit.exe").fileName();
        wwt = QFile("wit:wwt.exe").fileName();
    #else
        wit = QFile("wit:wit").fileName();
        wwt = QFile("wit:wwt").fileName();
    #endif
}

void WiTools::resetProgressBarVariables() {
    percent = "0";
    elapsed_text = "0:00";
    eta_text = "0:00";
    mib_total = "0";
    mib_per_sec = "0";
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
        case 10: titles = "titles-no";
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

    QDir::setSearchPaths("witTitles", QStringList() << QDir::currentPath().append("/wit") << "/usr/local/share/wit" << WiiBaFuSettings.value("WIT/PathToTitles", QVariant(DEFAULT_TITLES_PATH)).toString() << QDir::currentPath().remove("MacOS").append("wit") << QDir::currentPath().append("/Wii Backup Fusion.app/Contents/wit") << QString(getenv("PATH")).split(":") << QString(getenv("WIT-TITLES")).split(":"));

    return QFile(QString("witTitles:%1.txt").arg(titles)).fileName();
}
