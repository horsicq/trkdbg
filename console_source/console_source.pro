QT += core
QT -= gui

include(../build.pri)

CONFIG += c++11

TARGET = trkdbgc
CONFIG += console
CONFIG -= app_bundle

SOURCES += \
        main_console.cpp

DEFINES += USE_XPROCESS

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include(../XOptions/xoptions.pri)
}

!contains(XCONFIG, xspecdebugger) {
    XCONFIG += xspecdebugger
    include(../XSpecDebugger/xspecdebugger.pri)
}

!contains(XCONFIG, xdebugscript) {
    XCONFIG += xdebugscript
    include(../XDebugScript/xdebugscript.pri)
}
