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

#ifndef HEADERCONVERTERPROXY_H
#define HEADERCONVERTERPROXY_H

#include "KDABBaseProxyModel.h"

/**
 * This proxy converts row zero of the source model into horizontal header
 * data, row 1 into row 0 etc, and similar for columns, i.e. column 0
 * becomes vertical header data etc.
 *
 * See the \ref ModelInfo class description for details of this setup
 */
class HeaderConverterProxy :public KDABBaseProxyModel
{
public:
    HeaderConverterProxy( QAbstractItemModel* sourceModel);
    void showHeaders( bool horizontal, bool vertical );

    virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
};

#endif /* HEADERCONVERTERPROXY_H */
