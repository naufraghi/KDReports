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

#include "HeaderDiscardingProxy.h"
#include <QColor>

HeaderDiscardingProxy::HeaderDiscardingProxy( QAbstractItemModel* sourceModel )
    : KDABBaseProxyModel( sourceModel )
{
    setSourceModel( sourceModel );
    m_headerVisible[Qt::Vertical] = true;
    m_headerVisible[Qt::Horizontal] = true;
}

void HeaderDiscardingProxy::setHeaderVisible( Qt::Orientation orient, bool b)
{
    m_headerVisible[orient] = b;
    reset();
}

QModelIndex HeaderDiscardingProxy::mapFromSource( const QModelIndex & sourceIndex ) const
{
    if ( !sourceIndex.isValid() )
        return QModelIndex();
    return index( sourceIndex.row() - horizontalAdjustment(), sourceIndex.column() + verticalAdjustment() );
}

QModelIndex HeaderDiscardingProxy::mapToSource( const QModelIndex & proxyIndex ) const
{
    if ( !proxyIndex.isValid() )
        return QModelIndex();

    return sourceModel()->index( proxyIndex.row() + horizontalAdjustment(), proxyIndex.column()+verticalAdjustment() );
}

QVariant HeaderDiscardingProxy::data( const QModelIndex & index, int role ) const
{
    if ( index.row() == 0 && index.column() == 0 && m_headerVisible[Qt::Horizontal] && m_headerVisible[Qt::Vertical] && role == Qt::BackgroundRole)
        return QColor( Qt::gray ).lighter(); // This is the corner cell that should not be editable
    return sourceModel()->data( mapToSource(index), role );
}

bool HeaderDiscardingProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return sourceModel()->setData(mapToSource(index), value, role);
}


QModelIndex HeaderDiscardingProxy::index( int row, int column, const QModelIndex&  ) const
{
    return createIndex( row, column );
}

int HeaderDiscardingProxy::rowCount( const QModelIndex&  ) const
{
    return sourceModel()->rowCount()-horizontalAdjustment();
}

int HeaderDiscardingProxy::columnCount( const QModelIndex& ) const
{
    return sourceModel()->columnCount()-verticalAdjustment();
}

QVariant HeaderDiscardingProxy::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal ) {
        if ( section == 0 && m_headerVisible[Qt::Vertical] )
            return tr("Title");
        else
            return QString::number( section + verticalAdjustment() );
    }
    else {
        if ( section == 0 && m_headerVisible[Qt::Horizontal] )
            return tr("Title");
        else
            return QString::number( section + horizontalAdjustment() );
    }
}

int HeaderDiscardingProxy::horizontalAdjustment() const
{
    return m_headerVisible[Qt::Horizontal] ? 0 : 1;
}

int HeaderDiscardingProxy::verticalAdjustment() const
{
    return m_headerVisible[Qt::Vertical] ? 0 : 1;
}

QModelIndex HeaderDiscardingProxy::parent( const QModelIndex& ) const
{
    return QModelIndex();
}

Qt::ItemFlags HeaderDiscardingProxy::flags( const QModelIndex & index ) const
{
    if ( index.row() == 0 && index.column() == 0 && m_headerVisible[Qt::Horizontal] && m_headerVisible[Qt::Vertical] )
        return 0; // This is the corner cell that should not be editable
    else
        return KDABBaseProxyModel::flags( index );
}
