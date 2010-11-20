HEADERS     += src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h src/coverviewdialog.h src/wbfsdialog.h
SOURCES     += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp src/coverviewdialog.cpp src/wbfsdialog.cpp
FORMS       += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui resources/ui/coverviewdialog.ui resources/ui/wbfsdialog.ui
RESOURCES   += resources/wiibafu.qrc

TEMPLATE     = app
TARGET       = WiiBaFu
CONFIG      += qt warn_on
QT          += core gui network
DESTDIR      = bin
INCLUDEPATH  = src
OBJECTS_DIR  = build
UI_DIR       = build
MOC_DIR      = build
RCC_DIR      = build

win32 {
  RC_FILE			 = win/appicon.rc
}

macx {
  ICON              += resources/images/appicon.icns
  QMAKE_INFO_PLIST  += mac/Info.plist
  TARGET             = "Wii Backup Fusion"
  CONFIG            += x86 ppc
}
