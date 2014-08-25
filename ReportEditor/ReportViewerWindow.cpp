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

#include "ReportViewerWindow.h"
#include <KDReportsErrorDetails.h>
#include "XMLHandler.h"
#include <QTime>
#include <QTabBar>
#include "ModelEditor.h"
#include <QDir>
#include <QSettings>
#include <QCloseEvent>
#include "KDReportsErrorDetails.h"
#include "ValueAssociationWidget.h"
#include "XmlUtils.h"
#include <KDReportsPreviewWidget.h>
#include <KDReportsReport.h>
#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QMessageBox>
#include <QXmlStreamReader>
#include "ReportObjectXmlElementHandler.h"
#include "structure/ReportData.h"
#include "structure/TextElementData.h"
#include "structure/TableElementData.h"
#include "structure/ImageElementData.h"
#include "structure/ParagraphChild.h"
#include "structure/ParagraphData.h"
#include "structure/HLineElementData.h"
#include "structure/VSpaceData.h"
#include "structure/AutoTableElementData.h"
#include "ReportStructureWidget.h"
#include "PropertyEditorWidget.h"
#include "ReportStructureModel.h"
#include "StructureChangeCommand.h"
#include "EditorData.h"
#include "DesignerWidget.h"
#include "TableEditorWidget.h"
#include <QInputDialog>

ReportViewerWindow::ReportViewerWindow()
    : QMainWindow( 0 ),
      m_editorData( new EditorData(this) ), m_dirty(false), m_currentView(DesignerWidget::NoView)
{
    static const int s_iconSizes[] = { 16, 22, 32, 48, 64, 128, 256 };
    QIcon icon;
    for (uint i = 0; i < sizeof(s_iconSizes)/sizeof(*s_iconSizes); ++i) {
        const int sz = s_iconSizes[i];
        icon.addFile(":/icon" + QString::number(sz), QSize(sz, sz));
    }
    setWindowIcon(icon);

    setupFileMenu();
    setupEditMenu();
    connect(m_editorData->undoStack(), SIGNAL(canUndoChanged(bool)), m_undoAction, SLOT(setEnabled(bool)));
    connect(m_editorData->undoStack(), SIGNAL(undoTextChanged(QString)), this, SLOT(updateUndoRedoAction()));
    connect(m_editorData->undoStack(), SIGNAL(canRedoChanged(bool)), m_redoAction, SLOT(setEnabled(bool)));
    connect(m_editorData->undoStack(), SIGNAL(redoTextChanged(QString)), this, SLOT(updateUndoRedoAction()));

    setupSampleDataMenu();
    setupToolsMenu();
    setupToolBar();
    changeView(DesignerWidget::Editor);
    // TODO investigate setting a QSizePolicy on m_valueAssociationWidget or its dock
    // so that enlarging the window enlarges that widget, not just the preview area...
    // Jesper: I tried without luck. Mail QtSw support.
    m_designerWidget = new DesignerWidget(m_editorData, this);
    setCentralWidget( m_designerWidget );
    connect(m_designerWidget, SIGNAL(changedView(DesignerWidget::View)), SLOT(changeView(DesignerWidget::View)));
   // resize( 600, 600 );

    loadLayout();
}

ReportViewerWindow::~ReportViewerWindow()
{
    m_valueAssociationWidget->close(); // so that the app quits
    delete m_valueAssociationWidget;
    delete m_editorData;
    delete m_reportStructureWidget;
}

void ReportViewerWindow::slotNew()
{
    checkSave();
    m_editorData->newReport();
    connect(m_editorData->reportData(), SIGNAL(updated(ObjectData*)), this, SLOT(slotUpdate(ObjectData*)));
    m_reportStructureWidget->setEditorData(m_editorData);
    m_designerWidget->setBodyDocument(&m_editorData->document());
    m_valueAssociationWidget->setReportData( m_editorData->reportData());
    m_modelEditor->setReportData( m_editorData->reportData());
}

