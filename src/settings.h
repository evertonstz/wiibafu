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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui>

namespace Ui {
    class Settings;
}

class Settings : public QDialog {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::Settings *ui;

    void apply();
    void load();
    void save();

    void restoreDefaults(int index);

    void setScrollMode(int mode);
    void setResizeMode(int mode);
    int scrollMode();
    int resizeMode();

private slots:
    void on_listWidget_itemSelectionChanged();

    void on_main_pushButton_PathToWITOpen_clicked();
    void on_main_pushButton_WBFSOpenFile_clicked();
    void on_main_pushButton_WBFSOpenDirectory_clicked();
    void on_main_pushButton_DVDDriveOpenDirectory_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonBox_accepted();

    void on_main_checkBox_Auto_stateChanged(int state);
    void on_checkWBFS_checkBox_Repair_stateChanged(int state);
};

#endif // SETTINGS_H
