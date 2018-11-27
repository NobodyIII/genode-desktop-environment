include $(REP_DIR)/lib/import/import-qt5_qprocessenvironment.mk

SRC_DIR = $(REP_DIR)/src/lib/qt5/qprocessenvironment

SHARED_LIB = yes

QT_DEFINES += -DQT_CORE_LIB

# the original project file is incompatible with make, so we integrate a modified version into this file instead

SOURCES += \
    qprocess.cpp

HEADERS += \
    qprocess_p.h

OTHER_FILES += \
    qt5ct.json


LIBS += qt5_core

vpath % $(SRC_DIR)
INC_DIR += $(SRC_DIR)

CC_CXX_WARN_STRICT =
