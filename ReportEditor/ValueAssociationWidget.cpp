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

#include "structure/ReportData.h"
#include "ValueAssociationWidget.h"
#include <KDReportsReport>
#include <QHeaderView>
#include <QFile>
#include <QDebug>
#include "XmlUtils.h"
#include <QTableView>
#include "IdModel.h"
#include <QHBoxLayout>
#include <QToolButton>

ValueAssociationWidget::ValueAssociationWidget( QWidget* parent )
    : QWidget( parent )
    , m_reportData(0)
    , m_report(0)
{
    QVBoxLayout* topLayout = new QVBoxLayout( this );
    m_tableView = new QTableView( this );
    topLayout->addWidget( m_tableView );
    m_tableView->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );

    m_model = new IdModel( this );

    connect( m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(slotDataChanged(QModelIndex,QModelIndex)) );

    m_tableView->setModel( m_model );
    m_tableView->horizontalHeader()->setStretchLastSection( true );
}

void ValueAssociationWidget::setReportData( ReportData* reportData)
{
    if (m_reportData != reportData) {
        m_reportData = reportData;
        m_model->setReportData(m_reportData);
    }
}

void ValueAssociationWidget::updateAssociationsToReport()
{
    for( int row=0; row < m_model->rowCount(); ++row ) {
        updateAssociation( row );
    }
}

void ValueAssociationWidget::setReport(KDReports::Report *report)
{
    m_report = report;
    if (m_report)
        updateAssociationsToReport();
}

void ValueAssociationWidget::slotDataChanged( const QModelIndex& from, const QModelIndex& to )
{
    for ( int row = from.row(); row <= to.row(); ++row ) {
        updateAssociation(row);
        emit changed();
    }
}

void ValueAssociationWidget::updateAssociation( int row )
{
    if (!m_report)
        return;
    const QString id = m_model->data( m_model->index( row, 0 ) ).value<QString>();
    const QString value = m_model->data( m_model->index( row, 1 ) ).value<QString>();
    if (!value.isEmpty())
        m_report->associateTextValue( id, value );
}
