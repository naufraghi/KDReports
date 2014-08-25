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

#ifndef CHARTELEMENTDATA_H
#define CHARTELEMENTDATA_H

#include "KDReportsChartElement.h"
#include "ParagraphChild.h"
#include <QBrush>
#include <QSizeF>

class ChartElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QString modelKey READ modelKey WRITE setModelKey)
    Q_PROPERTY (QBrush background READ background WRITE setBackground)
    Q_PROPERTY (QSizeF size READ size WRITE setSize)

public:
    explicit ChartElementData(const KDReports::ChartElement &chartElement, const QString& modelKey);
    QString modelKey() const;
    void setModelKey(const QString& modelKey);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    QSizeF size() const;
    void setSize(QSizeF size);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int elementSize() const;
    QString translatePropertyName(const QString &name) const;

private:
    KDReports::ChartElement m_chartElement;
    QString m_modelKey;
};

#endif // CHARTELEMENTDATA_H
