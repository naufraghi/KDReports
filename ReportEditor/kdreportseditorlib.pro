TEMPLATE = lib

# Use the filename "kdreportseditorlibd1.dll" (or "kdreporteditord1.lib") on Windows
# to avoid name clashes between debug/non-debug versions of the
# KD Reports Editor library:
TARGET = kdreportseditorlib
CONFIG(debug, debug|release):!unix:TARGET = $${TARGET}d
include(../kdchart.pri)
include(../kdtools.pri)

# Workaround for visual studio integration
DESTDIR = ../lib
win32:DLLDESTDIR = ../bin

QT += xml
contains( $$list($$[QT_VERSION]), 5.* ): QT += widgets printsupport

KDREPORTS_PATH = $$PWD/..

include( $$KDREPORTS_PATH/examples/examples.pri )

DEFINES += KDREPORT_BUILD_EDITOR_LIB
DEPENDPATH += .
INCLUDEPATH += $$KDREPORTS_PATH/src/KDReports \
    $$KDREPORTS_PATH/ReportEditor

HEADERS = ReportViewerWindow.h ValueAssociationWidget.h ModelEditor.h \
          HeaderConverterProxy.h kdsignalblocker.h ModelInfo.h \
          XmlHandler.h HeaderDiscardingProxy.h KDABBaseProxyModel.h \
          ReportObjectXmlElementHandler.h ReportStructureWidget.h \
          ReportStructureModel.h \
          structure/ObjectData.h \
          structure/ReportData.h structure/HeaderData.h \
          structure/FooterData.h structure/TextElementData.h \
          structure/HtmlElementData.h structure/ImageElementData.h \
          structure/HLineElementData.h \
          structure/PageBreakData.h structure/ChartElementData.h \
          structure/AutoTableElementData.h structure/CellData.h \
          structure/TableElementData.h structure/VariableData.h \
          structure/VSpaceData.h structure/ParagraphData.h \
          structure/ParagraphOptionData.h \
          structure/ParagraphChild.h \
          structure/TopElement.h \
          PropertyEditorWidget.h PropertyEditorProperty.h \
          PropertyChangeCommand.h StructureChangeCommand.h \
          EditorData.h ReportStructureTreeView.h \
          TableModificationCommand.h \
    DesignerWidget.h \
    ReportTextEdit.h \
    HeaderEditorWidget.h \
    IdModel.h \
    structure/PageBreakTextObject.h \
    TableEditorWidget.h

SOURCES = ReportViewerWindow.cpp ValueAssociationWidget.cpp \
          XmlUtils.cpp ModelEditor.cpp HeaderConverterProxy.cpp \
          kdsignalblocker.cpp ModelInfo.cpp XmlHandler.cpp \
          HeaderDiscardingProxy.cpp KDABBaseProxyModel.cpp \
          ReportObjectXmlElementHandler.cpp ReportStructureWidget.cpp \
          ReportStructureModel.cpp \
          structure/ObjectData.cpp \
          structure/ReportData.cpp structure/HeaderData.cpp \
          structure/FooterData.cpp structure/TextElementData.cpp \
          structure/HtmlElementData.cpp structure/ImageElementData.cpp \
          structure/HLineElementData.cpp \
          structure/PageBreakData.cpp structure/ChartElementData.cpp \
          structure/AutoTableElementData.cpp structure/CellData.cpp \
          structure/TableElementData.cpp structure/VariableData.cpp \
          structure/VSpaceData.cpp structure/ParagraphData.cpp \
          structure/ParagraphOptionData.cpp \
          structure/ParagraphChild.cpp \
          structure/TopElement.cpp \
          PropertyEditorWidget.cpp PropertyEditorProperty.cpp \
          PropertyChangeCommand.cpp StructureChangeCommand.cpp \
          EditorData.cpp ReportStructureTreeView.cpp \
          TableModificationCommand.cpp \
    DesignerWidget.cpp \
    ReportTextEdit.cpp \
    HeaderEditorWidget.cpp \
    IdModel.cpp \
    structure/PageBreakTextObject.cpp \
    TableEditorWidget.cpp

RESOURCES = ReportViewer.qrc

FORMS = ui/TableEditor.ui

win32:RC_FILE = reportviewer.rc
mac {
  ICON = reportviewer.icns
  #QMAKE_INFO_PLIST = Info_mac.plist
}
