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

#include "ModelInfo.h"
#include "HeaderDiscardingProxy.h"
#include <QDebug>
#include "HeaderConverterProxy.h"
#include <QStandardItemModel>

ModelInfo::ModelInfo( const QString& title )
    : m_title( title ),
      m_internalDataModel( new QStandardItemModel ),
      m_editModel( new HeaderDiscardingProxy( m_internalDataModel ) ),
      m_viewModel( new HeaderConverterProxy( m_internalDataModel ) )
{
    setDataRowCount(1);
    setDataColumnCount(1);
    resetHeaderInfo();
}

void ModelInfo::setTitle(const QString &title)
{
    m_title = title;
}

QString ModelInfo::title() const
{
    return m_title;
}

QAbstractItemModel* ModelInfo::editModel()
{
    return m_editModel;
}

void ModelInfo::setDataRowCount( int count )
{
    m_internalDataModel->setRowCount( count + 1 );
}

void ModelInfo::setDataColumnCount( int count )
{
    m_internalDataModel->setColumnCount( count + 1 );
}

int ModelInfo::dataRowCount() const
{
    return m_internalDataModel->rowCount() - 1;
}

int ModelInfo::dataColumnCount() const
{
    return m_internalDataModel->columnCount() - 1;
}

QAbstractItemModel* ModelInfo::viewModel()
{
    return m_viewModel;
}

bool ModelInfo::headerVisible( Qt::Orientation orient) const
{
    return m_headerVisible[orient];
}

QAbstractItemModel* ModelInfo::internalModel()
{
    return m_internalDataModel;
}

/**
 * A model may be used several places in a report, some places with the
 * header visible, some places without, that is what makes the decition on
 * whether a header should be shown or not "difficult".
 *
 * The logic behind if headers should be shown is this (and this applies
 * both to horizontal and vertical headers, read from the attribute
 * horizontalHeaderVisible and verticalHeaderVisible):
 *
 * <li> Nothing specified means do show
 * <li> Only hide the header if all instances of the model has specified
 * that it should not be shown.
 */
void ModelInfo::setHeaderStateInfo( Qt::Orientation orient, const QString& str)
{
    // Notice it starts out false, but once it bounces to true, it will
    // stay true, untill reset.
    if ( str != "false" )
        m_headerVisible[orient] = true;
    m_editModel->setHeaderVisible( orient, m_headerVisible[orient] );
}

void ModelInfo::resetHeaderInfo()
{
    m_headerVisible[Qt::Vertical] = false;
    m_headerVisible[Qt::Horizontal] = false;
}
