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
}

void Common::requestGameCover(QString gameID, QString language, GameCoverArt gameCoverArt) {
    if (gameCoverArt == GameCoverDisc) {
        url = QString("http://wiitdb.com/wiitdb/artwork/disc/%1/%2.png").arg(language, gameID);
    }
    else if (gameCoverArt == GameCover3D) {
        url = QString("http://wiitdb.com/wiitdb/artwork/cover3D/%1/%2.png").arg(language, gameID);
    }
    else if (gameCoverArt == GameCoverHQ) {
        url = QString("http://wiitdb.com/wiitdb/artwork/coverfullHQ/%1/%2.png").arg(language, gameID);
    }

    currentGameCoverArt = gameCoverArt;

    http->setHost(url.host());
    http->get(url.path());
}

void Common::loadGameCover_responseHeaderReceived(const QHttpResponseHeader &resp) {
    emit newLogEntry(http->currentRequest().toString());
    emit newLogEntry(resp.toString().remove(" "));
}

void Common::loadGameCover_done(bool error) {
    if (!error) {
        QImage *image = new QImage();
        image->loadFromData(http->readAll());

        if (!image->isNull()) {
            emit showStatusBarMessage(tr("Ready."));

            if (currentGameCoverArt == GameCoverHQ) {
                emit newGameFullHQCover(image);
            }
            else if (currentGameCoverArt == GameCover3D) {
                emit newGame3DCover(image);
            }
            else if (currentGameCoverArt == GameCoverDisc) {
                emit newGameDiscCover(image);
            }
        }
        else {
            emit showStatusBarMessage(tr("No game cover available!"));
        }

        delete image;
    }
    else {
        emit showStatusBarMessage(http->errorString());
        emit newLogEntry(http->errorString().append("\n"));
    }
}

Common::~Common() {
    delete http;
}
