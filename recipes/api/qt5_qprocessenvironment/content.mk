MIRRORED_FROM_REP_DIR := include/qt5/qprocessenvironment lib/import/import-qt5_qprocessenvironment.mk lib/symbols/qt5_qprocessenvironment

content: $(MIRRORED_FROM_REP_DIR) LICENSE

$(MIRRORED_FROM_REP_DIR):
	$(mirror_from_rep_dir)

LICENSE:
	cp $(GENODE_DIR)/LICENSE $@