void ReportViewerWindow::slotClose()
{
    checkSave();
    m_editorData->closeReport();
}

void ReportViewerWindow::checkSave()
{
    if (!m_editorData->isSaved()) {
        QMessageBox::StandardButton answer =
                QMessageBox::question( this, tr("Save before quit"),
                               tr("Save your report?"),
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );

        if ( answer == QMessageBox::Cancel )
            return;
        if ( answer == QMessageBox::Yes )
            slotSave();
    }
}

void ReportViewerWindow::slotOpen()
{
    checkSave();
    const QString xmlFile = QFileDialog::getOpenFileName(
        this, tr( "Open a KDReports XML file" ),
        m_lastVisitedDirectory, tr( "XML files (*.xml)" ) );
    if ( !xmlFile.isEmpty() ) {
        setXmlFile( xmlFile );
        m_lastVisitedDirectory = QFileInfo(xmlFile).absolutePath();
    }
}

void ReportViewerWindow::slotSave()
{
    if (m_xmlFile.isEmpty())
        slotSaveAs();
    else if (m_editorData->reportData()) {
        QFile file(m_xmlFile);
        if (file.open(QFile::WriteOnly)){
            m_editorData->reportData()->saveToXml( &file );
            m_editorData->setSaved();
        }
    } else {
        qWarning() << "No report data";
    }
}

void ReportViewerWindow::slotSaveAs()
{
    if (m_editorData->reportData()) {
        const QString xmlFile = QFileDialog::getSaveFileName(
                    this, tr( "Save a KDReports XML file" ),
                    m_lastVisitedDirectory, tr( "XML files (*.xml)" ) );
        if ( !xmlFile.isEmpty() ) {
            QFile file(xmlFile);
            if (file.open(QFile::WriteOnly)){
                m_lastVisitedDirectory = QFileInfo(xmlFile).absolutePath();
                m_editorData->reportData()->saveToXml( &file );
                m_editorData->setSaved();
                m_xmlFile = xmlFile;
            }
        }
    } else {
        qWarning() << "No report data";
    }
}


void ReportViewerWindow::setXmlFile( const QString& xmlFile )
{
    m_xmlFile = xmlFile;
    reloadReport();
}

void ReportViewerWindow::reloadReport()
{
    QFile file( m_xmlFile );
    if (file.open(QIODevice::ReadOnly)){
        QTextDocument *doc= m_designerWidget->bodyDocument();
        m_designerWidget->setBodyDocument(0);
        if (m_editorData->loadReport(&file)) {
            connect(m_editorData->reportData(), SIGNAL(updated(ObjectData*)), this, SLOT(slotUpdate(ObjectData*)));
            m_valueAssociationWidget->setReportData( m_editorData->reportData());
            m_modelEditor->setReportData( m_editorData->reportData());
            m_reportStructureWidget->setEditorData(m_editorData);
            m_designerWidget->setBodyDocument(&m_editorData->document());
        } else {
            m_designerWidget->setBodyDocument(doc);
            QMessageBox::critical(this, tr("Error loading report"),
                                  tr("Error loading %1: %2").arg(m_xmlFile).arg(m_editorData->errorMessage()));
        }
    } else {
    QMessageBox::critical(this, tr( "Error loading report"),
        tr("Could not open %1 for reading, file does not exist, or permission denied").arg(m_xmlFile));
    }
}

void ReportViewerWindow::slotUpdate(ObjectData* object)
{
    if (m_editorData->selectedObject() == object) {
        m_propertyEditorWidget->changeObjectData();
    }
}

void ReportViewerWindow::loadSampleData( QIODevice* device, KDReports::Report* newReport)
{
    Q_UNUSED(newReport)
    XMLHandler handler;
    KDReports::ErrorDetails details = handler.loadSampleData( device, m_valueAssociationWidget, m_modelEditor );
    if ( details.hasError() )
        QMessageBox::critical( this, tr("Error loading sample data"),
                               tr("Error loading sample data:\n%1").arg(details.message() ) );
}

