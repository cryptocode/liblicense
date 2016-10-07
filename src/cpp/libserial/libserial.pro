#-------------------------------------------------
#
# Project created by QtCreator 2016-10-07T21:11:42
#
#-------------------------------------------------

QT       -= gui

TARGET = libserial
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    sha1.cpp \
    liblicense.cpp

HEADERS += \
    sha1.h \
    liblicense.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
