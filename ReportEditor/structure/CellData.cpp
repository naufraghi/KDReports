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

#include "CellData.h"
#include "TableElementData.h"
#include "TextElementData.h"
#include "ParagraphChild.h"
#include "../EditorData.h"
#include <QTextTable>
#include <QTextTableCell>
#include <QDebug>
#include <QTextDocument>
#include "ReportTextEdit.h"
#include <QAbstractTextDocumentLayout>

CellData::CellData(TableElementData* parent, int row, int column, const QBrush &background)
    : ObjectData(parent)
    , m_table(parent)
    , m_row(row)
    , m_column(column)
    , m_valid(true)
    , m_background(background)
{
    setObjectName(tr("cell"));
    if (m_table->m_textTable) {
        m_cell = m_table->m_textTable->cellAt(row, column);
        m_rowSpan = m_cell.rowSpan();
        m_columnSpan = m_cell.columnSpan();
    } else {
        m_valid = false;
    }
}

int CellData::rowSpan() const
{
    return m_rowSpan;
}

void CellData::setRowSpan(int rowSpan)
{
    if (m_cell.rowSpan() < rowSpan) {
        m_table->m_textTable->mergeCells(m_cell.row(), m_cell.column(), rowSpan, m_cell.columnSpan());
    } else if (m_cell.rowSpan() > rowSpan) {
        int column = m_cell.columnSpan();
        m_table->m_textTable->splitCell(m_cell.row(), m_cell.column(), 1, 1);
        m_table->m_textTable->mergeCells(m_cell.row(), m_cell.column(), rowSpan, column);
    }
    m_rowSpan = m_cell.rowSpan();
    // else rowSpan == cell.rowSpan and we don't need to modify table structure
}

int CellData::columnSpan() const
{
    return m_columnSpan;
}

void CellData::setColumnSpan(int columnSpan)
{
    if (m_cell.columnSpan() < columnSpan) {
        m_table->m_textTable->mergeCells(m_cell.row(), m_cell.column(), m_cell.rowSpan(), columnSpan);
    } else if (m_cell.columnSpan() > columnSpan) {
        int row = m_cell.rowSpan();
        m_table->m_textTable->splitCell(m_cell.row(), m_cell.column(), 1, 1);
        m_table->m_textTable->mergeCells(m_cell.row(), m_cell.column(), row, columnSpan);
    }
    m_columnSpan = m_cell.columnSpan();
    // else columnSpan == cell.columnSpan and we don't need to modify table structure
}

QBrush CellData::background() const
{
    return m_background;
}

void CellData::setBackground(const QBrush &brush)
{
    m_background = brush;
}

void CellData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("cell"));
    writer.writeAttribute(QString::fromLatin1("row"), QString::number(m_cell.row()));
    writer.writeAttribute(QString::fromLatin1("column"), QString::number(m_cell.column()));
    if (rowSpan() != 1)
        writer.writeAttribute(QString::fromLatin1("rowspan"), QString::number(rowSpan()));
    if (columnSpan() != 1)
        writer.writeAttribute(QString::fromLatin1("colspan"), QString::number(columnSpan()));
    if (background().style() != Qt::NoBrush && background().color().isValid())
        writer.writeAttribute(QString::fromLatin1("background"), background().color().name());

    foreach(ObjectData* od, childrenElements()){
        od->writeXml(writer);
    }
    writer.writeEndElement();
}

int CellData::endPosition() const
{
    return m_cell.lastCursorPosition().position() + 1;
}

int CellData::startPosition() const
{
    return m_cell.firstCursorPosition().position();
}

QString CellData::translatePropertyName(const QString &name) const
{
    if (name == "rowSpan")
        return tr("Row span");
    else if (name == "columnSpan")
        return tr("Column span");
    else if (name == "background")
        return tr("Background");
    return ObjectData::translatePropertyName(name);
}

