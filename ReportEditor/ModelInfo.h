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

#ifndef MODELINFO_H
#define MODELINFO_H

class HeaderDiscardingProxy;
class QAbstractItemModel;
class HeaderConverterProxy;
class QStandardItemModel;
class Test;

#include "editor_export.h"

#include <QString>

/**
 * In the \ref ModelEditor it is possible to edit a number of models. This
 * class represent an edited model.
 *
 * The major challenge in all of this is whether headers should be editable
 * or not. Whether they should be editable or not is decided from the XML
 * file.
 *
 * To face this challenge three models (of which two are proxy models) are
 * in action.
 *
 * m_internalDataModel is the base model containing the actual data. Row 0
 * and Column 0 contains the headers nevertheless if they are visible in
 * the editor or not.
 *
 * The table for editing the data is connected to m_editModel which is a
 * proxy that removes row 0 and/or column 0 depending on if they should be
 * edited or not - that info is obtained from \ref headerVisible.
 *
 * The data is connected to the report through the proxy model m_viewModel,
 * which converts row 0 into horizontal headers, and column 0 into vertical
 * headers.
 */
class EDITOR_EXPORT ModelInfo
{
public:
    ModelInfo( const QString& title );

    void setTitle(const QString &title);
    QString title() const;

    void setDataRowCount( int count );
    int dataRowCount() const;

    void setDataColumnCount( int count );
    int dataColumnCount() const;

    QAbstractItemModel* internalModel();
    QAbstractItemModel* editModel();
    QAbstractItemModel* viewModel();

    void resetHeaderInfo();
    void setHeaderStateInfo( Qt::Orientation, const QString& );
    bool headerVisible( Qt::Orientation ) const;

    void setData( int row, int column, const QString& text );

private:
    QString m_title;
    bool m_headerVisible[3];  // Horizontal = 0x1, Vertical = 0x2
    QStandardItemModel* m_internalDataModel;
    HeaderDiscardingProxy* m_editModel;
    HeaderConverterProxy* m_viewModel;
};

#endif /* MODELINFO_H */
