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

#ifndef IMAGEELEMENTDATA_H
#define IMAGEELEMENTDATA_H

#include "KDReportsImageElement.h"
#include "ParagraphChild.h"
#include <QBrush>

class EDITOR_EXPORT ImageElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QString id READ id WRITE setId)
    Q_PROPERTY (QString filename READ filename WRITE setFilename)
    Q_PROPERTY (double width READ width WRITE setWidth)
    Q_PROPERTY (double height READ height WRITE setHeight)
    Q_PROPERTY (Unit unit READ unit WRITE setUnit)
    Q_PROPERTY (bool fitToPage READ fitToPage WRITE setFitToPage)
public:
    enum Unit {
        Millimeters = KDReports::Millimeters,
        Percent = KDReports::Percent
    };
    Q_ENUMS(Unit)
    explicit ImageElementData(const KDReports::ImageElement& imageElement, const QString &filename);
    QString id() const;
    void setId(const QString &id);
    QString filename() const;
    void setFilename(const QString &filename);
    double width() const;
    void setWidth(double width);
    double height() const;
    void setHeight(double height);
    bool fitToPage() const;
    void setFitToPage(bool fitToPage);
    Unit unit() const;
    void setUnit(Unit unit);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual void doAddToReport();
    virtual int elementSize() const;
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;
    QString translatedEnumValue(const QMetaEnum &metaEnum, int key) const;

signals:
    void idChanged(const QString &newId, const QString &oldId);

private:
    KDReports::ImageElement m_imageElement;
    QString m_filename;
    qreal m_lastWidthPercent;
    qreal m_lastHeightPercent;
    qreal m_lastWidthMillimeters;
    qreal m_lastHeightMillimeters;
};

#endif // IMAGEELEMENTDATA_H
