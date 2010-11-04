HEADERS     += src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h
SOURCES     += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp
FORMS       += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui
RESOURCES   += resources/wiibafu.qrc

TEMPLATE     = app
TARGET       = WiiBaFu
CONFIG      += qt warn_on debug
QT          += core gui network
DESTDIR     += bin
INCLUDEPATH += src
MOC_DIR     += build/moc
UI_DIR      += build/ui
RCC_DIR     += build/rcc

macx {
  ICON        += resources/images/wiibafu.icns
  OBJECTS_DIR += build/o/mac
}
unix {
  OBJECTS_DIR += build/o/unix
}
win32 {
  OBJECTS_DIR += build/o/win32
}
