TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS  = kdreportseditorlib.pro kdreportseditor.pro
unittests: SUBDIRS += unittests

# forward make test calls to unittests:
test.target=test
unix:!macx:test.commands=cd unittests && LD_LIBRARY_PATH=\"$${OUT_PWD}/../lib\":$$(LD_LIBRARY_PATH) $(MAKE) test;
macx:test.commands=cd unittests && DYLD_LIBRARY_PATH=\"$${OUT_PWD}/../lib\":$$(DYLD_LIBRARY_PATH) $(MAKE) test;
win32:test.commands=(cd unittests && $(MAKE) test);
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test
