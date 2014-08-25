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

#include "ModelEditor.h"
#include "XMLHandler.h"
#include <QTimer>
#include "ModelInfo.h"
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QTabBar>
#include "HeaderConverterProxy.h"
#include <QStandardItemModel>
#include <QDebug>
#include "XmlUtils.h"
#include <KDReportsReport.h>
#include "kdsignalblocker.h"
#include <limits>
#include <structure/ReportData.h>

Q_DECLARE_METATYPE( ModelInfo* )

ModelEditor::ModelEditor( QWidget* parent )
    : QWidget( parent), m_autoUpdate( true ), m_report(0), m_reportData(0)
{
    createUI();

    connect( m_tabBar, SIGNAL( currentChanged( int ) ),
             this, SLOT( modelChanged( int ) ) );

    connect( m_rows, SIGNAL( valueChanged( int ) ), this, SLOT( rowsChanged( int ) ) );
    connect( m_columns, SIGNAL( valueChanged( int ) ), this, SLOT( columnsChanged( int ) ) );

    m_changeTimer = new QTimer(this);
    m_changeTimer->setSingleShot(true);

    connect( m_changeTimer, SIGNAL( timeout() ), this, SIGNAL( changed() ) );
}

void ModelEditor::setReportData(ReportData* reportData)
{
    if (m_reportData != reportData) {
        m_reportData = reportData;
        foreach (QString model, m_reportData->modelKeyMap().uniqueKeys()) {
            addModel( model );
        }
        connect (m_reportData, SIGNAL(modelAdded(QString)), SLOT(slotAddModel(QString)));
        connect (m_reportData, SIGNAL(modelRemoved(QString)), SLOT(slotRemoveModel(QString)));
        connect (m_reportData, SIGNAL(modelModified(QString,QString)), SLOT(slotModifyModel(QString,QString)));
    }
}

void ModelEditor::slotAddModel(const QString &model)
{
    addModel(model);
}

void ModelEditor::slotRemoveModel(const QString &model)
{
    removeModel(model);
}

void ModelEditor::slotModifyModel(const QString &newModel, const QString &oldModel)
{
    modifyModel(newModel, oldModel);
}

void ModelEditor::setReport( KDReports::Report* report )
{
    m_report = report;
    if (m_report) {
        for ( int index = 0; index < m_tabBar->count(); index++ ) {
            ModelInfo* modelInfo = m_tabBar->tabData(index).value<ModelInfo*>();
            m_report->associateModel(modelInfo->title(), modelInfo->viewModel());
        }
    }
}

void ModelEditor::modelChanged( int index )
{
    // Without this call the application will hit an assert if a cell is
    // being edited at a position that doesn't exists in the new model.
    m_modelData->reset();

    m_currentModelInfo = m_tabBar->tabData( index ).value<ModelInfo*>();
    if ( ! m_currentModelInfo )
        return;

    m_modelData->setModel(m_currentModelInfo->editModel());

    KDSignalBlocker rowBlocker( m_rows );
    KDSignalBlocker columnsBlocker( m_columns );
    m_rows->setValue( m_currentModelInfo->dataRowCount() );
    m_columns->setValue( m_currentModelInfo->dataColumnCount() );
}

void ModelEditor::addModel( const QString& model )
{
    if (m_modelInfos.contains(model))
        return;
    ModelInfo* modelInfo = new ModelInfo( model );
    connect( modelInfo->editModel(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
             this, SLOT( dataChanged() ) );
    m_modelInfos.insert( model, modelInfo );

    int index = m_tabBar->addTab( modelInfo->title() );

    m_tabBar->setTabData( index, QVariant::fromValue<ModelInfo*>( modelInfo ) );
    m_currentModelInfo = modelInfo;
    m_modelData->setModel( m_currentModelInfo->editModel() );
    if (m_report)
        m_report->associateModel( modelInfo->title(), modelInfo->viewModel() );
}

void ModelEditor::modifyModel( const QString &newTitle, const QString &oldTitle)
{
    if (m_modelInfos.contains(newTitle)) {
        removeModel(oldTitle);
    } else {
        ModelInfo* modelInfo = m_modelInfos.value(oldTitle);
        modelInfo->setTitle(newTitle);
        for ( int index = 0; index < m_tabBar->count(); index++ ) {
            ModelInfo* modelInfo2 = m_tabBar->tabData(index).value<ModelInfo*>();
            if (modelInfo2 == modelInfo)
                m_tabBar->setTabText(index, newTitle);
        }
        m_modelInfos.remove(oldTitle);
        m_modelInfos.insert(newTitle, modelInfo);
    }
}

void ModelEditor::removeModel(const QString &model)
{
    ModelInfo* modelInfo = m_modelInfos.value(model);
    for ( int index = 0; index < m_tabBar->count(); index++ ) {
        ModelInfo* info = m_tabBar->tabData(index).value<ModelInfo*>();
        if ( info == modelInfo ) {
            m_tabBar->removeTab(index);
            m_modelInfos.remove(info->title());
            delete info;
            break;
        }
    }
}

void ModelEditor::rowsChanged( int count )
{
    m_currentModelInfo->setDataRowCount( count );
    dataChanged();
}

void ModelEditor::columnsChanged( int count )
{
    m_currentModelInfo->setDataColumnCount( count );
    dataChanged();
}

void ModelEditor::createUI()
{
    QVBoxLayout* topLay = new QVBoxLayout( this );
    m_tabBar = new QTabBar( this );

    topLay->addWidget( m_tabBar );

    QHBoxLayout* hlay = new QHBoxLayout;
    topLay->addLayout( hlay );

    QLabel* label = new QLabel( "Rows:" );
    hlay->addWidget( label );

    m_rows = new QSpinBox;
    m_rows->setMinimum(1);
    m_rows->setMaximum( std::numeric_limits<int>::max() );
    hlay->addWidget( m_rows );

    label = new QLabel( "Columns:" );
    hlay->addWidget( label );

    m_columns = new QSpinBox;
    m_columns->setMinimum(1);
    m_columns->setMaximum( std::numeric_limits<int>::max() );
    hlay->addWidget( m_columns );
    hlay->addStretch( 1 );

    m_modelData = new QTableView;
    topLay->addWidget( m_modelData );

    m_update = new QPushButton( tr("Update") );
    m_update->setEnabled(false);
    topLay->addWidget( m_update );
    connect( m_update, SIGNAL( clicked() ), this, SIGNAL( changed() ) );
}

void ModelEditor::dataChanged()
{
    // Due to what I believe is a bug in QAbstractItem model (and my usage
    // of reset), I can't emit the change signal directly.
    // If I do so, then it will crash when a cell is edited, and I change tab.
    if ( m_autoUpdate )
        m_changeTimer->start( 0 );
}

void ModelEditor::setAutoUpdate( bool b )
{
    m_autoUpdate = b;
    m_update->setEnabled( !b );
}
