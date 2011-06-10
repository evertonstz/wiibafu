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

#include "coverviewdialog.h"
#include "ui_coverviewdialog.h"

CoverViewDialog::CoverViewDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CoverViewDialog) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    connect(this, SIGNAL(finished(int)), this, SLOT(finished(int)));

    #ifdef Q_OS_MACX
        setMacOSXStyle();
    #endif
}

void CoverViewDialog::setCover(const QImage cover, const QString id) {
    gameID = id;

    ui->label_GameCover->setPixmap(QPixmap::fromImage(cover, Qt::AutoColor));
}

void CoverViewDialog::on_pushButton_Save_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save game cover"), QDir::homePath().append("/%1_CoverFullHQ.png").arg(gameID), tr("Image file *.png"));

    if (!fileName.isEmpty()) {
        ui->label_GameCover->pixmap()->save(fileName);
    }
}

void CoverViewDialog::on_pushButton_OK_clicked() {
    close();
}

void CoverViewDialog::setMacOSXStyle() {
    if (WIIBAFU_SETTINGS.value("Main/MacOSXStyle", QVariant("Aqua")).toString().contains("BrushedMetal")) {
        ui->frame_Cover->setFrameStyle(QFrame::NoFrame);
        ui->frame_Buttons->setFrameStyle(QFrame::NoFrame);

        this->setAttribute(Qt::WA_MacBrushedMetal, true);
    }
    else {
        ui->frame_Cover->setFrameStyle(QFrame::StyledPanel);
        ui->frame_Buttons->setFrameStyle(QFrame::Box);
        ui->frame_Buttons->setFrameShadow(QFrame::Raised);

        this->setAttribute(Qt::WA_MacBrushedMetal, false);
    }
}

void CoverViewDialog::finished(const int) {
    ui->label_GameCover->clear();
}

CoverViewDialog::~CoverViewDialog() {
    delete ui;
}
