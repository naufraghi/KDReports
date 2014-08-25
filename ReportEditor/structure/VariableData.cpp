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

#include "VariableData.h"
#include "HeaderData.h"
#include "ParagraphData.h"
#include <KDReportsReportBuilder_p.h>
#include <KDReportsHeader.h>
#include <QMetaEnum>
#include <QDebug>

static const int VariableLengthProperty = QTextFormat::UserProperty + 247;

VariableData::VariableData(KDReports::VariableType type)
    : ObjectData(0)
    , m_type(VariableData::Type(static_cast<int>(type)))
{
    setObjectName(tr("variable"));
}

void VariableData::setType(VariableData::Type type)
{
    m_type = type;
}

VariableData::Type VariableData::type() const
{
    return m_type;
}

void VariableData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("variable"));
    int idx = metaObject()->indexOfEnumerator("Type");
    QMetaEnum meta = metaObject()->enumerator(idx);
    writer.writeAttribute(QString::fromLatin1("type"), QString::fromLatin1(meta.valueToKey(m_type)).toLower());
    writer.writeEndElement();
}

int VariableData::endPosition() const
{
    return startPosition() + m_cursorSize;
}

void VariableData::doUpdate()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    QTextCharFormat format = cursor.charFormat();
    const int oldLength = format.property(VariableLengthProperty).toInt();
    cursor.setPosition( start+oldLength, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    KDReports::ReportBuilder builder( docData,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    //Needs to take in consideration paragraph option data
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        paragraph->initBuilder(builder);
        builder.addVariable(KDReports::VariableType(static_cast<int>(m_type)));
    }
}

void VariableData::doRemove()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.beginEditBlock();
    int start = startPosition();
    cursor.setPosition(start);
    QTextCharFormat format = cursor.charFormat();
    const int oldLength = format.property(VariableLengthProperty).toInt();
    cursor.setPosition( start+oldLength, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    cursor.endEditBlock();
}

QString VariableData::translatePropertyName(const QString &name) const
{
    if (name == "type")
        return tr("Type");
    return ObjectData::translatePropertyName(name);
}

QString VariableData::translatedEnumValue(const QMetaEnum &metaEnum, int key) const
{
    if (QString::fromLatin1(metaEnum.name()) == "Type")
    {
        switch (metaEnum.value(key)) {
        case VariableData::PageNumber:
            return tr("Page number");
        case VariableData::PageCount:
            return tr("Page count");
        case VariableData::TextDate:
            return tr("Text date");
        case VariableData::ISODate:
            return tr("ISO date");
        case VariableData::LocaleDate:
            return tr("Locale date");
        case VariableData::TextTime:
            return tr("Text time");
        case VariableData::ISOTime:
            return tr("ISO time");
        case VariableData::LocaleTime:
            return tr("Locale time");
        case VariableData::SystemLocaleShortDate:
            return tr("System locale short date");
        case VariableData::SystemLocaleLongDate:
            return tr("System locale long date");
        case VariableData::DefaultLocaleShortDate:
            return tr("Default locale short date");
        case VariableData::DefaultLocaleLongDate:
            return tr("Default locale long date");
        }
    }
    return ObjectData::translatedEnumValue(metaEnum, key);
}

void VariableData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    KDReports::ReportBuilder builder( docData,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    //Needs to take in consideration paragraph option data
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        paragraph->initBuilder(builder);
        builder.addVariable(KDReports::VariableType(static_cast<int>(m_type)));
    }
}
