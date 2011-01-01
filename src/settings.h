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

#ifndef Settings_H
#define Settings_H

#include <QtGui>

#include "global.h"

namespace Ui {
    class Settings;
}

class Settings : public QDialog {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    void setMacOSXStyle();

private:
    Ui::Settings *ui;

    void apply();
    void load();
    void save();

    void restoreDefaults(const int index);

    void setScrollMode(const int mode);
    void setResizeMode(const int mode);
    int scrollMode();
    int resizeMode();
    QString macOSXStyle();

    void setupGeometry();
    void saveGeometry();

private slots:
    void on_listWidget_itemSelectionChanged();

    void on_main_pushButton_OpenLogFile_clicked();

    void on_wit_pushButton_PathToWITOpen_clicked();
    void on_wit_pushButton_WBFSOpenFile_clicked();
    void on_wit_pushButton_WBFSOpenDirectory_clicked();
    void on_wit_pushButton_DVDDriveOpenDirectory_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonBox_accepted();

    void on_main_checkBox_useProxy_stateChanged(int state);
    void on_wit_checkBox_Auto_stateChanged(int state);
    void on_transfer_checkBox_ScrubbingData_stateChanged(int state);
    void on_transfer_checkBox_ScrubbingUpdate_stateChanged(int state);
    void on_transfer_checkBox_ScrubbingChannel_stateChanged(int state);
    void on_transfer_checkBox_ScrubbingRaw_stateChanged(int state);
    void on_checkWBFS_checkBox_Repair_stateChanged(int state);
};

#endif // Settings_H
