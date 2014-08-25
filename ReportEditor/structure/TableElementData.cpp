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

#include "TableElementData.h"
#include <KDReportsReportBuilder_p.h>
#include "ParagraphData.h"
#include "CellData.h"
#include <QTextBlock>
#include <QTextDocument>
#include <QTextTable>
#include <QDebug>
#include <KDReportsLayoutHelper_p.h>

TableElementData::TableElementData(const KDReports::TableElement &tableElement)
    : ParagraphChild()
    , m_tableElement(tableElement)
    , m_textTable(0)
{
    setObjectName(tr("table"));
}

void TableElementData::validate(){
    if (childrenElements().size() > 0) {
        KDReports::TextDocumentData& docData = textDocumentData();
        QTextCursor cursor(&docData.document());
        int start = startPosition();
        cursor.setPosition(start + 1);
        m_textTable = cursor.currentTable();
    }

    foreach(ObjectData* o, childrenElements()) {
        CellData* cell = qobject_cast<CellData*>(o);
        cell->validate();
    }

    // Create all cells
    for (int r=0; r < rows(); r++) {
        for (int c=0; c < columns(); c++) {
            cellAt(r, c);
        }
    }
}

QString TableElementData::translatePropertyName(const QString &name) const
{
    if (name == "background")
        return tr("Background");
    else if (name == "border")
        return tr("Border");
    else if (name == "width")
        return tr("Width");
    else if (name == "headerRowCount")
        return tr("Header row count");
    else if (name == "cellpadding")
        return tr("cellpadding");
    return ObjectData::translatePropertyName(name);
}

QVariant TableElementData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "border" || name == "width" || name == "cellpadding") && type == ObjectData::MIN)
        return 0.0;
    else if (name == "headerRowCount" && type == ObjectData::MIN)
        return 0;
    return ObjectData::propertyValue(name, type);
}

QBrush TableElementData::background() const
{
    return m_tableElement.background();
}

void TableElementData::setBackground(const QBrush &brush)
{
    m_tableElement.setBackground(brush);
}

double TableElementData::border() const
{
    return m_tableElement.border();
}

void TableElementData::setBorder(double border)
{
    m_tableElement.setBorder(border);
}

double TableElementData::width() const
{
    return m_tableElement.width();
}

void TableElementData::setWidth(double width)
{
    m_tableElement.setWidth(width);
}

int TableElementData::headerRowCount() const
{
    return m_tableElement.headerRowCount();
}

void TableElementData::setHeaderRowCount(int headerRowCount)
{
    m_tableElement.setHeaderRowCount(headerRowCount);
}

double TableElementData::cellpadding() const
{
    return m_tableElement.padding();
}

void TableElementData::setCellpadding(double cellpadding)
{
    m_tableElement.setPadding(cellpadding);
}

void TableElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("table"));
    writer.writeAttribute(QString::fromLatin1("background"), background().color().name());
    writer.writeAttribute(QString::fromLatin1("border"), QString::number(border()));
    writer.writeAttribute(QString::fromLatin1("width"), QString::number(width()));
    if (isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), QString::fromLatin1("true"));
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    writer.writeAttribute(QString::fromLatin1("headerRowCount"), QString::number(headerRowCount()));
    writer.writeAttribute(QString::fromLatin1("cellpadding"), QString::number(cellpadding()));
    foreach(ObjectData* od, childrenElements()){
        od->writeXml(writer);
    }
    writer.writeEndElement();
}

int TableElementData::endPosition() const
{
    if (!m_onDocument)
        return startPosition();
    if (m_textTable)
        return m_textTable->lastCursorPosition().position() + 1;
    return elementSize();
}

int TableElementData::elementSize() const
{
    return 0;
}

