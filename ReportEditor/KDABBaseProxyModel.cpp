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

#include "KDABBaseProxyModel.h"
#include <QAbstractItemModel>







KDABBaseProxyModel::KDABBaseProxyModel( QObject* parent )
    : QAbstractProxyModel( parent )
{
}

void KDABBaseProxyModel::setSourceModel( QAbstractItemModel * sourceModel )
{
    if ( this->sourceModel() ) {
        disconnect( this->sourceModel(), SIGNAL( columnsAboutToBeInserted ( QModelIndex , int , int ) ),
                    this, SLOT( slotColumnsAboutToBeInserted ( QModelIndex , int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( columnsAboutToBeRemoved( QModelIndex, int, int ) ),
                    this, SLOT( slotColumnsAboutToBeRemoved( QModelIndex, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( columnsInserted( QModelIndex, int, int ) ),
                    this, SLOT( slotColumnsInserted( QModelIndex, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( columnsRemoved( QModelIndex, int, int ) ),
                    this, SLOT( slotColumnsRemoved( QModelIndex, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
                    this, SLOT( slotDataChanged( QModelIndex, QModelIndex ) ) );
        disconnect( this->sourceModel(), SIGNAL( headerDataChanged( Qt::Orientation, int, int ) ),
                    this, SLOT( slotHeaderDataChanged( Qt::Orientation, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( layoutAboutToBeChanged() ),
                    this, SLOT( slotLayoutAboutToBeChanged() ) );
        disconnect( this->sourceModel(), SIGNAL( layoutChanged() ),
                    this, SLOT( slotLayoutChanged() ) );
        disconnect( this->sourceModel(), SIGNAL( modelAboutToBeReset() ),
                    this, SLOT( slotModelAboutToBeReset() ) );
        disconnect( this->sourceModel(), SIGNAL( modelReset() ),
                    this, SLOT( slotModelReset() ) );
        disconnect( this->sourceModel(), SIGNAL( rowsAboutToBeInserted( QModelIndex, int,int ) ),
                    this, SLOT( slotRowsAboutToBeInserted( QModelIndex, int,int ) ) );
        disconnect( this->sourceModel(), SIGNAL( rowsAboutToBeRemoved( QModelIndex, int, int ) ),
                    this, SLOT( slotRowsAboutToBeRemoved( QModelIndex, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( rowsInserted( QModelIndex, int, int ) ),
                    this, SLOT( slotRowsInserted( QModelIndex, int, int ) ) );
        disconnect( this->sourceModel(), SIGNAL( rowsRemoved( QModelIndex, int, int) ),
                    this, SLOT( slotRowsRemoved( QModelIndex, int, int) ) );
    }

    connect( sourceModel, SIGNAL( columnsAboutToBeInserted ( QModelIndex , int , int ) ),
             this, SLOT( slotColumnsAboutToBeInserted ( QModelIndex , int, int ) ) );
    connect( sourceModel, SIGNAL( columnsAboutToBeRemoved( QModelIndex, int, int ) ),
             this, SLOT( slotColumnsAboutToBeRemoved( QModelIndex, int, int ) ) );
    connect( sourceModel, SIGNAL( columnsInserted( QModelIndex, int, int ) ),
             this, SLOT( slotColumnsInserted( QModelIndex, int, int ) ) );
    connect( sourceModel, SIGNAL( columnsRemoved( QModelIndex, int, int ) ),
             this, SLOT( slotColumnsRemoved( QModelIndex, int, int ) ) );
    connect( sourceModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
             this, SLOT( slotDataChanged( QModelIndex, QModelIndex ) ) );
    connect( sourceModel, SIGNAL( headerDataChanged( Qt::Orientation, int, int ) ),
             this, SLOT( slotHeaderDataChanged( Qt::Orientation, int, int ) ) );
    connect( sourceModel, SIGNAL( layoutAboutToBeChanged() ),
             this, SLOT( slotLayoutAboutToBeChanged() ) );
    connect( sourceModel, SIGNAL( layoutChanged() ),
             this, SLOT( slotLayoutChanged() ) );
    connect( sourceModel, SIGNAL( modelAboutToBeReset() ),
             this, SLOT( slotModelAboutToBeReset() ) );
    connect( sourceModel, SIGNAL( modelReset() ),
             this, SLOT( slotModelReset() ) );
    connect( sourceModel, SIGNAL( rowsAboutToBeInserted( QModelIndex, int,int ) ),
             this, SLOT( slotRowsAboutToBeInserted( QModelIndex, int,int ) ) );
    connect( sourceModel, SIGNAL( rowsAboutToBeRemoved( QModelIndex, int, int ) ),
             this, SLOT( slotRowsAboutToBeRemoved( QModelIndex, int, int ) ) );
    connect( sourceModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             this, SLOT( slotRowsInserted( QModelIndex, int, int ) ) );
    connect( sourceModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ),
             this, SLOT( slotRowsRemoved( QModelIndex, int, int) ) );

    QAbstractProxyModel::setSourceModel( sourceModel );
}



void KDABBaseProxyModel::slotColumnsAboutToBeInserted( const QModelIndex & parent, int start, int end )
{
    int min, max;
    identifyColumnRange( start, end, parent, &min, &max );
    beginInsertColumns( mapFromSource(parent), min, max );
}

void KDABBaseProxyModel::slotColumnsAboutToBeRemoved( const QModelIndex & parent, int start, int end )
{
    int min, max;
    identifyColumnRange( start, end, parent, &min, &max );
    beginRemoveColumns( mapFromSource(parent), min, max );
}

void KDABBaseProxyModel::slotColumnsInserted( const QModelIndex&, int, int  )
{
    endInsertColumns();
}

void KDABBaseProxyModel::slotColumnsRemoved( const QModelIndex&, int, int  )
{
    endRemoveColumns();
}

void KDABBaseProxyModel::slotDataChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    int minRow, minColumn, maxRow, maxColumn;
    Q_ASSERT( topLeft.parent() == bottomRight.parent() );
    identifyRowRange( topLeft.row(), bottomRight.row(), topLeft.parent(), &minRow, &maxRow );
    identifyColumnRange( topLeft.column(), bottomRight.column(), topLeft.parent(), &minColumn, &maxColumn );
    emit dataChanged( index( minRow, minColumn, mapFromSource( topLeft.parent() ) ),
                      index( maxRow, maxColumn, mapFromSource( topLeft.parent() ) ) );
}

void KDABBaseProxyModel::slotHeaderDataChanged( Qt::Orientation orientation, int first, int last )
{
    int min, max;
    if ( orientation == Qt::Horizontal )
        identifyRowRange( first, last, QModelIndex(), & min, &max );
    else
        identifyColumnRange( first, last, QModelIndex(), &min, &max );

    emit headerDataChanged( orientation, min, max );
}

void KDABBaseProxyModel::slotLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void KDABBaseProxyModel::slotLayoutChanged()
{
    emit layoutChanged();
}

void KDABBaseProxyModel::slotModelAboutToBeReset()
{
    reset();
}

void KDABBaseProxyModel::slotModelReset()
{
    // There isn't much else I can do, I can't emit modelAboutToBeReset and modelReset, so all I can do is call reset in the above slot.
}

void KDABBaseProxyModel::slotRowsAboutToBeInserted( const QModelIndex & parent, int start, int end )
{
    int min, max;
    identifyRowRange( start, end, parent, &min, &max );
    beginInsertRows( mapFromSource(parent), min, max );
}

void KDABBaseProxyModel::slotRowsAboutToBeRemoved( const QModelIndex & parent, int start, int end )
{
    int min, max;
    identifyRowRange( start, end, parent, &min, &max );
    beginRemoveRows( mapFromSource(parent), min, max );
}

void KDABBaseProxyModel::slotRowsInserted( const QModelIndex& /*parent*/, int /*start*/, int /*end*/ )
{
    endInsertRows();
}

void KDABBaseProxyModel::slotRowsRemoved( const QModelIndex& /*parent*/, int /*start*/, int /*end*/ )
{
    endRemoveRows();
}

void KDABBaseProxyModel::identifyRowRange( int start, int end, const QModelIndex& parent, int* myStart, int* myEnd )
{
    if ( start > end )
        qSwap( start, end );

    int min = mapFromSource( index( start, 0, parent ) ).row();
    int max = min;
    for ( int i = start+1; i<= end; ++i ) {
        int cur = mapFromSource( index( i, 0, parent ) ).row();
        min = qMin( min, cur );
        max = qMax( max, cur );
    }
    *myStart = qMax( min, 0 );
    *myEnd = qMax( max, 0 );

}

void KDABBaseProxyModel::identifyColumnRange( int start, int end, const QModelIndex& parent, int* myStart, int* myEnd )
{
    if ( start > end )
        qSwap( start, end );

    int min = mapFromSource( index( 0,start, parent ) ).column();
    int max = min;
    for ( int i = start+1; i<= end; ++i ) {
        int cur = mapFromSource( index( 0, i, parent ) ).column();
        min = qMin( min, cur );
        max = qMax( max, cur );
    }
    *myStart = qMax( min, 0 );
    *myEnd = qMax( max, 0 );

}

#include "moc_KDABBaseProxyModel.cpp"
