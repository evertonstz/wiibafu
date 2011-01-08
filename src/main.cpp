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

#include <QtGui/QApplication>
#include "wiibafu.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationDomain("wiibafu.codeplex.com");
    QCoreApplication::setOrganizationName("Kai Heitkamp");
    QCoreApplication::setApplicationName("Wii Backup Fusion");
    QCoreApplication::setApplicationVersion("1.1");

    QDir::setSearchPaths("TranslationPath", QStringList() << QDir::currentPath() << QDir::currentPath().append("/locale") << QDir::homePath().append("/WiiBaFu/resources/locale") << QDir::currentPath().append("/Wii Backup Fusion.app/Contents/Resources"));
    QString file = QFile("TranslationPath:german.qm").fileName();
    QString path = file.mid(0, file.lastIndexOf("/"));

    QTranslator translator;
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    translator.load(QLocale::system().languageToString(QLocale::system().language()).toLower(), path);
    app.installTranslator(&translator);

    WiiBaFu wiibafu;
    wiibafu.show();

    return app.exec();
}
