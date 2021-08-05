QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = trkdbg

CONFIG += c++11

SOURCES += \
    guimainwindow.cpp \
    main_gui.cpp

HEADERS += \
    guimainwindow.h

FORMS += \
    guimainwindow.ui

!contains(XCONFIG, xdebuggerwidget) {
    XCONFIG += xdebuggerwidget
    include(../../_mylibs/XDebuggerWidget/xdebuggerwidget.pri)
}

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include(../../_mylibs/XOptions/xoptions.pri)
}

include(../build.pri)