QVariant CellData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "rowSpan" || name == "columnSpam") && type == ObjectData::MIN)
        return 0;
    return ObjectData::propertyValue(name, type);
}

void CellData::doRemove()
{
    foreach (ObjectData* od, childrenElements()) {
        od->doRemove();
    }
}

void CellData::doAddToReport()
{
    QTextCharFormat tableCellFormat;
    if (background().color().isValid())
        tableCellFormat.setBackground(background());
    tableCellFormat.setTableCellColumnSpan(columnSpan());
    tableCellFormat.setTableCellRowSpan(rowSpan());
    if ( columnSpan() > 1 || rowSpan() > 1 )
        m_table->m_textTable->mergeCells( m_row, m_column, rowSpan(), columnSpan());
    m_cell.setFormat(tableCellFormat);
    QTextCursor cellCursor = m_cell.firstCursorPosition();
    cellCursor.setCharFormat(tableCellFormat);
    foreach (ObjectData* od, childrenElements()) {
        od->doAddToReport();
    }
}

void CellData::updateCell(int row, int column)
{
    setParentElement(m_table);
    m_cell = m_table->m_textTable->cellAt(row, column);
    m_row = row;
    m_column = column;
    m_valid = true;
}

void CellData::validate()
{
    Q_ASSERT(m_table == parentElement());
    m_cell = m_table->m_textTable->cellAt(m_row, m_column);
    m_rowSpan = m_cell.rowSpan();
    m_columnSpan = m_cell.columnSpan();
    m_valid = true;
    foreach (ObjectData* object, childrenElements())
        object->validate();
}

void CellData::updateRowColumnCache()
{
    m_row = m_cell.row();
    m_column = m_cell.column();
}

bool CellData::isValid() const
{
    return m_valid;
}

int CellData::row() const
{
    return m_cell.row();
}

int CellData::column() const
{
    return m_cell.column();
}

QRect CellData::elementRect(ReportTextEdit *reportTextEdit)
{
    const QTextBlock firstBlock = m_cell.firstCursorPosition().block();
    const QTextBlock lastBlock = m_cell.lastCursorPosition().block();
    QRectF rect = reportTextEdit->document()->documentLayout()->blockBoundingRect( firstBlock ).
            unite( reportTextEdit->document()->documentLayout()->blockBoundingRect( lastBlock ) );
    const qreal padding = m_table->textTable()->format().cellPadding();
    rect.adjust( -padding, -padding, +padding, +padding );
    rect.setHeight(maxHeight(reportTextEdit));
    return rect.toRect();
}

qreal CellData::maxHeight(ReportTextEdit *reportTextEdit)
{
    qreal max = 0;
    for (int col = 0; col < m_table->columns(); col++) {
        QTextTableCell cell(m_table->cellAt(m_row, col)->m_cell);
        const QTextBlock firstBlock = cell.firstCursorPosition().block();
        const QTextBlock lastBlock = cell.lastCursorPosition().block();
        QRectF rect = reportTextEdit->document()->documentLayout()->blockBoundingRect( firstBlock ).
                unite( reportTextEdit->document()->documentLayout()->blockBoundingRect( lastBlock ) );
        const qreal padding = m_table->textTable()->format().cellPadding();
        rect.adjust( -padding, -padding, +padding, +padding );
        max = qMax(max, rect.height());
    }
    return max;
}

bool CellData::supportParagraph() const
{
    return true;
}

bool CellData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    if (startPosition() == cursorPosition)
        return editorData->insertSubElement(this, new TextElementData(text), 0);
    else if (endPosition() == cursorPosition) {
        ParagraphChild* pc = qobject_cast<ParagraphChild*>(parentElement());
        return editorData->insertSubElement(pc->parentElement(), new TextElementData(text), pc->childPosition() + 1);
    } else {
        qWarning() << "write text here not yet allowed" << this << cursorPosition;
        return false;
    }
}
