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

#include "ChartElementData.h"

ChartElementData::ChartElementData(const KDReports::ChartElement &chartElement, const QString& modelKey)
    : ParagraphChild()
    , m_chartElement(chartElement)
    , m_modelKey(modelKey)
{
    setObjectName(tr("chart"));
}

QString ChartElementData::modelKey() const
{
    return m_modelKey;
}

void ChartElementData::setModelKey(const QString &modelKey)
{
    m_modelKey = modelKey;
    m_chartElement.setModelKey(modelKey);
}

QBrush ChartElementData::background() const
{
    return m_chartElement.background();
}

void ChartElementData::setBackground(const QBrush &brush)
{
    m_chartElement.setBackground(brush);
}

QSizeF ChartElementData::size() const
{
    return m_chartElement.size();
}

void ChartElementData::setSize(QSizeF size)
{
    m_chartElement.setSize(size.width(), size.height());
}

void ChartElementData::writeXml(QXmlStreamWriter &writer)
{
    writer.writeStartElement(QString::fromLatin1("chart"));
    writer.writeAttribute(QString::fromLatin1("model"), modelKey());
    writer.writeAttribute(QString::fromLatin1("background"), background().color().name());
    writer.writeAttribute(QString::fromLatin1("width"), QString::number(size().width()));
    writer.writeAttribute(QString::fromLatin1("height"), QString::number(size().height()));
    if (isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), QString::fromLatin1("true"));
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    writer.writeEndElement();
}

int ChartElementData::elementSize() const
{
    return 0;
}

QString ChartElementData::translatePropertyName(const QString &name) const
{
    if (name == "modelKey")
        return tr("Model key");
    else if (name == "background")
        return tr("Background");
    else if (name == "size")
        return tr("Size");
    return ObjectData::translatePropertyName(name);
}
