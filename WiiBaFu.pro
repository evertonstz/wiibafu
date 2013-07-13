##########################################################################
#                                                                        #
#  Copyright (C) 2011 - 2013 Kai Heitkamp                                #
#  dynup@ymail.com | dynup.de.vu                                         #
#                                                                        #
#  This program is free software; you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation; either version 3 of the License, or     #
#  (at your option) any later version.                                   #
#                                                                        #
#  This program is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this program; if not, go to http://www.gnu.org             #
#                                                                        #
##########################################################################

HEADERS         += src/global.h src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h src/coverviewdialog.h src/wbfsdialog.h
SOURCES         += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp src/coverviewdialog.cpp src/wbfsdialog.cpp
FORMS           += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui resources/ui/coverviewdialog.ui resources/ui/wbfsdialog.ui
RESOURCES       += resources/wiibafu.qrc resources/locale.qrc
TRANSLATIONS    += resources/locale/english.ts resources/locale/german.ts resources/locale/french.ts resources/locale/dutch.ts resources/locale/spanish.ts resources/locale/norwegian.ts resources/locale/italian.ts

TEMPLATE         = app
TARGET           = WiiBaFu
CONFIG          += qt warn_on thread
QT              += widgets core network
DESTDIR          = bin
INCLUDEPATH      = src
OBJECTS_DIR      = build/o
UI_DIR           = build/ui
MOC_DIR          = build/moc
RCC_DIR          = build/rcc
CODECFORTR       = UTF-8
CODECFORSRC      = UTF-8

linux-g++ {
  target.path    = /usr/bin
  icon.path     += /usr/share/pixmaps
  icon.files    += resources/images/WiiBaFu.png
  desktop.path  += /usr/share/applications
  desktop.files += resources/WiiBaFu.desktop
  INSTALLS      += target icon desktop
}

win32 {
  RC_FILE = win/appicon.rc
  target.path  = "C:/Program Files/WiiBaFu"
  INSTALLS      += target
}

macx {
  ICON              += resources/images/appicon.icns
  QMAKE_INFO_PLIST  += mac/Info.plist
  TARGET             = "Wii Backup Fusion"
  CONFIG            += x86 ppc
  target.path        = /Applications
  INSTALLS          += target
}
