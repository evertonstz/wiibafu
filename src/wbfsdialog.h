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

#ifndef WBFSDIALOG_H
#define WBFSDIALOG_H

#include <QtGui>
#include "global.h"

namespace Ui {
    class WBFSDialog;
}

class WBFSDialog : public QDialog {
    Q_OBJECT

public:
    explicit WBFSDialog(QWidget *parent = 0);
    ~WBFSDialog();

    QString path();
    QString size();
    QString splitSize();
    QString hdSectorSize();
    QString wbfsSectorSize();
    bool recover();
    bool inode();
    bool test();
    void setMacOSXStyle();

private:
    Ui::WBFSDialog *ui;

private slots:

    void on_pushButton_Open_clicked();
};

#endif // WBFSDIALOG_H
