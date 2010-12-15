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

#include "common.h"

Common::Common(QObject *parent) : QObject(parent) {
    wiTools = new WiTools(this);
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(getCover_timeOut()));

    wiiTDBLanguages = QStringList() << "EN" << "FR" << "DE" << "ES" << "IT" << "NL" << "PT" << "SE" << "DK" << "NO" << "FI" << "RU" << "JA" << "KO" << "ZHTW" << "ZHCN";
    titlesExtensions = QStringList() << ".txt"  << "-fr.txt" << "-de.txt" << "-es.txt" << "-it.txt" << "-nl.txt" << "-pt.txt" << "-se.txt" << "-dk.txt" << "-no.txt" << "-fi.txt" << "-ru.txt" << "-ja.txt" << "-ko.txt" << "-zhtw.txt" << "-zhcn.txt";
}

void Common::requestGameCover(QString gameID, QString language, GameCoverArt gameCoverArt) {
    emit showStatusBarMessage(tr("Loading game cover..."));
    QNetworkReply::NetworkError result;

    if (gameCoverArt == Disc) {
        result = getCover(QString("http://wiitdb.com/wiitdb/artwork/disc/%1/%2.png").arg(language, gameID));

        if (result != QNetworkReply::NoError) {
            result = getCover(QString("http://www.wiiboxart.com/artwork/disc/%1.png").arg(gameID));
        }

        if (result != QNetworkReply::NoError) {
            emit showStatusBarMessage(tr("No game cover available!"));
        }
        else {
            emit showStatusBarMessage(tr("Ready."));
            emit newGameDiscCover(cover);
        }
    }
    else if (gameCoverArt == ThreeD) {
        result = getCover(QString("http://wiitdb.com/wiitdb/artwork/cover3D/%1/%2.png").arg(language, gameID));

        if (result != QNetworkReply::NoError) {
            result = getCover(QString("http://www.wiiboxart.com/artwork/cover3D/%1.png").arg(gameID));
        }

        if (result != QNetworkReply::NoError) {
            emit showStatusBarMessage(tr("No game cover available!"));
        }
        else {
            emit showStatusBarMessage(tr("Ready."));
            emit newGame3DCover(cover);
        }
    }
    else if (gameCoverArt == HighQuality) {
        result = getCover(QString("http://wiitdb.com/wiitdb/artwork/coverfullHQ/%1/%2.png").arg(language, gameID));

        if (result != QNetworkReply::NoError) {
            result = getCover(QString("http://www.wiiboxart.com/artwork/coverfull/%1.png").arg(gameID));
        }

        if (result != QNetworkReply::NoError) {
            emit showStatusBarMessage(tr("No game cover available!"));
        }
        else {
            emit showStatusBarMessage(tr("Ready."));
            emit newGameFullHQCover(cover);
        }
    }
}

QNetworkReply::NetworkError Common::getCover(QString url) {
    QEventLoop loop;
    QNetworkReply *reply;
    QNetworkAccessManager manager;

    timer->start(3000);

    emit setMainProgressBar(0, "%p%");
    emit newLogEntry(tr("Loading game cover..."), WiTools::Info);

    manager.setProxy(proxy());
    reply = manager.get(QNetworkRequest(QUrl(url)));

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() != 0) {
        emit newLogEntry(tr("Not found! (%1)").arg(url), WiTools::Error);
    }
    else {
        emit newLogEntry(tr("Done! (%1)").arg(url), WiTools::Info);
        cover = QImage::fromData(reply->readAll());
    }

    if (timer->isActive()) {
        timer->stop();
    }

    emit setMainProgressBarVisible(false);

    return reply->error();
}

void Common::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    emit setMainProgressBar(bytesReceived * 100 / bytesTotal, "%p%");
}

void Common::getCover_timeOut() {
    emit setMainProgressBarVisible(true);
}

void Common::updateTitles() {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");


    bool error = false;
    QString wiiTDBUrl = "http://wiitdb.com/titles.txt?LANG=";
    QString titlesPath = wiTools->witTitlesPath();
    QString fileName = titlesPath.left(titlesPath.lastIndexOf("titles") + 6);

    for (int i = 0; i < 16; i++) {
        #ifdef Q_OS_MACX
            emit showStatusBarMessage(tr("Downloading titles%1... (%2%)").arg(titlesExtensions.at(i), QString::number(i * 100 / 16)));
        #else
            emit showStatusBarMessage(tr("Downloading titles%1...").arg(titlesExtensions.at(i)));
        #endif

        emit newLogEntry(tr("Downloading titles%1...").arg(titlesExtensions.at(i)), WiTools::Info);

        if (getTitle(QString(wiiTDBUrl).append(wiiTDBLanguages.at(i)), QString(fileName).append(titlesExtensions.at(i))) != QNetworkReply::NoError) {
            emit newLogEntry(tr("Download of titles%1 failed!").arg(titlesExtensions.at(i)), WiTools::Error);
            error = true;
        }
        else {
            emit newLogEntry(tr("Download titles%1 successfully!").arg(titlesExtensions.at(i)), WiTools::Info);
        }

        emit setMainProgressBar((i + 1) * 100 / 16, "%p%");
    }

    if (error) {
        emit showStatusBarMessage("Download titles failed!");
    }
    else {
        emit showStatusBarMessage("Download titles successfully!");
    }

    emit setMainProgressBarVisible(false);
}

QNetworkReply::NetworkError Common::getTitle(QString wiitdbPath, QString fileName) {
    QEventLoop loop;
    QNetworkReply *reply;
    QNetworkAccessManager manager;

    manager.setProxy(proxy());
    reply = manager.get(QNetworkRequest(QUrl(wiitdbPath)));

    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() != 0) {
        emit newLogEntry(reply->errorString(), WiTools::Error);
    }

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    return reply->error();
}

QNetworkProxy Common::proxy() {
    QNetworkProxy proxy;

    if (WiiBaFuSettings.value("Main/UseProxy", QVariant(false)).toBool()) {
        proxy.setType((QNetworkProxy::ProxyType)WiiBaFuSettings.value("Main/ProxyType", QVariant(3)).toInt());
        proxy.setHostName(WiiBaFuSettings.value("Main/ProxyHost", QVariant("")).toString());
        proxy.setPort(WiiBaFuSettings.value("Main/ProxyPort", QVariant(0)).toInt());
        proxy.setUser(WiiBaFuSettings.value("Main/ProxyUser", QVariant("")).toString());
        proxy.setPassword(WiiBaFuSettings.value("Main/ProxyPassword", QVariant("")).toString());
    }

    return proxy;
}

QString Common::titleFromDB(QString gameID) {
    QFile file(wiTools->witTitlesPath());
    QString title;

    file.open(QIODevice::ReadOnly);
    while (!file.atEnd()) {
        QString line = file.readLine().data();
            if (line.contains(gameID)) {
                title = fromUtf8(line.right(line.length() - line.indexOf("=") - 2).remove("\r"));
                break;
            }
    }

    file.close();
    return title;
}

void Common::viewInBrowser(QString gameID) {
    QDesktopServices::openUrl(QUrl(QString("http://wiitdb.com/Game/%1").arg(gameID), QUrl::StrictMode));
}

QString Common::fromUtf8(QString string) {
    return QString::fromUtf8(string.toLatin1());
}

Common::~Common() {
    delete wiTools;
}
