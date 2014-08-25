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

#ifndef VALUEASSOCIATIONWIDGET_H
#define VALUEASSOCIATIONWIDGET_H

#include <QWidget>
#include <QXmlStreamWriter> // must include it because the mac uses #define hacks
namespace KDReports { class Report; }
class ReportData;
class QModelIndex;
class QToolButton;
class IdModel;
class QTableView;

/**
 * Property-editor-like widget, where pairs of ID+value can be entered.
 */
class ValueAssociationWidget : public QWidget
{
    Q_OBJECT

    typedef QWidget super;

public:
    explicit ValueAssociationWidget( QWidget* parent = 0 );

    void setReportData(ReportData *reportData);
    void updateAssociationsToReport(  );
    void setReport(KDReports::Report* report);
Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void slotDataChanged( const QModelIndex& from, const QModelIndex& to );

private:
    friend class XMLHandler; // To allow it to get the model for saveing and loading
    void updateAssociation( int row );

    QTableView* m_tableView;
    IdModel* m_model;
    ReportData* m_reportData;
    KDReports::Report* m_report;
};


#endif /* VALUEASSOCIATIONWIDGET_H */