void ReportViewerWindow::saveSampleData( QIODevice* device ) const
{
    XMLHandler handler;
    handler.saveSampleData( device, m_valueAssociationWidget, m_modelEditor );
}

void ReportViewerWindow::slotNamedAssociationsChanged()
{
    if (m_designerWidget->report()) {
        m_designerWidget->repaintPrev();
        m_dirty = true;
    }
}

void ReportViewerWindow::quit()
{
    if ( m_dirty ) {
        QMessageBox::StandardButton answer =
            QMessageBox::question( this, tr("Save before quit"),
                                   tr("Save sample data?"),
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        if ( answer == QMessageBox::Cancel )
            return;

        if ( answer == QMessageBox::Yes ) {
            if ( !saveSampleData() )
                return;
        }
    }

    checkSave();
    saveLayout();
    qApp->quit();
}

void ReportViewerWindow::closeEvent( QCloseEvent* event )
{
    event->ignore();
    quit();
}

void ReportViewerWindow::saveLayout()
{
    QByteArray data = saveState();
    QSettings settings( QLatin1String("KDAB"), QLatin1String("KDReports") );
    settings.beginGroup( QLatin1String("MainWindow") );
    settings.setValue( QLatin1String("Layout"), data );
    settings.endGroup();
}

void ReportViewerWindow::loadLayout()
{
    QSettings settings( QLatin1String("KDAB"), QLatin1String("KDReports") );
    settings.beginGroup( QLatin1String("MainWindow") );
    if ( settings.contains( QLatin1String("Layout") ) ) {
        QByteArray data = settings.value( QLatin1String("Layout") ).value<QByteArray>();
        restoreState( data );
    }

    settings.endGroup();
}

void ReportViewerWindow::setupFileMenu()
{
    QAction* newAction = new QAction( tr( "&New..." ), this );
    newAction->setShortcut( Qt::CTRL + Qt::Key_N );
    newAction->setIcon( QPixmap( ":/new" ) );
    connect( newAction, SIGNAL( triggered() ), this, SLOT( slotNew() ) );

    QAction* openAction = new QAction( tr( "&Open..." ), this );
    openAction->setShortcut( Qt::CTRL + Qt::Key_O );
    openAction->setIcon( QPixmap( ":/open" ) );
    connect( openAction, SIGNAL( triggered() ), this, SLOT( slotOpen() ) );

    QAction* saveAction = new QAction( tr( "&Save..." ), this );
    saveAction->setShortcut( Qt::CTRL + Qt::Key_S );
    saveAction->setIcon( QPixmap( ":/save" ) );
    connect( saveAction, SIGNAL( triggered() ), this, SLOT( slotSave() ) );

    QAction* saveAsAction = new QAction( tr( "&Save as..." ), this );
    saveAsAction->setIcon( QPixmap( ":/save" ) );
    connect( saveAsAction, SIGNAL( triggered() ), this, SLOT( slotSaveAs() ) );

    QAction* reloadAction = new QAction( tr( "&Reload" ), this );
    reloadAction->setShortcut( Qt::Key_F5 );
    reloadAction->setIcon( QPixmap( ":/refresh" ) );
    connect( reloadAction, SIGNAL( triggered() ), this, SLOT( reloadReport() ) );

    QAction* printAction = new QAction( tr( "&Print..." ), this );
    printAction->setShortcut( Qt::CTRL + Qt::Key_P );
    printAction->setIcon( QPixmap( ":/print" ) );
    connect( printAction, SIGNAL( triggered() ), this, SLOT( slotPrint() ) );

    QAction* quitAction = new QAction( tr( "&Quit" ), this );
    quitAction->setShortcut( Qt::CTRL + Qt::Key_Q );
    quitAction->setIcon( QPixmap( ":/quit" ) );
    connect( quitAction, SIGNAL( triggered() ), this, SLOT( quit() ) );

    QMenu* fileMenu = new QMenu( tr( "&File" ), this );
    menuBar()->addMenu( fileMenu );
    fileMenu->addAction( newAction );
    fileMenu->addAction( openAction );
    fileMenu->addAction( saveAction );
    fileMenu->addAction( saveAsAction );
    fileMenu->addAction( printAction );
    fileMenu->addAction( quitAction );
}

void ReportViewerWindow::setupEditMenu()
{
    m_undoAction = new QAction(this);
    m_redoAction = new QAction(this);
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
    connect(m_undoAction, SIGNAL(triggered()), m_editorData->undoStack(), SLOT(undo()));
    connect(m_redoAction, SIGNAL(triggered()), m_editorData->undoStack(), SLOT(redo()));
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_redoAction->setShortcut(QKeySequence::Redo);
    updateUndoRedoAction();
    QMenu* editMenu = new QMenu( tr( "&Edit" ), this );
    menuBar()->addMenu( editMenu );
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
}

void ReportViewerWindow::updateUndoRedoAction()
{
    if (m_editorData->undoStack()->canUndo())
        m_undoAction->setText(tr( "&Undo %1" ).arg(m_editorData->undoStack()->undoText()));
    else
        m_undoAction->setText(tr( "&Undo" ));

    if (m_editorData->undoStack()->canRedo())
        m_redoAction->setText(tr( "&Redo %1" ).arg(m_editorData->undoStack()->redoText()));
    else
        m_redoAction->setText(tr( "&Redo" ));
}

void ReportViewerWindow::setupToolsMenu()
{
    m_viewMenu = new QMenu( tr( "&View" ), this );
    menuBar()->addMenu( m_viewMenu );

    m_valueAssociationWidget = new ValueAssociationWidget;
    connect( m_valueAssociationWidget, SIGNAL(changed()), this, SLOT(slotNamedAssociationsChanged()) );
    addToolDock( m_valueAssociationWidget, Qt::LeftDockWidgetArea, tr( "Named Values" ), "namedValuesDockWidget", false, DesignerWidget::Views(DesignerWidget::Preview | DesignerWidget::Editor));

    m_modelEditor = new ModelEditor;
    connect( m_modelEditor, SIGNAL(changed()), this, SLOT(modelDataChanged()) );
    addToolDock( m_modelEditor, Qt::LeftDockWidgetArea, tr("Model Editor"), "modelEditorDockWidget", false, DesignerWidget::Views(DesignerWidget::Preview | DesignerWidget::Editor));

    m_reportStructureWidget = new ReportStructureWidget;
    addToolDock( m_reportStructureWidget, Qt::RightDockWidgetArea, tr("Report structure"), "reportStructureDockWidget", true, DesignerWidget::Editor);

    m_propertyEditorWidget = new PropertyEditorWidget(m_editorData);
    addToolDock( m_propertyEditorWidget, Qt::RightDockWidgetArea, tr("Property Editor"), "propertyEditorDockWidget", true, DesignerWidget::Editor);
    connect(m_editorData, SIGNAL(indexChanged(QModelIndex)), m_propertyEditorWidget, SLOT(changeObjectData()));
    connect(m_editorData, SIGNAL(indexChanged(QModelIndex)), m_reportStructureWidget, SLOT(setCurrentIndex(QModelIndex)));

    m_tableEditorWidget = new TableEditorWidget(m_editorData);
    addToolDock( m_tableEditorWidget, Qt::RightDockWidgetArea, tr("Table Editor"), "tableEditorDockWidget", true, DesignerWidget::Editor);
    connect(m_editorData, SIGNAL(indexChanged(QModelIndex)), m_tableEditorWidget, SLOT(changeCurrentElement()));
}

void ReportViewerWindow::setupToolBar()
{
    m_toolBar = addToolBar("Object modificator");
    m_toolBar->setObjectName("Object modificator");
    QMenu* addMenu = new QMenu( tr("&Add Menu"), this );
    addMenu->addAction(QIcon(":/add-text"), tr("Text"), this, SLOT(addText()));
    addMenu->addAction(QIcon(":/add-image"), tr("Image"), this, SLOT(addImage()));
    addMenu->addAction(QIcon(":/add-table"), tr("Table"), this, SLOT(addTable()));
    addMenu->addAction(QIcon(":/add-hline"), tr("Horizontal Line"), this, SLOT(addHLine()));
    addMenu->addAction(tr("Vertical Space"), this, SLOT(addVSpace()));
    addMenu->addAction(tr("Auto Table"), this, SLOT(addAutoTable()));
    QAction *action = m_toolBar->addAction(QIcon(":/add"), tr("Add"));
    QToolButton *button = qobject_cast<QToolButton*>(m_toolBar->widgetForAction(action));
    button->setMenu(addMenu);
    button->setPopupMode(QToolButton::InstantPopup);
    m_toolBar->addAction(QIcon(":/remove"), tr("Remove"), m_editorData, SLOT(removeCurrentObject()));
}


void ReportViewerWindow::addToolDock( QWidget* widget, Qt::DockWidgetArea pos, const QString& title, const char* name, bool show, DesignerWidget::Views views)
{
    QDockWidget* dockWidget = new QDockWidget( title, this );
    dockWidget->setObjectName( name );
    addDockWidget( pos, dockWidget );
    dockWidget->setWidget( widget );
    dockWidget->setVisible(show);
    m_mapDock.insert(dockWidget, views);
}

void ReportViewerWindow::setupSampleDataMenu()
{
    QMenu* menu = new QMenu( tr("&Sample Data"), this );
    menuBar()->addMenu( menu );

    QAction* load = new QAction( tr("&Load"), this );
    load->setIcon( QPixmap( ":/open-data" ) );
    QAction* save = new QAction( tr("&Save"), this );
    save->setIcon( QPixmap( ":/save" ) );
    QAction* saveAs = new QAction( tr("Save &As"), this );
    saveAs->setIcon( QPixmap( ":/save-as" ) );

    menu->addAction( load );
    menu->addAction( save );
    menu->addAction( saveAs );

    connect( load, SIGNAL( triggered() ), this, SLOT( loadSampleData() ) );
    connect( save, SIGNAL( triggered() ), this, SLOT( saveSampleData() ) );
    connect( saveAs, SIGNAL( triggered() ), this, SLOT( saveSampleDataAs() ) );
}

void ReportViewerWindow::loadSampleData()
{
    if ( m_dirty ) {
        QMessageBox::StandardButton answer =
            QMessageBox::question( this, tr("Save before loading new data"),
                                   tr("Save data before loading new data?"),
                                   QMessageBox::Yes | QMessageBox::No );
        if ( answer == QMessageBox::Yes )
            saveSampleData();
    }


    QString fileName = QFileDialog::getOpenFileName( this, tr("Load Sample Data"), QString(),
                                                     QLatin1String( "Data File(*.dat);;All Files (*)" ) );
    if ( fileName.isEmpty() )
        return;

    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly) ) {
        QMessageBox::critical( this, tr("Unable to open file"), tr("Unable to open file \"%1\"\n%2").arg(fileName).arg(file.errorString() ) );
        return;
    }
    loadSampleData( &file, m_editorData->report() );
    m_designerWidget->repaintPrev();
    m_sampleDataFileName = fileName;
    m_dirty = false;
}

