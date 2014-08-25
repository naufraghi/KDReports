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

#include "ReportObjectXmlElementHandler.h"
#include "KDReportsXmlParser_p.h"
#include "KDReportsXmlHelper.h"
#include "structure/AutoTableElementData.h"
#include "structure/CellData.h"
#include "structure/ChartElementData.h"
#include "structure/FooterData.h"
#include "structure/HLineElementData.h"
#include "structure/HeaderData.h"
#include "structure/HtmlElementData.h"
#include "structure/ImageElementData.h"
#include "structure/PageBreakData.h"
#include "structure/ReportData.h"
#include "structure/TableElementData.h"
#include "structure/TextElementData.h"
#include "structure/VariableData.h"
#include "structure/VSpaceData.h"
#include <QDomElement>

ReportObjectXmlElementHandler::ReportObjectXmlElementHandler()
{}

ReportObjectXmlElementHandler::~ReportObjectXmlElementHandler()
{
}

bool ReportObjectXmlElementHandler::startReport(KDReports::Report &report, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData = new ReportData(report);
    return true;
}

bool ReportObjectXmlElementHandler::startHeader(KDReports::Header &header, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->addObjectData(new HeaderData(header));
    return true;
}

void ReportObjectXmlElementHandler::endHeader(KDReports::Header &header, const QDomElement &xmlElement)
{
    Q_UNUSED(header);
    Q_UNUSED(xmlElement);
    m_reportData->endHeader();
}


bool ReportObjectXmlElementHandler::startFooter(KDReports::Footer &footer, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->addObjectData(new FooterData(footer));
    return true;
}

void ReportObjectXmlElementHandler::endFooter(KDReports::Footer &footer, const QDomElement &xmlElement)
{
    Q_UNUSED(footer);
    Q_UNUSED(xmlElement);
    m_reportData->endFooter();
}

bool ReportObjectXmlElementHandler::textElement(KDReports::TextElement &textElement, QDomElement &xmlElement)
{
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    QColor background;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
        background = KDReports::XmlHelper::readColor(xmlElement, "paragraph-background");
    }
    bool fontModified = xmlElement.hasAttribute("font") || xmlElement.hasAttribute("pointsize");
    TextElementData::ModelData modelData;
    modelData.modelKey = xmlElement.attribute("model", QString());
    modelData.modelRow = xmlElement.attribute("row", 0).toInt();
    modelData.hasModelRow = xmlElement.hasAttribute("row");
    modelData.modelColumn = xmlElement.attribute("column", 0).toInt();
    modelData.hasModelColumn = xmlElement.hasAttribute("column");
    m_reportData->addObjectElementData(new TextElementData(textElement, fontModified, modelData), isInline, alignment, background);
    return true;
}

bool ReportObjectXmlElementHandler::htmlElement(KDReports::HtmlElement &htmlElement, QDomElement &xmlElement)
{
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
    }
    m_reportData->addObjectElementData(new HtmlElementData(htmlElement), isInline, alignment);
    return true;
}

bool ReportObjectXmlElementHandler::startTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
    }
    m_reportData->addObjectElementData(new TableElementData(tableElement), isInline, alignment);
    return true;
}

bool ReportObjectXmlElementHandler::endTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->endTableElement(tableElement);
    return true;
}

bool ReportObjectXmlElementHandler::startCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    m_reportData->addObjectData(new CellData(m_reportData->currentTable(), xmlElement.attribute("row").toInt(), xmlElement.attribute("column").toInt(), cell.background()));
    return true;
}

bool ReportObjectXmlElementHandler::endCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    Q_UNUSED(cell);
    Q_UNUSED(xmlElement);
    m_reportData->endCell();
    return true;
}

bool ReportObjectXmlElementHandler::autoTableElement(KDReports::AutoTableElement &tableElement, QDomElement &xmlElement)
{
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
    }
    m_reportData->addObjectElementData(new AutoTableElementData(tableElement, xmlElement.attribute("model")), isInline, alignment);
    return true;
}

bool ReportObjectXmlElementHandler::chartElement(KDReports::ChartElement &chartElement, QDomElement &xmlElement)
{
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
    }
    m_reportData->addObjectElementData(new ChartElementData(chartElement, xmlElement.attribute("model")), isInline, alignment);
    return true;
}

bool ReportObjectXmlElementHandler::imageElement(KDReports::ImageElement &imageElement, QDomElement &xmlElement)
{
    bool isInline = xmlElement.hasAttribute(QLatin1String("inline"));
    Qt::AlignmentFlag alignment = Qt::AlignLeft;
    if(!isInline){
        if(xmlElement.hasAttribute("alignment"))
            alignment = KDReports::XmlHelper::stringToAlignment(xmlElement.attribute("alignment"));
    }
    if (imageElement.image().isNull())
        imageElement.setImage(QImage(QString::fromLatin1(":/image-missing")));
    m_reportData->addObjectElementData(new ImageElementData(imageElement, xmlElement.attribute(QLatin1String("file"))), isInline, alignment);
    return true;
}

bool ReportObjectXmlElementHandler::pageBreak(QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->addObjectData(new PageBreakData());
    return true;
}

ReportData* ReportObjectXmlElementHandler::reportData()
{
    return m_reportData;
}

bool ReportObjectXmlElementHandler::hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement)
{
    Q_UNUSED( xmlElement);
    m_reportData->addObjectData(new HLineElementData(hLineElement));
    return true;
}

bool ReportObjectXmlElementHandler::variable(KDReports::VariableType &type, QDomElement &xmlElement)
{
    m_reportData->addObjectElementData(new VariableData(type), xmlElement.hasAttribute(QLatin1String("inline")));
    return true;
}

bool ReportObjectXmlElementHandler::vspace(int &size, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->addObjectData(new VSpaceData(size));
    return true;
}

bool ReportObjectXmlElementHandler::tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->setTabs(tabs);
    return true;
}

bool ReportObjectXmlElementHandler::paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    m_reportData->setParagraphMargin(left, top, right, bottom);
    return true;
}

void ReportObjectXmlElementHandler::endReport(KDReports::Report &report, const QDomElement &xmlElement)
{
    Q_UNUSED(report)
    Q_UNUSED(xmlElement)
    m_reportData->validate();
}
