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

#include "FooterData.h"
#include "ReportData.h"

FooterData::FooterData(KDReports::Footer &footer)
    : HeaderData(footer)
{
    setObjectName(tr("footer"));
}

void FooterData::setLocation(HeaderData::Locations location)
{
    if(m_reportData){
        m_reportData->m_report.setFooterLocation(KDReports::HeaderLocations(static_cast<int>(location)), &m_header);
    }
}

HeaderData::Locations FooterData::location() const
{
    if(m_reportData){
        return Locations(static_cast<int>(m_reportData->m_report.footerLocation(&m_header)));
    }
    return 0;
}

void FooterData::writeXml(QXmlStreamWriter &writer)
{
    writeHelper(writer, QString::fromLatin1("footer"));
}