bool ReportViewerWindow::saveSampleData()
{
    if ( m_sampleDataFileName.isEmpty() )
        return saveSampleDataAs();
    else {
        QFile file( m_sampleDataFileName );
        if (!file.open(QIODevice::WriteOnly) ) {
            QMessageBox::critical( this, tr("Unable to save file"),
                                   tr("Unable to save to file \"%1\"\n%2")
                                   .arg(m_sampleDataFileName).arg(file.errorString() ) );
            return false;
        }
        saveSampleData( &file );
        m_dirty = false;
        return true;
    }
}

bool ReportViewerWindow::saveSampleDataAs()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Sample Data"), QString(),
                                                     QLatin1String( "Data File(*.dat);;All Files (*)" ) );
    if ( fileName.isEmpty() )
        return false;

    if ( !QDir().exists(fileName) && !fileName.endsWith( QLatin1String(".dat") ) )
        fileName += QLatin1String(".dat");

    m_sampleDataFileName = fileName;
    return saveSampleData();
}

void ReportViewerWindow::modelDataChanged()
{
    m_dirty = true;

    QTime time;
    time.start();
    if (m_designerWidget->report()) {
        m_designerWidget->report()->regenerateAutoTables();
        m_designerWidget->repaintPrev();
    }

    // This code is to ensure that a single slow update doesn't disable auto update immediately.
    static int average = time.elapsed();
    average = average /2.0 + time.elapsed() /2.0;
    //qDebug("%d %d", time.elapsed(), average );

    static bool enabled = true;
    if ( enabled && average > 300 ) {
        m_modelEditor->setAutoUpdate(false);
        enabled = false;
    }

    else if (!enabled && time.elapsed() < 250 ) {
        m_modelEditor->setAutoUpdate( true );
        enabled = true;
        average = time.elapsed();
    }
}