void TableElementData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    cursor.beginEditBlock();

    cursor.setBlockFormat(blockFormat());
    int nbRows = 1;
    int nbColumns = 1;
    foreach (ObjectData *object, childrenElements()) {
        CellData* cell = qobject_cast<CellData*>(object);
        nbRows = qMax(cell->m_row + 1, nbRows);
        nbColumns = qMax(cell->m_column + 1, nbColumns);
    }
    m_textTable = cursor.insertTable(nbRows, nbColumns, tableFormat());
    cursor.endEditBlock();
    foreach (ObjectData *object, childrenElements()) {
        CellData* cell = qobject_cast<CellData*>(object);
        cell->updateCell(cell->m_row, cell->m_column);
        cell->doAddToReport();
    }
    cellAt(0, 0);
    m_onDocument = true;
}

int TableElementData::startPosition() const
{
    if (m_textTable)
        return m_textTable->firstCursorPosition().position();
    QTextBlock lastBlock = textDocumentData().document().lastBlock();
    int start = ObjectData::startPosition();
    return qMin(start, lastBlock.position() + lastBlock.length()-1);
}

void TableElementData::doRemove()
{
    QTextCursor first = m_textTable->firstCursorPosition();
    first.beginEditBlock();
    QTextCursor last = m_textTable->lastCursorPosition();
    first.movePosition(QTextCursor::PreviousCharacter);
    first.setPosition(last.position()+1, QTextCursor::KeepAnchor);
    first.removeSelectedText();
    first.endEditBlock();
    m_textTable = 0;
    m_onDocument = false;
}

QTextTable * TableElementData::textTable()
{
    if (!m_textTable)
        doAddToReport();
    return m_textTable;
}

void TableElementData::insertRows(int pos, int number)
{
    m_textTable->insertRows(pos, number);
}

void TableElementData::removeRows(int pos, int number)
{
    m_textTable->removeRows(pos, number);
}

void TableElementData::insertColumn(int pos, int number)
{
    m_textTable->insertColumns(pos, number);
}

void TableElementData::removeColumns(int pos, int number)
{
    m_textTable->removeColumns(pos, number);
}

CellData* TableElementData::cellAt(int row, int column)
{
    if (row >= m_textTable->rows() || column >= m_textTable->columns())
        return 0;

    foreach (QObject * object, childrenElements())
        if (CellData* cellData = qobject_cast<CellData*>(object)) {
            if (cellData->m_cell == m_textTable->cellAt(row, column))
                return cellData;
        }
    return new CellData(this, row, column);
}

int TableElementData::columns() const
{
    return m_textTable->columns();
}

int TableElementData::rows() const
{
    return m_textTable->rows();
}

void TableElementData::doUpdate()
{
    QTextCursor cursor(m_textTable->firstCursorPosition());
    cursor.setBlockFormat(blockFormat());
    m_textTable->setFormat(tableFormat());
}

QTextTableFormat TableElementData::tableFormat() const
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.setPosition(startPosition());
    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount( headerRowCount() );
    tableFormat.setAlignment(cursor.blockFormat().alignment());
    tableFormat.setBackground(background());

    if ( width() ) {
        tableFormat.setWidth( QTextLength( QTextLength::FixedLength, KDReports::mmToPixels( width() ) ) );
    }

    tableFormat.setBorder( border() );
#if QT_VERSION >= 0x040300
    tableFormat.setBorderBrush( Qt::darkGray );
#endif
    tableFormat.setCellPadding(KDReports::mmToPixels(cellpadding()));
    tableFormat.setCellSpacing( 0 ); // HTML-like table borders look so old century
    return tableFormat;
}

QTextBlockFormat TableElementData::blockFormat() const
{
    QTextBlockFormat blockFormat;
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        QList<QTextOption::Tab> tabsInPixels;
        foreach (QTextOption::Tab tab, paragraph->tabs()) {
            tab.position = KDReports::mmToPixels( tab.position );
            tabsInPixels.append( tab );
        }
        blockFormat.setTabPositions(tabsInPixels);
        blockFormat.setLeftMargin(paragraph->marginLeft());
        blockFormat.setRightMargin(paragraph->marginRight());
        blockFormat.setTopMargin(paragraph->marginTop());
        blockFormat.setBottomMargin(paragraph->marginBottom());
        blockFormat.setAlignment(Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())));
        if (paragraph->background().color().isValid())
                blockFormat.setBackground(paragraph->background().color());
    }
    return blockFormat;
}
