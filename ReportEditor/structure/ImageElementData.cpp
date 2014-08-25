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

#include "ImageElementData.h"
#include "ParagraphData.h"
#include <QTextDocument>
#include <QDebug>
#include <KDReportsReportBuilder_p.h>

ImageElementData::ImageElementData(const KDReports::ImageElement &imageElement, const QString &filename)
    : ParagraphChild()
    , m_imageElement(imageElement)
    , m_filename(filename)
    , m_lastWidthPercent(0.0)
    , m_lastHeightPercent(0.0)
    , m_lastWidthMillimeters(0.0)
    , m_lastHeightMillimeters(0.0)
{
    setObjectName(tr("image"));
}

QString ImageElementData::id() const
{
    return m_imageElement.id();
}

void ImageElementData::setId(const QString &id)
{
    QString oldId = m_imageElement.id();
    m_imageElement.setId(id);
    emit idChanged(id, oldId);
}

QString ImageElementData::filename() const
{
    return m_filename;
}

void ImageElementData::setFilename(const QString &filename)
{
    m_filename = filename;
    QImage image(m_filename);
    if (image.isNull())
        image = QImage(QString::fromLatin1(":/image-missing"));
    m_imageElement.setImage(image);
}

double ImageElementData::width() const
{
    // In order to have a size if no specific size was specified
    if (!fitToPage() && !height() && !m_imageElement.width())
        return m_imageElement.image().width();
    return m_imageElement.width();
}

void ImageElementData::setWidth(double width)
{
    m_imageElement.setWidth(width, m_imageElement.unit());
}

double ImageElementData::height() const
{
    return m_imageElement.height();
}

void ImageElementData::setHeight(double height)
{
    m_imageElement.setHeight(height, m_imageElement.unit());
}

bool ImageElementData::fitToPage() const
{
    return m_imageElement.fitToPage();
}

void ImageElementData::setFitToPage(bool fitToPage)
{
    if(fitToPage)
        m_imageElement.setFitToPage();
    else
        m_imageElement.setHeight(0); //Reset all size properties
}

ImageElementData::Unit ImageElementData::unit() const
{
    return ImageElementData::Unit(static_cast<int>(m_imageElement.unit()));
}

void ImageElementData::setUnit(Unit unit)
{
    if (unit == ImageElementData::Millimeters) {
        m_lastWidthPercent = m_imageElement.width();
        m_lastHeightPercent = m_imageElement.height();
    } else {
        m_lastWidthMillimeters = m_imageElement.width();
        m_lastHeightMillimeters = m_imageElement.height();
    }
    m_imageElement.setUnit(KDReports::Unit(static_cast<int>(unit)));
    if (unit == ImageElementData::Millimeters) {
        if (m_lastWidthMillimeters)
            setWidth(m_lastWidthMillimeters);
        else
            setHeight(m_lastHeightMillimeters);
    } else {
        if (m_lastHeightMillimeters)
            setWidth(m_lastWidthPercent);
        else
            setHeight(m_lastHeightPercent);
    }
}

void ImageElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("image"));
    writer.writeAttribute(QString::fromLatin1("file"), filename());
    writer.writeAttribute(QString::fromLatin1("id"), id());
    if(fitToPage())
        writer.writeAttribute(QString::fromLatin1("fitToPage"), QString::fromLatin1("true"));
    else if(width())
        writer.writeAttribute(QString::fromLatin1("width"), QString::number(width()));
    else if(height())
        writer.writeAttribute(QString::fromLatin1("height"), QString::number(height()));
    if (isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), QString::fromLatin1("true"));
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    writer.writeEndElement();
}

int ImageElementData::elementSize() const
{
    return 1;
}

QString ImageElementData::translatePropertyName(const QString &name) const
{
    if (name == "id")
        return tr("id");
    else if (name == "filename")
        return tr("File name");
    else if (name == "width")
        return tr("Width");
    else if (name == "height")
        return tr("Height");
    else if (name == "unit")
        return tr("Unit");
    else if (name == "fitToPage")
        return tr("Fit to page");
    return ObjectData::translatePropertyName(name);
}

QVariant ImageElementData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "width" || name == "height") && type == ObjectData::MIN)
        return 0.0;
    if ((name == "width" || name == "height")
            && unit() == ImageElementData::Percent && type == ObjectData::MAX)
        return 100.0;
    return ObjectData::propertyValue(name, type);
}

QString ImageElementData::translatedEnumValue(const QMetaEnum &metaEnum, int key) const
{
    if (QString::fromLatin1(metaEnum.name()) == "Unit")
    {
        switch (metaEnum.value(key)) {
        case ImageElementData::Millimeters:
            return tr("Millimeters");
        case ImageElementData::Percent:
            return tr("Percent");
        }
    }
    return ObjectData::translatedEnumValue(metaEnum, key);
}

void ImageElementData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    cursor.beginEditBlock();
    KDReports::ReportBuilder builder( docData,
                                      cursor, 0 /* hack - assumes Report is not needed */ );
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        paragraph->initBuilder(builder);
        builder.addBlockElement(m_imageElement, Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())), paragraph->background().color());
    }
    cursor.endEditBlock();
    m_onDocument = true;
}
