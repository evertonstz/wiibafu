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
#include "wiibafu.h"

WiTools::WiTools(QObject *parent) : QObject(parent) {

}

QStandardItemModel* WiTools::getFilesGameListModel(QStandardItemModel *model, QString path) {
    emit showStatusBarMessage(tr("Loading games..."));

    QProcess filesRead;
    filesRead.start("wit", QStringList() << "LIST" << "--section" << "--recurse" << path);
    filesRead.waitForFinished();

    QByteArray bytes = filesRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    double count = 0;
    QList<QStandardItem *> ids, names, titles, regions, sizes, itimes, mtimes, ctimes, atimes, filetypes, filenames;

    foreach (QString line, lines) {
        if (line.contains("total-discs=0")) {
            emit showStatusBarMessage(tr("No games found!"));
            return model;
            break;
        }
        else if (line.isEmpty())
            continue;

        if (line.startsWith("id=")) {
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
            count = count + line.section("=", 1).toDouble();
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

    model->setHeaderData(0, Qt::Horizontal, tr("Game ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Game title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Original title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(6, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(9, Qt::Horizontal, tr("Type"));
    model->setHeaderData(10, Qt::Horizontal, tr("File name"));

    emit newFilesLabelTotalDiscs(tr("Total discs: %1").arg(ids.count()));
    emit newFilesLabelTotalSize(tr("Total size: %1 GB").arg(QString::number((count / 1073741824), 'f', 2)));
    emit showStatusBarMessage(tr("Ready."));

    return model;
}

QStandardItemModel* WiTools::getHDDGameListModel(QStandardItemModel *model) {
    emit showStatusBarMessage(tr("Loading games..."));

    QProcess filesRead;
    filesRead.start("wwt", QStringList() << "LIST-A" << "--section");
    filesRead.waitForFinished();

    QByteArray bytes = filesRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    int free, total;
    QString file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB;
    QList<QStandardItem *> ids, names, titles, regions, sizes, usedblocks, itimes, mtimes, ctimes, atimes, filetypes, containers, wbfsslots, filenames;

    foreach (QString line, lines) {
        if (line.contains("text=No WBFS found")) {
            emit showStatusBarMessage(tr("No WBFS partitions found!"));
            return model;
            break;
        }
        else if (line.contains("used_discs=0")) {
            emit showStatusBarMessage(tr("No games found!"));
            return model;
            break;
        }
        else if (line.isEmpty())
            continue;

        if (line.startsWith("file=")) {
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
            usedMB = tr("Used MB: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("free_mib=")) {
            freeMB = tr("Free MB: %1").arg(line.section("=", 1));
            free = line.section("=", 1).toInt();
            continue;
        }
        else if (line.startsWith("total_mib=")) {
            totalMB = tr("Total MB: %1").arg(line.section("=", 1));
            total = line.section("=", 1).toInt();
            continue;
        }

        if (line.startsWith("id=")) {
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
            itimes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("ctime=")) {
            ctimes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("atime=")) {
            atimes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("container=")) {
            containers.append(new QStandardItem(line.section("=", 1)));
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
    model->appendColumn(containers);
    model->appendColumn(wbfsslots);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, tr("Game ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Game title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Original title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Used blocks"));
    model->setHeaderData(6, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(9, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(10, Qt::Horizontal, tr("Type"));
    model->setHeaderData(11, Qt::Horizontal, tr("Container"));
    model->setHeaderData(12, Qt::Horizontal, tr("WBFS slot"));
    model->setHeaderData(13, Qt::Horizontal, tr("File/Partition"));

    // total - free = bug fix for wwts 'used_mib'
    emit setProgressBarHDD(0, total, total - free, QString("%1 - %2 - %3 - %4 (%p%) - %5 - %6").arg(file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB));
    emit showStatusBarMessage(tr("Ready."));

    return model;
}
