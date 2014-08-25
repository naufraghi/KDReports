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

#ifndef VARIABLEDATA_H
#define VARIABLEDATA_H

#include "KDReportsVariableType.h"
#include "ObjectData.h"

class VariableData : public ObjectData
{
    Q_OBJECT
    Q_PROPERTY(VariableData::Type type READ type WRITE setType )
public:
    enum Type {
        PageNumber = KDReports::PageNumber,  ///< Page number
        PageCount = KDReports::PageCount,   ///< Page count
        TextDate = KDReports::TextDate,    ///< Current date in text format like "Thu Aug 18 2005", with translated
                     ///  names but unlocalized order of names and numbers
        ISODate = KDReports::ISODate,     ///< Current date in ISO 8601 format like "2005-08-18"
        LocaleDate = KDReports::LocaleDate,  ///< Current date in locale-dependent format, deprecated in favour of SystemLocaleShortDate or SystemLocaleLongDate.
        TextTime = KDReports::TextTime,    ///< Current time in text format like "13:42:59"
        ISOTime = KDReports::ISOTime,     ///< Current time in ISO 8601 format like "13:42:59"
        LocaleTime = KDReports::LocaleTime,  ///< Current time in locale-dependent format

        SystemLocaleShortDate = KDReports::SystemLocaleShortDate, ///< Current date formatted according to the system locale, short format, see Qt::SystemLocaleShortDate
        SystemLocaleLongDate = KDReports::SystemLocaleLongDate, ///< Current date formatted according to the system locale, long format, see Qt::SystemLocaleLongDate
        DefaultLocaleShortDate = KDReports::DefaultLocaleShortDate, ///< Current date formatted according to the application locale, short format, see Qt::DefaultLocaleShortDate
        DefaultLocaleLongDate = KDReports::DefaultLocaleLongDate ///< Current date formatted according to the application locale, long format, see Qt::DefaultLocaleLongDate
    };
    Q_ENUMS(Type)
    explicit VariableData(KDReports::VariableType type);
    void setType(VariableData::Type type);
    VariableData::Type type() const;
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    virtual void doUpdate();
    virtual void doAddToReport();
    virtual void doRemove();
    QString translatePropertyName(const QString &name) const;
    QString translatedEnumValue(const QMetaEnum &metaEnum, int key) const;

private:
    VariableData::Type m_type;
    int m_cursorSize;
};

#endif // VARIABLEDATA_H
