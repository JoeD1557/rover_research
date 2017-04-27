QT += core network
QT -= gui

CONFIG += c++11 no_keywords
CONFIG -= app_bundle
TARGET = core
TEMPLATE = lib

DEFINES += CORE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

BUILD_DIR = ../build/core
DESTDIR = ../lib
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR

SOURCES += audioformat.cpp \
    channel.cpp \
    csvrecorder.cpp \
    drivemessage.cpp \
    gamepadutil.cpp \
    gpscsvseries.cpp \
    logger.cpp \
    mbedchannel.cpp \
    nmeamessage.cpp \
    videoformat.cpp \
    socketaddress.cpp \
    sensordataparser.cpp \
    mediastreamer.cpp

HEADERS += core_global.h \
    audioformat.h \
    channel.h \
    constants.h \
    csvrecorder.h \
    drivemsssage.h \
    enums.h \
    gamepadutil.h \
    gpscsvseries.h \
    latnlg.h \
    logger.h \
    mbedchannel.h \
    mediaformat.h \
    nmeamessage.h \
    util.h \
    videoformat.h \
    socketaddress.h \
    sensordataparser.h \
    mediastreamer.h


