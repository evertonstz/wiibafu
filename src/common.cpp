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
    http = new QHttp(this);
    connect(http, SIGNAL(done(bool)), this, SLOT(loadGameCover_done(bool)));
    connect(http, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(loadGameCover_responseHeaderReceived(QHttpResponseHeader)));

    wiTools = new WiTools(this);

    wiiTDBLanguages = QStringList() << "EN" << "FR" << "DE" << "ES" << "IT" << "NL" << "PT" << "SE" << "DK" << "NO" << "FI" << "RU" << "JA" << "KO" << "ZHTW" << "ZHCN";
    titlesExtensions = QStringList() << ".txt"  << "-fr.txt" << "-de.txt" << "-es.txt" << "-it.txt" << "-nl.txt" << "-pt.txt" << "-se.txt" << "-dk.txt" << "-no.txt" << "-fi.txt" << "-ru.txt" << "-ja.txt" << "-ko.txt" << "-zhtw.txt" << "-zhcn.txt";
}

void Common::requestGameCover(QString gameID, QString language, GameCoverArt gameCoverArt) {
    emit showStatusBarMessage(tr("Loading game cover..."));

    if (gameCoverArt == Disc) {
        url = QString("http://wiitdb.com/wiitdb/artwork/disc/%1/%2.png").arg(language, gameID);
    }
    else if (gameCoverArt == ThreeD) {
        url = QString("http://wiitdb.com/wiitdb/artwork/cover3D/%1/%2.png").arg(language, gameID);
    }
    else if (gameCoverArt == HighQuality) {
        url = QString("http://wiitdb.com/wiitdb/artwork/coverfullHQ/%1/%2.png").arg(language, gameID);
    }

    currentGameCoverArt = gameCoverArt;

    http->setProxy(proxy());
    http->setHost(url.host());
    http->get(url.path());
}

void Common::loadGameCover_responseHeaderReceived(const QHttpResponseHeader &resp) {
    emit newLogEntry(http->currentRequest().toString(), WiTools::Info);
    emit newLogEntry(resp.toString().remove(" "), WiTools::Info);
}

void Common::loadGameCover_done(bool error) {
    if (!error) {
        QImage *image = new QImage();
        image->loadFromData(http->readAll());

        if (!image->isNull()) {
            emit showStatusBarMessage(tr("Ready."));

            if (currentGameCoverArt == HighQuality) {
                emit newGameFullHQCover(image);
            }
            else if (currentGameCoverArt == ThreeD) {
                emit newGame3DCover(image);
            }
            else if (currentGameCoverArt == Disc) {
                emit newGameDiscCover(image);
            }
        }
        else {
            emit showStatusBarMessage(tr("No game cover available!"));
            emit newLogEntry(tr("No game cover available!"), WiTools::Error);
        }

        delete image;
    }
    else {
        emit showStatusBarMessage(http->errorString());
        emit newLogEntry(http->errorString().append("\n"), WiTools::Error);
    }
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
    delete http;
    delete wiTools;
}
