#-------------------------------------------------
#
# Project created by QtCreator 2016-10-09T00:36:02
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LicenseUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libserial/release/ -llibserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libserial/debug/ -llibserial
else:unix: LIBS += -L$$OUT_PWD/../libserial/ -llibserial

INCLUDEPATH += $$PWD/../libserial
DEPENDPATH += $$PWD/../libserial

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libserial/release/liblibserial.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libserial/debug/liblibserial.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libserial/release/libserial.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libserial/debug/libserial.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libserial/liblibserial.a
