#-------------------------------------------------
#
# Project created by QtCreator 2017-06-04T11:46:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = file_manager_qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    nodedelegate.cpp \
    folderview.cpp \
    dragdropmenu.cpp \
    utils.cpp \
    fsmodel.cpp

HEADERS  += mainwindow.h \
    nodedelegate.h \
    folderview.h \
    dragdropmenu.h \
    utils.h \
    fsmodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
