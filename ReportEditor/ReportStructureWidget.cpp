/****************************************************************************
** Copyright (C) 2011-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#include "ReportStructureWidget.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QDebug>
#include "structure/ReportData.h"
#include "ReportStructureModel.h"
#include "ReportStructureTreeView.h"
#include "EditorData.h"

ReportStructureWidget::ReportStructureWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* topLayout = new QVBoxLayout( this );
    m_treeView = new ReportStructureTreeView( this );
    topLayout->addWidget( m_treeView );
}

void ReportStructureWidget::setEditorData(EditorData *editorData)
{
    m_editorData = editorData;
    connect(m_treeView, SIGNAL(itemChanged(QModelIndex)), m_editorData, SLOT(changeIndex(QModelIndex)));
    m_treeView->setModel(editorData->model());
}

void ReportStructureWidget::setCurrentIndex(const QModelIndex &index)
{
    m_treeView->setCurrentIndex(index);
}
