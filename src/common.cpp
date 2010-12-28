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

void Common::requestGameCover(const QString gameID, const QString language, const GameCoverArt gameCoverArt) {
    emit showStatusBarMessage(tr("Loading game cover..."));
    QNetworkReply::NetworkError result;

    if (gameCoverArt == Disc) {
        result = getCover(QString("http://wiitdb.com/wiitdb/artwork/disc/%1/%2.png").arg(language, gameID));

        if (result != QNetworkReply::NoError && language != "EN") {
            emit newLogEntry(tr("No game cover in selected language available! Trying to download the english one..."), WiTools::Info);
            result = getCover(QString("http://wiitdb.com/wiitdb/artwork/disc/EN/%2.png").arg(gameID));
        }

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

        if (result != QNetworkReply::NoError && language != "EN") {
            emit newLogEntry(tr("No game cover in selected language available! Trying to download the english one..."), WiTools::Info);
            result = getCover(QString("http://wiitdb.com/wiitdb/artwork/cover3D/EN/%2.png").arg(gameID));
        }

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

        if (result != QNetworkReply::NoError && language != "EN") {
            emit newLogEntry(tr("No game cover in selected language available! Trying to download the english one..."), WiTools::Info);
            result = getCover(QString("http://wiitdb.com/wiitdb/artwork/coverfullHQ/EN/%2.png").arg(gameID));
        }

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

QNetworkReply::NetworkError Common::getCover(const QString url) {
    QEventLoop loop;
    QNetworkReply *reply;
    QNetworkAccessManager manager;

    timer->start(3000);

    emit setMainProgressBar(0, "%p%");
    emit newLogEntry(tr("Loading game cover from %1...").arg(QUrl(url).host()), WiTools::Info);

    manager.setProxy(proxy());
    reply = manager.get(QNetworkRequest(QUrl(url)));

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(getCover_downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() != 0) {
        emit newLogEntry(tr("Error: %1, %2").arg(QString::number(reply->error()), reply->errorString()), WiTools::Error);
    }
    else {
        emit newLogEntry(tr("Done! %1 successfully downloaded!").arg(url), WiTools::Info);
        cover = QImage::fromData(reply->readAll());
    }

    if (timer->isActive()) {
        timer->stop();
    }

    emit setMainProgressBarVisible(false);

    return reply->error();
}

void Common::getCover_downloadProgress(const qint64 bytesReceived, const qint64 bytesTotal) {
    if (bytesReceived > 0 && bytesTotal > 0) {
        emit setMainProgressBar(bytesReceived * 100 / bytesTotal, "%p%");
    }
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
        emit showStatusBarMessage(tr("Download titles failed!"));
    }
    else {
        emit showStatusBarMessage(tr("Download titles successfully!"));
    }

    emit setMainProgressBarVisible(false);
}

QNetworkReply::NetworkError Common::getTitle(const QString wiitdbPath, const QString fileName) {
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

QString Common::titleFromDB(const QString gameID) {
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

void Common::viewInBrowser(const QString gameID) {
    QDesktopServices::openUrl(QUrl(QString("http://wiitdb.com/Game/%1").arg(gameID), QUrl::StrictMode));
}

QString Common::fromUtf8(const QString string) {
    return QString::fromUtf8(string.toLatin1());
}

QString Common::calculateSize(const double size) {
    QString str;

    if ( size >= 1073741824 ) {
        str = QString::number(size / 1073741824, 'f', 2);

        if (str.mid(str.indexOf(".") + 1, 2).contains("00")) {
            str.remove(str.indexOf("."), 3);
        }

        return QString("%1 GiB").arg(str);
    }
    else if ( size >= 1048576 ) {
        str = QString::number(size / 1048576, 'f', 2);

        if (str.mid(str.indexOf(".") + 1, 2).contains("00")) {
            str.remove(str.indexOf("."), 3);
        }

        return QString("%1 MiB").arg(str);
    }
    else if ( size >= 1024 ) {
        str = QString::number(size / 1024, 'f', 2);

        if (str.mid(str.indexOf(".") + 1, 2).contains("00")) {
            str.remove(str.indexOf("."), 3);
        }

        return QString("%1 KiB").arg(str);
    }
    else {
        return QString("%1 Bytes").arg(QString::number(size));
    }
}

QString Common::calculateSize(const QString size) {
    return calculateSize(size.toDouble());
}

QString Common::translatedTransferMessage(QString str) {
    if (str.contains("copied")) {
        str.replace("copied", tr("copied"));
    }

    if (str.contains("compared")) {
        str.replace("compared", tr("compared"));
    }

    if (str.contains("sec")) {
        str.replace("sec", tr("sec"));
    }

    if (str.contains("disc added")) {
        str.replace("disc added", tr("disc added"));
    }

    if (str.contains("discs added")) {
        str.replace("discs added", tr("discs added"));
    }

    if (str.contains("disc extracted")) {
        str.replace("disc extracted", tr("disc extracted"));
    }

    if (str.contains("discs extracted")) {
        str.replace("discs extracted", tr("discs extracted"));
    }

    return str;
}

Common::~Common() {
    delete wiTools;
}
