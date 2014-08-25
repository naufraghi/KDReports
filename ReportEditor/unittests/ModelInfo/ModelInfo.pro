include ($$TOP_SOURCE_DIR/ReportEditor/kdreportseditorlib.pri)
CONFIG += qtestlib
test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test

DESTDIR = .
HEADERS =
SOURCES = ModelInfoTest.cpp
