#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T22:34:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = text_editor_qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    linenumberarea.cpp \
    textarea.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    linenumberarea.h \
    textarea.h \
    util.h

FORMS    += mainwindow.ui

CONFIG += c++11
