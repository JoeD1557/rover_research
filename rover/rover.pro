QT += core network

CONFIG += c++11 no_keywords
CONFIG += console
CONFIG -= app_bundle
TARGET = core
TEMPLATE = app

BUILD_DIR = ../build/rover
DESTDIR = ../bin
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

LIBS += -L../lib -lcore
