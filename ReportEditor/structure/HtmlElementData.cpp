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

#include "HtmlElementData.h"
#include "ParagraphData.h"
#include <KDReportsReportBuilder_p.h>

HtmlElementData::HtmlElementData(const KDReports::HtmlElement &htmlElement)
    : ParagraphChild()
    , m_htmlElement(htmlElement)
{
    setObjectName(tr("html"));
}

QString HtmlElementData::id() const
{
    return m_htmlElement.id();
}

void HtmlElementData::setId(const QString &id)
{
    QString oldId = m_htmlElement.id();
    m_htmlElement.setId(id);
    emit idChanged(id, oldId);
}

QString HtmlElementData::html() const
{
    return m_htmlElement.html();
}

void HtmlElementData::setHtml(const QString &html)
{
   m_htmlElement.setHtml(html);
}

QBrush HtmlElementData::background() const
{
    return m_htmlElement.background();
}

void HtmlElementData::setBackground(const QBrush &brush)
{
    m_htmlElement.setBackground(brush);
}

void HtmlElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("html"));
    if(!id().isEmpty())
        writer.writeAttribute(QString::fromLatin1("id"), id());
    if (background().style()!=Qt::NoBrush && background().color().isValid())
        writer.writeAttribute(QString::fromLatin1("background"), background().color().name());
    if (isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), QString::fromLatin1("true"));
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    writer.writeCharacters(html());
    writer.writeEndElement();
}

void HtmlElementData::doUpdate()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.beginEditBlock();
    int start = startPosition();
    cursor.setPosition(start);
    cursor.setPosition( endPosition(), QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    KDReports::ReportBuilder builder( docData,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    //Needs to take in consideration paragraph option data
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        paragraph->initBuilder(builder);
        builder.addBlockElement(m_htmlElement, Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())), paragraph->background().color());
    }
    m_cursorSize = cursor.position() - start;
    cursor.endEditBlock();
}

int HtmlElementData::elementSize() const
{
    return m_htmlElement.html().size();
}

QString HtmlElementData::translatePropertyName(const QString &name) const
{
    if (name == "html")
        return tr("html");
    else if (name == "id")
        return tr("id");
    else if (name == "background")
        return tr("Background");
    return ObjectData::translatePropertyName(name);
}