void ReportViewerWindow::slotPrint()
{
    m_editorData->report()->printWithDialog(this);
}

void ReportViewerWindow::addText()
{
    addReportObject(new TextElementData(tr("Default text.")));
}

void ReportViewerWindow::addImage()
{
    QPixmap defaultPixmap(":/image-missing");
    addReportObject(new ImageElementData(ImageElement(defaultPixmap), "default"));
}

void ReportViewerWindow::addHLine()
{
    addReportObject(new HLineElementData(HLineElement()));
}

void ReportViewerWindow::addTable()
{
    bool ok;
    int nbRows = QInputDialog::getInt(this, tr("Create table"),
                                         tr("number of rows:"), 2, 1, 2147483647, 1, &ok);
    if (!ok)
        return;
    int nbColumns = QInputDialog::getInt(this, tr("Create table"),
                                         tr("number of columns:"), 2, 1, 2147483647, 1, &ok);
    if (!ok)
        return;
    TableElementData* table = new TableElementData();
    m_editorData->beginCommand();
    addReportObject(table);
    for (int i = 1; i < nbRows; i++) {
        m_editorData->insertRow(table, 0);
    }
    for (int j = 1; j < nbColumns; j++) {
        m_editorData->insertColumn(table, 0);
    }
    m_editorData->endCommand();
}

