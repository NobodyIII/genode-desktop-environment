MIRRORED_FROM_REP_DIR := src/lib/qt5/qprocessenvironment include/qt5/qprocessenvironment \
                         lib/mk/qt5_qprocessenvironment.mk lib/import/import-qt5_qprocessenvironment.mk

content: $(MIRRORED_FROM_REP_DIR) LICENSE

$(MIRRORED_FROM_REP_DIR):
	$(mirror_from_rep_dir)

LICENSE:
	cp $(GENODE_DIR)/LICENSE $@
