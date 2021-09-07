QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = trkdbg

CONFIG += c++11

SOURCES += \
    dialogabout.cpp \
    dialogoptions.cpp \
    guimainwindow.cpp \
    main_gui.cpp

HEADERS += \
    dialogabout.h \
    dialogoptions.h \
    guimainwindow.h

FORMS += \
    dialogabout.ui \
    dialogoptions.ui \
    guimainwindow.ui

!contains(XCONFIG, xdebuggerwidget) {
    XCONFIG += xdebuggerwidget
    include(../XDebuggerWidget/xdebuggerwidget.pri)
}

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include(../XOptions/xoptions.pri)
}

include(../build.pri)
