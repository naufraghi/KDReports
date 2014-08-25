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

#include "VSpaceData.h"
#include <KDReportsReportBuilder_p.h>
#include <QTextBlock>
#include <QDebug>
#include <QTextDocument>
#include <KDReportsLayoutHelper_p.h>
#include "../EditorData.h"
#include "TextElementData.h"

VSpaceData::VSpaceData(int size)
    : TopElement()
    , m_size(size)
{
    setObjectName(tr("vertical space"));
}

void VSpaceData::setSize(int size)
{
    m_size = size;
}

int VSpaceData::size() const
{
    return m_size;
}

void VSpaceData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("vspace"));
    writer.writeAttribute(QString::fromLatin1("size"), QString::number(m_size));
    writer.writeEndElement();
}

int VSpaceData::endPosition() const
{
    if (!m_onDocument || !m_lineReturn)
        return startPosition();
    else
        return startPosition() + 1;
}

void VSpaceData::doUpdate()
{
    ObjectData::doUpdate();
}

void VSpaceData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    cursor.beginEditBlock();
    insertLineBefore(cursor);
    QTextBlockFormat f = cursor.blockFormat();

    // Add vspace
    KDReports::ReportBuilder builder( docData,
                                cursor, 0 /* hack - assumes Report is not needed */ );
    builder.addVerticalSpacing(size());

    insertLineAfter(cursor, f);
    cursor.endEditBlock();
    m_onDocument = true;
}

void VSpaceData::doRemove()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.beginEditBlock();
    cursor.setPosition(startPosition());
    cursor.setBlockFormat(QTextBlockFormat());
    cursor.endEditBlock();
    TopElement::doRemove();
}

bool VSpaceData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    Q_UNUSED(cursorPosition);
    int position = childPosition() + 1;
    return editorData->insertSubElement(parentElement(), new TextElementData(text), position);
}

QString VSpaceData::translatePropertyName(const QString &name) const
{
    if (name == "size")
        return tr("Size");
    return ObjectData::translatePropertyName(name);
}

QVariant VSpaceData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if (name == "size" && type == ObjectData::MIN)
        return 0;
    return ObjectData::propertyValue(name, type);
}
