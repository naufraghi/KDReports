/****************************************************************************
** Copyright (C) 2007-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#ifndef REPORTVIEWERWINDOW_H
#define REPORTVIEWERWINDOW_H

#include "DesignerWidget.h"
#include <QMainWindow>
#include "editor_export.h"
#include <QUndoStack>
#include <QMap>

class ModelEditor;
class QMenu;
class QToolBar;
class ValueAssociationWidget;
class ReportStructureWidget;
class PropertyEditorWidget;
class TableEditorWidget;
class ObjectData;
class EditorData;
namespace KDReports {
class Report;
}
class ReportData;
class QIODevice;

class EDITOR_EXPORT ReportViewerWindow : public QMainWindow
{
    Q_OBJECT
public:
    ReportViewerWindow();
    ~ReportViewerWindow();

    void setXmlFile( const QString& xmlFile );

public Q_SLOTS:
    void changeView(DesignerWidget::View view);

private Q_SLOTS:
    void slotUpdate(ObjectData *object);
    void slotNew();
    void slotClose();
    void slotOpen();
    void slotSave();
    void slotSaveAs();
    void slotPrint();
    void reloadReport();
    void slotNamedAssociationsChanged();
    void modelDataChanged();
    void quit();
    void loadSampleData();
    bool saveSampleData();
    bool saveSampleDataAs();
    void updateUndoRedoAction();
    void addText();
    void addImage();
    void addHLine();
    void addTable();
    void addVSpace();
    void addAutoTable();

protected:
    void closeEvent( QCloseEvent* );

private:
    void checkSave();
    void loadSampleData( QIODevice* device, KDReports::Report* newReport );
    void saveSampleData( QIODevice* device ) const;
    void saveLayout();
    void loadLayout();
    void setupFileMenu();
    void setupEditMenu();
    void setupToolsMenu();
    void setupToolBar();
    void setupSampleDataMenu();
    void addToolDock(QWidget *widget, Qt::DockWidgetArea pos, const QString &title, const char *name, bool show, DesignerWidget::Views views);
    void addReportObject(ObjectData *object);

    DesignerWidget *m_designerWidget;
    ValueAssociationWidget* m_valueAssociationWidget;
    ReportStructureWidget* m_reportStructureWidget;
    PropertyEditorWidget* m_propertyEditorWidget;
    TableEditorWidget* m_tableEditorWidget;
    ModelEditor* m_modelEditor;
    QString m_xmlFile;
    QString m_lastVisitedDirectory;
    QString m_sampleDataFileName;
    EditorData* m_editorData;
    bool m_dirty;
    QMenu* m_viewMenu;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QToolBar* m_toolBar;
    QMap<QDockWidget*, DesignerWidget::Views> m_mapDock;
    QMap< QPair<QDockWidget*, DesignerWidget::View> , bool> m_dockVisibility;
    DesignerWidget::View m_currentView;
};

#endif
