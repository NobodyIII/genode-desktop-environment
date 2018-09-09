include $(call select_from_repositories,src/app/qt5/tmpl/target_defaults.inc)

include $(call select_from_repositories,src/app/qt5/tmpl/target_final.inc)

LIBS += libc qt5_component qt5_widgets

CC_CXX_WARN_STRICT =
