#-------------------------------------------------
#
# Project created by QtCreator 2017-09-13T10:17:20
#
#-------------------------------------------------

QT       += widgets #-= gui

TARGET = QESHidUSB
TEMPLATE = lib


INCLUDEPATH += Include

#Uncomment below for static lib
#CONFIG += staticlib


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HidUSBLink.cpp \
    QHidWatcher.cpp \
    BootloaderUSBLink.cpp \


unix: {
    SOURCES += hid.c \
    worker.cpp
}

HEADERS += \
    Include/qeshidusb_global.h \
    Include/HidUSBLink.h \
    Include/QHidWatcher.h \
    Include/BootloaderUSBLink.h \
    worker.h

unix: {
    HEADERS += hidapi.h
}

win32: LIBS += -lhid
win32: LIBS += -lsetupapi
win32: LIBS += -luser32

unix:CONFIG(debug, debug|release): DESTDIR = ./lib-debug
unix:CONFIG(release, debug|release): DESTDIR = ./lib-release

staticlib: {
    DESTDIR = ./lib-static
    DEFINES += QESHIDUSB_STATIC_LIBRARY
} else {
    DEFINES += QESHIDUSB_LIBRARY
}

unix:!macx: LIBS += -ludev
