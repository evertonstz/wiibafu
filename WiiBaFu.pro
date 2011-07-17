HEADERS         += src/global.h src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h src/coverviewdialog.h src/wbfsdialog.h
SOURCES         += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp src/coverviewdialog.cpp src/wbfsdialog.cpp
FORMS           += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui resources/ui/coverviewdialog.ui resources/ui/wbfsdialog.ui
RESOURCES       += resources/wiibafu.qrc resources/locale.qrc
TRANSLATIONS    += resources/locale/english.ts resources/locale/german.ts resources/locale/french.ts resources/locale/dutch.ts resources/locale/spanish.ts resources/locale/norwegian.ts resources/locale/italian.ts

TEMPLATE         = app
TARGET           = WiiBaFu
CONFIG          += qt warn_on thread
QT              += core gui network
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
