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

#ifndef MODELEDITOR_H
#define MODELEDITOR_H

#include "editor_export.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSpinBox>
#include <QSet>
#include <QMap>
#include <QWidget>
class QPushButton;
class QTimer;
class ModelInfo;
class QTableView;
class QTabBar;
class QStandardItemModel;
class QTreeWidgetItem;
class ReportData;

namespace KDReports { class Report; }

/**
 * In the ReportViewer it is possible to fill the models specified in the
 * XML file in a table view. This class sets up this table view, and
 * associate models to it.
 */
class EDITOR_EXPORT ModelEditor :public QWidget
{
    Q_OBJECT
public:
    explicit ModelEditor( QWidget* parent = 0 );
    void setReport( KDReports::Report* report );
    void setAutoUpdate( bool );
    void setReportData(ReportData *reportData);

public slots:
    void slotAddModel(const QString &model);
    void slotRemoveModel(const QString &model);
    void slotModifyModel(const QString &newModel, const QString &oldModel);

Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void modelChanged( int index );
    void rowsChanged( int count );
    void columnsChanged( int count );
    void dataChanged();

private:
    friend class XMLHandler; // For loading and saving
    friend class TestModelAssociation; // For unit test
    void createUI();
    void removeModel(const QString &model);
    void addModel( const QString &model );
    void modifyModel(const QString &newTitle, const QString &oldTitle);

    bool m_autoUpdate;
    KDReports::Report* m_report;
    ReportData* m_reportData;

    ModelInfo* m_currentModelInfo;
    typedef QMap<QString, ModelInfo*> ModelInfoMap;
    ModelInfoMap m_modelInfos;

    QTimer* m_changeTimer;
    QPushButton* m_update;
    QTabBar* m_tabBar;
    QSpinBox* m_rows;
    QSpinBox* m_columns;
    QTableView* m_modelData;
};

#endif /* MODELEDITOR_H */
