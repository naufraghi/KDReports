# This file is included by the KDReportsEditor.

include ($$TOP_SOURCE_DIR/examples/examples.pri)

INCLUDEPATH += $${TOP_SOURCE_DIR}/ReportEditor
DEPENDPATH += $${TOP_SOURCE_DIR}/ReportEditor

LIBS        += -L$${TOP_BUILD_DIR}/lib -l$$KDREPORTSEDITORLIB -l$$KDREPORTSLIB

contains( $$list($$[QT_VERSION]), 5.* ): QT += widgets printsupport

# Must be last (order matters for static builds)
include ($$TOP_SOURCE_DIR/kdtools.pri)
