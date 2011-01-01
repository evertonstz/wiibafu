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

#ifndef WIIBAFUDIALOG_H
#define WIIBAFUDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
    class WiiBaFuDialog;
}

class WiiBaFuDialog : public QDialog {
    Q_OBJECT

public:
    explicit WiiBaFuDialog(QWidget *parent = 0);
    ~WiiBaFuDialog();

    QString directory();
    QString filePath();
    bool split();
    QString splitSize();
    QString imageFormat();
    QString compression();
    void setOpenImageDirectory();
    void setOpenDirectory();
    void setOpenFile();
    void setMacOSXStyle();

private:
    Ui::WiiBaFuDialog *ui;
    void setCurrentImageFormat(const QString path);

private slots:
    void on_pushButton_Open_clicked();
    void on_pushButton_OpenFile_clicked();
    void on_comboBox_ImageFormat_currentIndexChanged(int index);
    void on_pushButton_Split_toggled(bool checked);
    void on_comboBox_compressionDefaults_currentIndexChanged(int index);
};

#endif // WIIBAFUDIALOG_H
