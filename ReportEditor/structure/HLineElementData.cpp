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

#include "HLineElementData.h"
#include <KDReportsReportBuilder_p.h>
#include "PageBreakData.h"
#include "ParagraphData.h"
#include "../EditorData.h"
#include "TextElementData.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>
#include "ReportTextEdit.h"

HLineElementData::HLineElementData(const KDReports::HLineElement &hLineElement)
    : TopElement()
    , m_hLineElement(hLineElement)
{
    setObjectName(tr("horizontal line"));
}

QColor HLineElementData::color() const
{
    return m_hLineElement.color();
}

void HLineElementData::setColor(const QColor &color)
{
    m_hLineElement.setColor(color);
}

int HLineElementData::thickness() const
{
    return m_hLineElement.thickness();
}

void HLineElementData::setThickness(int thickness)
{
    m_hLineElement.setThickness(thickness);
}

int HLineElementData::margin()
{
    return m_hLineElement.margin();
}

void HLineElementData::setMargin(int margin)
{
    m_hLineElement.setMargin(margin);
}

void HLineElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("hline"));
    writer.writeAttribute(QString::fromLatin1("color"), color().name());
    writer.writeAttribute(QString::fromLatin1("thickness"), QString::number(thickness()));
    writer.writeAttribute(QString::fromLatin1("margin"), QString::number(margin()));
    writer.writeEndElement();
}

int HLineElementData::endPosition() const
{
    if (!m_onDocument)
        return startPosition();
    QTextBlock lastBlock = textDocumentData().document().lastBlock();
    int lineReturn = 0;
    if (m_lineReturn)
        lineReturn = 1;
    return qMin(startPosition() + 1 + lineReturn, lastBlock.position() + lastBlock.length()-1);
}

void HLineElementData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    QTextBlockFormat f = cursor.blockFormat();
    cursor.beginEditBlock();
    insertLineBefore(cursor);
    KDReports::ReportBuilder builder( docData,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    builder.addBlockElement(m_hLineElement, Qt::AlignLeft);
    insertLineAfter(cursor, f);
    cursor.endEditBlock();
    m_onDocument = true;
}

QRect HLineElementData::elementRect(ReportTextEdit *reportTextEdit)
{
    int endPos = endPosition();
    if (childPosition() != parentElement()->childrenElements().size() - 1)
        endPos--;
    return calculateRect(reportTextEdit, startPosition(), endPos);
}

bool HLineElementData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    int position = childPosition();
    if (cursorPosition != startPosition())
        position++;
    return editorData->insertSubElement(parentElement(), new TextElementData(text), position);
}

void HLineElementData::doUpdate()
{
    doRemove();
    doAddToReport();
}

void HLineElementData::doRemove()
{
    TopElement::doRemove();
}

QString HLineElementData::translatePropertyName(const QString &name) const
{
    if (name == "color")
        return tr("Color");
    else if (name == "thickness")
        return tr("Thickness");
    else if (name == "margin")
        return tr("Margin");
    return ObjectData::translatePropertyName(name);
}

QVariant HLineElementData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "thickness" || name == "margin") && type == ObjectData::MIN)
        return 0;
    return ObjectData::propertyValue(name, type);
}
