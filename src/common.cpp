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
    connect(http, SIGNAL(done(bool)), this, SLOT(loadGameCoverDone(bool)));
    connect(http, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(loadGameCoverResponseHeaderReceived(QHttpResponseHeader)));
}

void Common::getGame3DCover(QString gameID, QString language) {
    hqCover = false;
    getGameCover(gameID, language, false);
}

void Common::getGameFullHQCover(QString gameID, QString language) {
    hqCover = true;
    getGameCover(gameID, language, true);
}

void Common::getGameCover(QString gameID, QString language, bool fullHQCover) {
    if (fullHQCover)
        url = QString("http://wiitdb.com/wiitdb/artwork/coverfullHQ/%1/%2.png").arg(language, gameID);
    else
        url = QString("http://wiitdb.com/wiitdb/artwork/cover3D/%1/%2.png").arg(language, gameID);

    http->setHost(url.host());
    http->get(url.path());
}

void Common::loadGameCoverResponseHeaderReceived(const QHttpResponseHeader &resp) {
    emit newLogEntry(http->currentRequest().toString());
    emit newLogEntry(resp.toString().remove(" "));
}

void Common::loadGameCoverDone(bool error) {
    if (!error) {
        QImage *image = new QImage();
        image->loadFromData(http->readAll());

        if (!image->isNull()) {
            emit showStatusBarMessage(tr("Ready."));
            if (hqCover)
                emit newGameFullHQCover(image);
            else
                emit newGame3DCover(image);
        }
        else
            emit showStatusBarMessage(tr("No game cover available!"));
    }
    else {
        emit showStatusBarMessage(http->errorString());
        emit newLogEntry(http->errorString().append("\n"));
    }
}
