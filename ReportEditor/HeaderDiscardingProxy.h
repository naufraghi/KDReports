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

#ifndef HEADERDISCARDINGPROXY_H
#define HEADERDISCARDINGPROXY_H

#include "KDABBaseProxyModel.h"
#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>

/**
 * This proxy will remove column 0 and shift all other columns one indexes
 * up, if m_headerVisible[Qt::Horizontal] is true otherwise it is a noop
 * mapping for rows.
 *
 * Same applies to columns.
 *
 * See the \ref ModelInfo class description for details of this setup
 */
class HeaderDiscardingProxy :public KDABBaseProxyModel
{
public:
    HeaderDiscardingProxy( QAbstractItemModel* sourceModel );
    void setHeaderVisible( Qt::Orientation, bool );

    virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

private:
    int horizontalAdjustment() const;
    int verticalAdjustment() const;

    bool m_headerVisible[3]; // Horizontal = 0x1, Vertical = 0x2
};


#endif /* HEADERDISCARDINGPROXY_H */
