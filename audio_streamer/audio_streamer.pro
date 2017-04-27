QT += core network

CONFIG += c++11 no_keywords
CONFIG += console
CONFIG -= app_bundle
TARGET = core
TEMPLATE = app

BUILD_DIR = ../build/audio_streamer
DESTDIR = ../bin
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR

HEADERS += audiostreamer.h

SOURCES += audiostreamer.cpp \
main.cpp

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

#Link Qt5GStreamer
LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0

LIBS += -L../lib -lcore
