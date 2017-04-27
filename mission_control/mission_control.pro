QT += core widgets network qml quick quickcontrols2 webengine

CONFIG += c++11 no_keywords
TARGET = core
TEMPLATE = app

BUILD_DIR = ../build/mission_control
DESTDIR = ../bin
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR

SOURCES += main.cpp \
    settingsmodel.cpp \
    hudpowerimpl.cpp \
    hudlatencygraphimpl.cpp \
    hudorientationsideimpl.cpp \
    hudorientationbackimpl.cpp \
    abstracthudorientationimpl.cpp \
    latencycsvseries.cpp \
    commentcsvseries.cpp \
    connectioneventcsvseries.cpp \
    gstreamerrecorder.cpp \
    drivecontrolsystem.cpp \
    gamepadmanager.cpp \
    audioplayer.cpp \
    mediaclient.cpp \
    mainwindowcontroller.cpp \
    controlwindowcontroller.cpp \
    maincontroller.cpp \
    commentswindowcontroller.cpp \
    videoclient.cpp \
    audioclient.cpp \
    qmlgstreamerglitem.cpp \
    qmlgstreamerpainteditem.cpp

HEADERS  += \
    settingsmodel.h \
    hudpowerimpl.h \
    hudlatencygraphimpl.h \
    hudorientationsideimpl.h \
    hudorientationbackimpl.h \
    abstracthudorientationimpl.h \
    latencycsvseries.h \
    commentcsvseries.h \
    connectioneventcsvseries.h \
    gstreamerrecorder.h \
    drivecontrolsystem.h \
    gamepadmanager.h \
    audioplayer.h \
    mediaclient.h \
    videoclient.h \
    audioclient.h \
    qmlgstreamerglitem.h \
    qmlgstreamerpainteditem.h \
    mainwindowcontroller.h \
    controlwindowcontroller.h \
    maincontroller.h \
    commentswindowcontroller.h

RESOURCES += \
    qml.qrc \
    assets.qrc

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

# Link SDL2
LIBS += -lSDL2
#Link Qt5GStreamer
LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUi-1.0 -lQt5GStreamerUtils-1.0 -lQt5GStreamerQuick-1.0

LIBS += -L../lib -lcore
