/***************************************************************************
 *   Copyright (C) 2010 DynUp                                              *
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
    QProcess filesRead;
    filesRead.start("wit", QStringList() << "LIST" << "--section" << "--recurse" << path);
    filesRead.waitForFinished();

    QByteArray bytes = filesRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    QList<QStandardItem *> ids, names, titles, regions, sizes, mtimes, filetypes, filenames;

    foreach (QString line, lines) {
        if (line.isEmpty())
            continue;

        if (line.startsWith("id=")) {
            ids.append(new QStandardItem(line.section("=", 1)));
        }
        else if (line.startsWith("name=")) {
            names.append(new QStandardItem(line.section("=", 1)));
        }
        else if (line.startsWith("title=")) {
            titles.append(new QStandardItem(line.section("=", 1)));
        }
        else if (line.startsWith("region=")) {
            regions.append(new QStandardItem(line.section("=", 1)));
        }
        else if (line.startsWith("size=")) {
            sizes.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
        }
        else if (line.startsWith("filename=")) {
            filenames.append(new QStandardItem(line.section("=", 1)));
        }
    }

    model->appendColumn(ids);
    model->appendColumn(names);
    model->appendColumn(titles);
    model->appendColumn(regions);
    model->appendColumn(sizes);
    model->appendColumn(mtimes);
    model->appendColumn(filetypes);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Game ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Game title"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Original title"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Type"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("File name"));

    return model;
}
