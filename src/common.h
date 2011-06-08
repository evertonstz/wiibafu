/***************************************************************************
 *   Copyright (C) 2010-2011 Kai Heitkamp                                  *
 *   dynup@ymail.com | http://sf.net/p/wiibafu                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

private:
    WiTools *wiTools;
    QImage cover;
    QTimer *timer;
    QStringList wiiTDBLanguages;
    QStringList titlesExtensions;

    QNetworkReply::NetworkError getCover(const QString url);
    QNetworkReply::NetworkError getTitle(const QString wiitdbPath, const QString fileName);
    QNetworkProxy proxy();

public:
    enum GameCoverArt {
        Disc = 0x0,
        ThreeD = 0x1,
        HighQuality = 0x2
    };

    explicit Common(QObject *parent = 0);
    ~Common();

    void requestGameCover(const QString gameID, const QString language, const GameCoverArt gameCoverArt);
    void updateTitles();
    QString titleFromDB(const QString gameID);
    void viewInBrowser(const QString gameID);

    static QString fromUtf8(const QString string);
    static QString calculateSize(const double size);
    static QString calculateSize(const QString size);
    static QString translateTransferMessage(QString str);

signals:
    void newGame3DCover(const QImage game3DCover);
    void newGameFullHQCover(const QImage gameFullHQCover);
    void newGameDiscCover(const QImage gameDiscCover);
    void showStatusBarMessage(const QString message);
    void newLogEntry(const QString entry, const WiTools::LogType);
    void setMainProgressBarVisible(const bool visible);
    void setMainProgressBar(const int value, const QString format);

private slots:
    void getCover_downloadProgress(const qint64 bytesReceived, const qint64 bytesTotal);
    void getCover_timeOut();
};

#endif // COMMON_H
