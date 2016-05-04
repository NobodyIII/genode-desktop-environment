# identify the qt5 repository by searching for a file that is unique for qt5
QT5_REP_DIR := $(call select_from_repositories,lib/import/import-qt5.inc)
QT5_REP_DIR := $(realpath $(dir $(QT5_REP_DIR))../..)

include $(QT5_REP_DIR)/src/app/qt5/tmpl/target_defaults.inc

include $(QT5_REP_DIR)/src/app/qt5/tmpl/target_final.inc

#CC_CXX_OPT += -DTEST_LINUX

dragdropmenu.o: dragdropmenu.moc
folderview.o: folderview.moc
fsnode.o: fsnode.moc
fsregistry.o: fsregistry.moc
genodefs.o: genodefs.moc
genodefsmodel.o: genodefsmodel.moc
main_window.o: main_window.moc
main_window.o: main_window.uic
nodedelegate.o: nodedelegate.moc
test_linux.o: test_linux.moc
#main.o: main.moc

LIBS += config qoost
