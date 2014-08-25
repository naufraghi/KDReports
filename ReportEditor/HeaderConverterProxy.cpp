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

#include "HeaderConverterProxy.h"

HeaderConverterProxy::HeaderConverterProxy( QAbstractItemModel* sourceModel )
    : KDABBaseProxyModel( sourceModel )
{
    setSourceModel( sourceModel );
}

QModelIndex HeaderConverterProxy::mapFromSource( const QModelIndex & sourceIndex ) const
{
    return index( sourceIndex.row()-1, sourceIndex.column()-1 );
}

QModelIndex HeaderConverterProxy::mapToSource( const QModelIndex & proxyIndex ) const
{
    return sourceModel()->index( proxyIndex.row() + 1, proxyIndex.column()+1 );
}

int HeaderConverterProxy::columnCount( const QModelIndex& )  const
{
    return sourceModel()->columnCount()-1;
}

QVariant HeaderConverterProxy::data( const QModelIndex & index, int role ) const
{
    return sourceModel()->data( mapToSource(index), role );
}

QModelIndex HeaderConverterProxy::index( int row, int column, const QModelIndex& ) const
{
    return createIndex( row, column );
}

QModelIndex HeaderConverterProxy::parent( const QModelIndex&  ) const
{
    return QModelIndex();
}

int HeaderConverterProxy::rowCount( const QModelIndex& ) const
{
    return sourceModel()->rowCount()-1;
}

QVariant HeaderConverterProxy::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal )
        return sourceModel()->data( sourceModel()->index( 0, section+1 ), role );
    else
        return sourceModel()->data( sourceModel()->index( section+1, 0 ), role );
}
