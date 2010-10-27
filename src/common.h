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

class Common : public QObject {
    Q_OBJECT

public:
    explicit Common(QObject *parent = 0);

    void getGame3DCover(QString gameID, QString language);
    void getGameFullHQCover(QString gameID, QString language);

private:
    QHttp *http;
    QUrl url;
    bool hqCover;

    void getGameCover(QString gameID, QString language, bool fullHQCover);

signals:
    void newGame3DCover(QImage *game3DCover);
    void newGameFullHQCover(QImage *gameFullHQCover);
    void showStatusBarMessage(QString message);
    void newLogEntry(QString entry);

private slots:
    void loadGameCoverResponseHeaderReceived(const QHttpResponseHeader &resp);
    void loadGameCoverDone(bool error);
};

#endif // COMMON_H
