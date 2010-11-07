HEADERS     += src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h
SOURCES     += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp
FORMS       += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui
RESOURCES   += resources/wiibafu.qrc

TEMPLATE     = app
TARGET       = WiiBaFu
CONFIG      += qt warn_on debug
QT          += core gui network
DESTDIR      = bin
INCLUDEPATH  = src
OBJECTS_DIR  = build
UI_DIR       = build
MOC_DIR      = build
RCC_DIR      = build

macx {
  ICON              += resources/images/appicon.icns
  QMAKE_INFO_PLIST  += Info.plist
  CONFIG            += x86 ppc
}

