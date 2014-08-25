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

#ifndef REPORTOBJECTXMLELEMENTHANDLER_H
#define REPORTOBJECTXMLELEMENTHANDLER_H

#include <KDReportsXmlElementHandlerV2.h>
#include "editor_export.h"

class QDomElement;
class ReportData;
class ObjectData;
using namespace KDReports;

class EDITOR_EXPORT ReportObjectXmlElementHandler : public XmlElementHandlerV2
{
public:
    ReportObjectXmlElementHandler();
    ~ReportObjectXmlElementHandler();
    ReportData* reportData();

    virtual bool startReport( KDReports::Report& report, QDomElement& xmlElement);
    virtual bool startHeader(KDReports::Header &header, QDomElement &xmlElement);
    virtual void endHeader(KDReports::Header &header, const QDomElement &xmlElement);
    virtual bool startFooter(KDReports::Footer &footer, QDomElement &xmlElement);
    virtual void endFooter(KDReports::Footer &footer, const QDomElement &xmlElement);
    virtual bool textElement(KDReports::TextElement &textElement, QDomElement &xmlElement);
    virtual bool htmlElement(KDReports::HtmlElement &htmlElement, QDomElement &xmlElement);
    virtual bool startTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement);
    virtual bool endTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement);
    virtual bool startCell(KDReports::Cell &cell, QDomElement &xmlElement);
    virtual bool endCell(KDReports::Cell &cell, QDomElement &xmlElement);
    virtual bool autoTableElement(KDReports::AutoTableElement &tableElement, QDomElement &xmlElement);
    virtual bool chartElement(KDReports::ChartElement &chartElement, QDomElement &xmlElement);
    virtual bool imageElement(KDReports::ImageElement &imageElement, QDomElement &xmlElement);
    virtual bool pageBreak(QDomElement &xmlElement);
    virtual bool hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement);
    virtual bool variable(KDReports::VariableType &type, QDomElement &xmlElement);
    virtual bool vspace(int &size, QDomElement &xmlElement);
    virtual bool tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement);
    virtual bool paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement &xmlElement);
    virtual void endReport( KDReports::Report& report, const QDomElement& xmlElement );
private:
    ReportData* m_reportData;
};


#endif // REPORTOBJECTXMLELEMENTHANDLER_H
