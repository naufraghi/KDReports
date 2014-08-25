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

#ifndef HEADERDATA_H
#define HEADERDATA_H

#include "ObjectData.h"
#include "KDReportsHeader.h"
#include "KDReportsReport.h"
#include <QFont>
class HeaderData : public ObjectData
{
    Q_OBJECT
    Q_FLAGS(Locations Location)
    Q_PROPERTY (QFont font READ font WRITE setFont)
    Q_PROPERTY (Location location READ location WRITE setLocation NOTIFY locationChanged)

public:
    enum Location {
        FirstPage = KDReports::FirstPage, ///< The first page of the report
        EvenPages = KDReports::EvenPages, ///< The even pages of the report: 2, 4, 6 etc.
        OddPages = KDReports::OddPages,  ///< The odd pages of the report: 1 (unless FirstPage has its own header), 3, 5, 7 etc.
        LastPage = KDReports::LastPage  ///< The last page of the report.
    };
    Q_DECLARE_FLAGS(Locations, Location)
    HeaderData(KDReports::Header &header);
    virtual void setLocation(HeaderData::Locations location);
    virtual HeaderData::Locations location() const;
    QFont font() const;
    void setFont(const QFont& font);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int startPosition() const;
    virtual int endPosition() const;
    virtual KDReports::TextDocumentData& textDocumentData() const;
    virtual ObjectData* topContainer() const;
    void writeHelper(QXmlStreamWriter &writer, const QString &type);
    void setAssociatedReportData(ReportData *reportData);
    QString translatePropertyName(const QString &name) const;
    QString translatedEnumValue(const QMetaEnum &metaEnum, int key) const;
    bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    bool supportParagraph() const;
    void doUpdate();

protected:
    friend class ReportData; // Access to m_header.currentPosition()
    KDReports::Header& m_header;
    ReportData *m_reportData;

signals:
    void locationChanged();
};

#endif // HEADERDATA_H
