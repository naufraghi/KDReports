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

#ifndef TABLEELEMENTDATA_H
#define TABLEELEMENTDATA_H

#include "KDReportsTableElement.h"
#include "ObjectData.h"
#include "ParagraphChild.h"
#include <QBrush>

class CellData;

class EDITOR_EXPORT TableElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QBrush background READ background WRITE setBackground)
    Q_PROPERTY (double border READ border WRITE setBorder)
    Q_PROPERTY (double width READ width WRITE setWidth)
    Q_PROPERTY (int headerRowCount READ headerRowCount WRITE setHeaderRowCount)
    Q_PROPERTY (double cellpadding READ cellpadding WRITE setCellpadding)

public:
    explicit TableElementData(const KDReports::TableElement &tableElement = KDReports::TableElement());
    QBrush background() const;
    void setBackground(const QBrush& brush);
    double border() const;
    void setBorder(double border);
    double width() const;
    void setWidth(double width);
    int headerRowCount() const;
    void setHeaderRowCount(int headerRowCount);
    double cellpadding() const;
    void setCellpadding(double cellpadding);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int startPosition() const;
    virtual int endPosition() const;
    virtual int elementSize() const;
    virtual void doAddToReport();
    virtual void doRemove();
    virtual void doUpdate();
    QTextTable* textTable();
    void createCell(int row, int column);
    void insertRows(int pos, int number);
    void removeRows(int pos, int number);
    void insertColumn(int pos, int number);
    void removeColumns(int pos, int number);
    CellData * cellAt(int row, int column);
    int columns() const;
    int rows() const;
    virtual void validate();
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

private:
    QTextTableFormat tableFormat() const;
    QTextBlockFormat blockFormat() const;
    friend class CellData; // Need access to m_tableElement
    KDReports::TableElement m_tableElement;
    QTextTable* m_textTable;
};

#endif // TABLEELEMENTDATA_H
