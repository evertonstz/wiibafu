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

#ifndef COMMON_H
#define COMMON_H

#include <QtGui>
#include <QtNetwork>

#include "witools.h"

class Common : public QObject {
    Q_OBJECT

public:
    enum GameCoverArt {
        Disc = 0x0,
        ThreeD = 0x1,
        HighQuality = 0x2
    };

    explicit Common(QObject *parent = 0);
    ~Common();

    void requestGameCover(QString gameID, QString language, GameCoverArt gameCoverArt);
    void updateTitles();

private:
    QHttp *http;
    QUrl url;
    GameCoverArt currentGameCoverArt;
    QStringList wiiTDBLanguages;
    QStringList titlesExtensions;

    void getGameCover(QString gameID, QString language);
    QNetworkReply::NetworkError getTitle(QString wiitdbPath, QString fileName);

signals:
    void newGame3DCover(QImage *game3DCover);
    void newGameFullHQCover(QImage *gameFullHQCover);
    void newGameDiscCover(QImage *gameDiscCover);
    void showStatusBarMessage(QString message);
    void newLogEntry(QString entry, WiTools::LogType);
    void setMainProgressBarVisible(bool visible);
    void setMainProgressBar(int value, QString format);

private slots:
    void loadGameCover_responseHeaderReceived(const QHttpResponseHeader &resp);
    void loadGameCover_done(bool error);
};

#endif // COMMON_H
