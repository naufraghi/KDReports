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

#ifndef CELLDATA_H
#define CELLDATA_H

#include "KDReportsCell.h"
#include "ObjectData.h"
#include <QBrush>
#include <QTextTableCell>

class TableElementData;
class InteractionLayer;

class EDITOR_EXPORT CellData : public ObjectData
{
    Q_OBJECT
    Q_PROPERTY (int rowSpan READ rowSpan WRITE setRowSpan)
    Q_PROPERTY (int columnSpan READ columnSpan WRITE setColumnSpan)
    Q_PROPERTY (QBrush background READ background WRITE setBackground)

public:
    explicit CellData(TableElementData* parent, int row, int column, const QBrush &background = QBrush());
    int rowSpan() const;
    void setRowSpan(int rowSpan);
    int columnSpan() const;
    void setColumnSpan(int columnSpan);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    void updateCell(int row, int column);
    void validate();
    void updateRowColumnCache();
    bool isValid() const;
    int row() const;
    int column() const;
    virtual QRect elementRect(ReportTextEdit *reportTextEdit);
    virtual void doRemove();
    virtual void doAddToReport();
    virtual bool supportParagraph() const;
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    virtual int startPosition() const;
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

private:
    qreal maxHeight(ReportTextEdit *reportTextEdit);
    friend class TableElementData;
    friend class ReportTextEdit;
    TableElementData* m_table;
    QTextTableCell m_cell;
    int m_columnSpan;
    int m_rowSpan;
    int m_row;
    int m_column;
    bool m_valid;
    QBrush m_background;
};

#endif // CELLDATA_H
