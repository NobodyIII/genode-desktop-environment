# identify the qt5 repository by searching for a file that is unique for qt5
QT5_REP_DIR := $(call select_from_repositories,lib/import/import-qt5.inc)
QT5_REP_DIR := $(realpath $(dir $(QT5_REP_DIR))../..)

SHARED_LIB = yes

#echo $(PRG_DIR)

#QMAKE_PROJECT_FILE = $(REP_DIR)/src/lib/bgde_file_request_shim/bgde_file_request_shim.pro
vpath % $(REP_DIR)/src/lib/bgde_file_request_shim
vpath % $(REP_DIR)/src/app/file_dialog

SOURCES += \
	shim.cpp \
	filedialog.cpp
           
HEADERS += \
	filedialog.h

include $(QT5_REP_DIR)/src/app/qt5/tmpl/target_defaults.inc

include $(QT5_REP_DIR)/src/app/qt5/tmpl/target_final.inc
