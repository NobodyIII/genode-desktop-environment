TARGET := qt5ct

QT5CT_PORT_DIR := $(call select_from_ports,qt5ct)/src/qt5ct
QT5CT_DIR := $(QT5CT_PORT_DIR)/src/qt5ct

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tabpage.cpp \
    appearancepage.cpp \
    fontspage.cpp \
    qt5ct.cpp \
    paletteeditdialog.cpp \
    iconthemepage.cpp \
    interfacepage.cpp \
    fontconfigdialog.cpp \
    qsspage.cpp \
    qsseditordialog.cpp

FORMS += \
    mainwindow.ui \
    appearancepage.ui \
    fontspage.ui \
    paletteeditdialog.ui \
    iconthemepage.ui \
    interfacepage.ui \
    fontconfigdialog.ui \
    previewform.ui \
    qsspage.ui \
    qsseditordialog.ui

HEADERS += \
    mainwindow.h \
    tabpage.h \
    appearancepage.h \
    fontspage.h \
    qt5ct.h \
    paletteeditdialog.h \
    iconthemepage.h \
    interfacepage.h \
    fontconfigdialog.h \
    qsspage.h \
    qsseditordialog.h

QT_DEFINES += -DUSE_WIDGETS
QT_DEFINES += -DQT_CORE_LIB -DQT_GUI_LIB -DQT_WIDGETS_LIB

vpath % $(QT5CT_DIR)
INC_DIR += $(QT5CT_DIR)

LIBS += qt5_qprocessenvironment qt5_core qt5_gui qt5_widgets qt5_qpa_nitpicker qt5_component

CC_CXX_WARN_STRICT =
