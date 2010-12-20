HEADERS         += src/global.h src/wiibafu.h src/wiibafudialog.h src/witools.h src/common.h src/settings.h src/coverviewdialog.h src/wbfsdialog.h
SOURCES         += src/main.cpp src/wiibafu.cpp src/wiibafudialog.cpp src/witools.cpp src/common.cpp src/settings.cpp src/coverviewdialog.cpp src/wbfsdialog.cpp
FORMS           += resources/ui/wiibafu.ui resources/ui/wiibafudialog.ui resources/ui/settings.ui resources/ui/coverviewdialog.ui resources/ui/wbfsdialog.ui
RESOURCES       += resources/wiibafu.qrc
TRANSLATIONS    += resources/locale/german.ts

TEMPLATE         = app
TARGET           = WiiBaFu
CONFIG          += qt warn_on
QT              += core gui network
DESTDIR          = bin
INCLUDEPATH      = src
OBJECTS_DIR      = build/o
UI_DIR           = build/ui
MOC_DIR          = build/moc
RCC_DIR          = build/rcc
CODECFORTR       = UTF-8
CODECFORSRC      = UTF-8

QMAKE_EXTRA_COMPILERS += lrelease
lrelease.input         = TRANSLATIONS
lrelease.output        = ${QMAKE_FILE_BASE}.qm
lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm bin/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG       += no_link target_predeps

win32 {
  RC_FILE = win/appicon.rc
}

macx {
  ICON              += resources/images/appicon.icns
  QMAKE_INFO_PLIST  += mac/Info.plist
  TARGET             = "Wii Backup Fusion"
  CONFIG            += x86 ppc
}
