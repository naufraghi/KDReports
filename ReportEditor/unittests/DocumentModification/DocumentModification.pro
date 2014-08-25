include ($${TOP_SOURCE_DIR}/ReportEditor/kdreportseditorlib.pri)
CONFIG += qtestlib
QT += xml
test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test

DESTDIR = .
HEADERS =
SOURCES = TestDocumentModification.cpp
RESOURCES += $${TOP_SOURCE_DIR}/ReportEditor/unittests/file/file.qrc