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

#include "AutoTableElementData.h"

#include "ParagraphData.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextTable>

AutoTableElementData::AutoTableElementData(const KDReports::AutoTableElement &autoTableElement, const QString& modelKey)
    : ParagraphChild()
    , m_autoTableElement(autoTableElement)
    , m_modelKey(modelKey)
    , m_textTable(0)
{
    setObjectName(tr("auto table"));
    m_onDocument = true;
}

QString AutoTableElementData::modelKey() const
{
    return m_modelKey;
}

void AutoTableElementData::setModelKey(const QString &modelKey)
{
    const QString oldKey = m_modelKey;
    m_modelKey = modelKey;
    m_autoTableElement.setModelKey(modelKey);
    emit modelKeyChanged(m_modelKey, oldKey);
}

QBrush AutoTableElementData::background() const
{
    return m_autoTableElement.background();
}

void AutoTableElementData::setBackground(const QBrush &brush)
{
    m_autoTableElement.setBackground(brush);
}


double AutoTableElementData::border() const
{
    return m_autoTableElement.border();
}

void AutoTableElementData::setBorder(double border)
{
    m_autoTableElement.setBorder(border);
}

double AutoTableElementData::width() const
{
    return m_autoTableElement.width();
}

void AutoTableElementData::setWidth(double width)
{
    m_autoTableElement.setWidth(width);
}

QBrush AutoTableElementData::headerBackground() const
{
    return m_autoTableElement.headerBackground();
}

void AutoTableElementData::setHeaderBackground(const QBrush &headerBackground)
{
    m_autoTableElement.setHeaderBackground(headerBackground);
}

bool AutoTableElementData::verticalHeaderVisible() const
{
    return m_autoTableElement.isVerticalHeaderVisible();
}

void AutoTableElementData::setVerticalHeaderVisible(bool verticalHeaderVisible)
{
    m_autoTableElement.setVerticalHeaderVisible(verticalHeaderVisible);
}

bool AutoTableElementData::horizontalHeaderVisible() const
{
    return m_autoTableElement.isHorizontalHeaderVisible();
}

void AutoTableElementData::setHorizontalHeaderVisible(bool horizontalHeaderVisible)
{
    m_autoTableElement.setHorizontalHeaderVisible(horizontalHeaderVisible);
}

void AutoTableElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("table"));
    writer.writeAttribute(QString::fromLatin1("model"), modelKey());
    writer.writeAttribute(QString::fromLatin1("background"), background().color().name());
    writer.writeAttribute(QString::fromLatin1("border"), QString::number(border()));
    writer.writeAttribute(QString::fromLatin1("width"), QString::number(width()));
    if (isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), QString::fromLatin1("true"));
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    writer.writeAttribute(QString::fromLatin1("header-background"), headerBackground().color().name());
    writer.writeAttribute(QString::fromLatin1("verticalHeaderVisible"), QString::number(verticalHeaderVisible()));
    writer.writeAttribute(QString::fromLatin1("horizontalHeaderVisible"), QString::number(horizontalHeaderVisible()));
    writer.writeEndElement();
}

void AutoTableElementData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    cursor.beginEditBlock();
    QTextTableFormat tableFormat;
    ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement());
    tableFormat.setAlignment(Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())));
    tableFormat.setBackground(background());
    tableFormat.setBorder(border());
    m_textTable = cursor.insertTable(1, 1, tableFormat);
    QTextTableCell cell = m_textTable->cellAt( 0, 0 );
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText( "auto-table" );
    cursor.endEditBlock();
    m_onDocument = true;
}

int AutoTableElementData::elementSize() const
{
    return 12;
}

void AutoTableElementData::validate()
{
    doAddToReport();
}

QString AutoTableElementData::translatePropertyName(const QString &name) const
{
    if (name == "modelKey")
        return tr("Model key");
    else if (name == "background")
        return tr("Background");
    else if (name == "border")
        return tr("Border");
    else if (name == "width")
        return tr("Width");
    else if (name == "headerBackground")
        return tr("Header background");
    else if (name == "verticalHeaderVisible")
        return tr("Vertical header visible");
    else if (name == "horizontalHeaderVisible")
        return tr("Horizontal header visible");
    return ObjectData::translatePropertyName(name);
}

QVariant AutoTableElementData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "border" || name == "width") && type == ObjectData::MIN)
        return 0.0;
    return ObjectData::propertyValue(name, type);
}
