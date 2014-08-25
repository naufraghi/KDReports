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

#include "HeaderData.h"
#include "ReportData.h"
#include <QMetaEnum>
#include <QString>
#include <KDReportsTextDocument_p.h>
#include "EditorData.h"
#include "structure/TextElementData.h"
#include <QDebug>

HeaderData::HeaderData(KDReports::Header& header)
    : ObjectData(0)
    , m_header(header)
    , m_reportData(0)
{
    setObjectName(tr("header"));
}

void HeaderData::setFont(const QFont& font)
{
    m_header.setDefaultFont(font);
}

QFont HeaderData::font() const
{
    return m_header.defaultFont();
}

void HeaderData::setLocation(HeaderData::Locations location)
{
    if(m_reportData){
        m_reportData->m_report.setHeaderLocation(KDReports::HeaderLocations(static_cast<int>(location)), &m_header);
    }
    emit locationChanged();
}

HeaderData::Locations HeaderData::location() const
{
    if(m_reportData){
        return Locations(static_cast<int>(m_reportData->m_report.headerLocation(&m_header)));
    }
    return 0;
}

void HeaderData::writeXml(QXmlStreamWriter &writer)
{
    writeHelper(writer, QString::fromLatin1("header"));
}

void HeaderData::writeHelper(QXmlStreamWriter &writer, const QString &type)
{
    writer.writeStartElement(type);
    int idx = metaObject()->indexOfEnumerator("Locations");
    QMetaEnum meta = metaObject()->enumerator(idx);
    QString loc(QString::fromLatin1(meta.valueToKeys(location())).toLower());
    loc.replace('|',',');
    loc.remove(QString::fromLatin1("pages"));
    loc.remove(QString::fromLatin1("page"));
    writer.writeAttribute(QString::fromLatin1("location"), loc);
    writer.writeAttribute(QString::fromLatin1("font"), font().family());
    writer.writeAttribute(QString::fromLatin1("pointsize"), QString::number(font().pointSizeF()));
    foreach(ObjectData* od, childrenElements()){
        od->writeXml(writer);
    }
    writer.writeEndElement();
}

void HeaderData::setAssociatedReportData(ReportData* reportData)
{
    m_reportData = reportData;
}

QString HeaderData::translatePropertyName(const QString &name) const
{
    if (name == "font")
        return tr("Font");
    else if (name == "location")
        return tr("Location");
    return ObjectData::translatePropertyName(name);
}

QString HeaderData::translatedEnumValue(const QMetaEnum &metaEnum, int key) const
{
    if (QString::fromLatin1(metaEnum.name()) == "Location")
    {
        switch (metaEnum.value(key)) {
        case HeaderData::EvenPages:
            return tr("Even pages");
        case HeaderData::OddPages:
            return tr("Odd pages");
        case HeaderData::FirstPage:
            return tr("First page");
        case HeaderData::LastPage:
            return tr("Last page");
        }
    }
    return ObjectData::translatedEnumValue(metaEnum, key);
}

int HeaderData::endPosition() const
{
    if (!childrenElements().isEmpty()) {
        return childrenElements().last()->endPosition();
    }
    return startPosition();
}

int HeaderData::startPosition() const
{
    return 0;
}

KDReports::TextDocumentData & HeaderData::textDocumentData() const
{
    return m_header.doc().contentDocumentData();
}

ObjectData* HeaderData::topContainer() const
{
    return const_cast<HeaderData*>(this);
}

bool HeaderData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    if (startPosition() == cursorPosition)
        return editorData->insertSubElement(this, new TextElementData(text), 0);
    else {
        qWarning() << "write text here not yet allowed" << this << cursorPosition;
        return false;
    }
}

bool HeaderData::supportParagraph() const
{
    return true;
}

void HeaderData::doUpdate()
{
    foreach (ObjectData* od, childrenElements()) {
        od->doUpdate();
    }
}
