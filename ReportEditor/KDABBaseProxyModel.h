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

#ifndef KDABBASEPROXYMODEL_H
#define KDABBASEPROXYMODEL_H

#include <QAbstractProxyModel>

/**
 * This proxy is a base proxy for proxies. Its main purpose is to ensure
 * that signals are correctly emitted.
 */
class KDABBaseProxyModel :public QAbstractProxyModel
{
Q_OBJECT

public:
    KDABBaseProxyModel( QObject* parent );
    virtual void setSourceModel(  QAbstractItemModel * sourceModel );

private slots:
    void slotColumnsAboutToBeInserted ( const QModelIndex & parent, int start, int end );
    void slotColumnsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
    void slotColumnsInserted ( const QModelIndex & parent, int start, int end );
    void slotColumnsRemoved ( const QModelIndex & parent, int start, int end );
    void slotDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void slotHeaderDataChanged ( Qt::Orientation orientation, int first, int last );
    void slotLayoutAboutToBeChanged ();
    void slotLayoutChanged ();
    void slotModelAboutToBeReset ();
    void slotModelReset ();
    void slotRowsAboutToBeInserted ( const QModelIndex & parent, int start, int end );
    void slotRowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
    void slotRowsInserted ( const QModelIndex & parent, int start, int end );
    void slotRowsRemoved ( const QModelIndex & parent, int start, int end );

private:
    void identifyRowRange( int start, int end, const QModelIndex& parent, int* myStart, int* myEnd );
    void identifyColumnRange( int start, int end, const QModelIndex& parent, int* myStart, int* myEnd );
};

#endif /* KDABBASEPROXYMODEL_H */
