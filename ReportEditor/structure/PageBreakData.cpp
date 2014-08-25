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

#include "PageBreakData.h"
#include <QCursor>
#include <QTextBlock>
#include <QTextDocument>
#include <KDReportsReportBuilder_p.h>
#include "PageBreakTextObject.h"

PageBreakData::PageBreakData()
    : TopElement()
{
    setObjectName(tr("page-break"));
}

void PageBreakData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("page-break"));
    writer.writeEndElement();
}

int PageBreakData::endPosition() const
{
    if (!m_onDocument)
        return startPosition();
    QTextBlock lastBlock = textDocumentData().document().lastBlock();
    int lineReturn = 0;
    if (m_lineReturn)
        lineReturn = 1;
    return qMin(startPosition() + 1 + lineReturn, lastBlock.position() + lastBlock.length()-1);
}

void PageBreakData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    QTextBlockFormat f = cursor.blockFormat();
    cursor.beginEditBlock();
    insertLineBefore(cursor);
    QTextCharFormat fmt;

    fmt.setObjectType( PageBreakTextObject::PageBreakTextFormat );
    cursor.insertText( QString(QChar::ObjectReplacementCharacter), fmt);
    insertLineAfter(cursor, f);
    cursor.endEditBlock();
    m_onDocument = true;
}

void PageBreakData::validate()
{
    doAddToReport();
}