void ReportViewerWindow::addVSpace()
{
    addReportObject(new VSpaceData(10));
}

void ReportViewerWindow::addAutoTable()
{
    addReportObject(new AutoTableElementData(AutoTableElement("autoTable"), "autoTable"));
}

void ReportViewerWindow::addReportObject(ObjectData* object)
{
    ObjectData *od = m_editorData->selectedObject();
    if (!od)
        return;
    object->addToHierarchy(od, m_designerWidget->bodyCursorPosition(), m_editorData);
}

void ReportViewerWindow::changeView(DesignerWidget::View view)
{
    if (m_currentView == view)
        return;

    // Manage dock widget
    foreach (QDockWidget* w, m_mapDock.keys()) {
        m_dockVisibility.insert(qMakePair(w, m_currentView), w->isVisible());
    }

    m_viewMenu->clear();
    m_currentView = view;
    foreach (QDockWidget* w, m_mapDock.keys()) {
        if (m_mapDock.value(w).testFlag(view)) {
            m_viewMenu->addAction( w->toggleViewAction());
            w->setVisible(m_dockVisibility.value(qMakePair(w, m_currentView), false));
        } else {
            w->setVisible(false);
        }
    }

    // Manage ID Widget
    if (view == DesignerWidget::Editor) {
        m_valueAssociationWidget->setReport(0);
        m_modelEditor->setReport(0);
    } else if (view == DesignerWidget::Preview) {
        m_valueAssociationWidget->setReport(m_designerWidget->report());
        m_modelEditor->setReport(m_designerWidget->report());
        m_designerWidget->repaintPrev();
    }
}
