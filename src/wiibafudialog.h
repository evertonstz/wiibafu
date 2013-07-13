/***************************************************************************
 *   Copyright (C) 2010-2013 Kai Heitkamp                                  *
 *   dynup@ymail.com | http://dynup.de.vu                                  *
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

    QString sourceFilePath();
    QString directory();
    QString filePath();
    bool split();
    QString splitSize();
    QString imageFormat();
    QString compression();

    void setSourceFilePath(const QString path);
    void setDirectory(const QString path);
    void setOpenImageDirectory(const bool patch);
    void setOpenDirectory(const bool patch);
    void setOpenFile(const bool patch);
    void setPatchGame();
    void setGameID(const QString gameID);
    void setGameName(const QString gameName);

    QString gameID();
    QString gameName();
    QString gameRegion();
    QString gameIOS();
    QString gameModify();
    QString gameEncodingMode();
    QString gameCommonKey();

    void setMacOSXStyle();

private:
    Ui::WiiBaFuDialog *ui;
    QString m_sourceFilePath;

    void setCurrentImageFormat(const QString path);
    bool event(QEvent *event);

private slots:
    void on_pushButton_Open_clicked();
    void on_pushButton_SourcePath_clicked();
    void on_pushButton_OpenFile_clicked();
    void on_comboBox_ImageFormat_currentIndexChanged(int index);
    void on_pushButton_Split_toggled(bool checked);
    void on_comboBox_compressionDefaults_currentIndexChanged(int index);
    void on_pushButton_Patch_toggled(bool checked);
    void on_comboBox_IOS_currentIndexChanged(int index);
};

#endif // WIIBAFUDIALOG_H
