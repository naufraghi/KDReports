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

#ifndef AUTOTABLEELEMENTDATA_H
#define AUTOTABLEELEMENTDATA_H

#include "KDReportsAutoTableElement.h"
#include "ParagraphChild.h"
#include <QBrush>
#include "editor_export.h"

class QTextTable;

class EDITOR_EXPORT AutoTableElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QString modelKey READ modelKey WRITE setModelKey)
    Q_PROPERTY (QBrush background READ background WRITE setBackground)
    Q_PROPERTY (double border READ border WRITE setBorder)
    Q_PROPERTY (double width READ width WRITE setWidth)
    Q_PROPERTY (QBrush headerBackground READ headerBackground WRITE setHeaderBackground)
    Q_PROPERTY (bool verticalHeaderVisible READ verticalHeaderVisible WRITE setVerticalHeaderVisible)
    Q_PROPERTY (bool horizontalHeaderVisible READ horizontalHeaderVisible WRITE setHorizontalHeaderVisible )

public:
    explicit AutoTableElementData(const KDReports::AutoTableElement& autoTableElement, const QString& modelKey);
    QString modelKey() const;
    void setModelKey(const QString& modelKey);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    double border() const;
    void setBorder(double border);
    double width() const;
    void setWidth(double width);
    QBrush headerBackground() const;
    void setHeaderBackground(const QBrush& headerBackground);
    bool verticalHeaderVisible() const;
    void setVerticalHeaderVisible(bool verticalHeaderVisible);
    bool horizontalHeaderVisible() const;
    void setHorizontalHeaderVisible(bool horizontalHeaderVisible);
    virtual void writeXml(QXmlStreamWriter &writer);
    void doAddToReport();
    int elementSize() const;
    void validate();
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

signals:
    void modelKeyChanged(const QString &newModelKey, const QString &oldModelKey);

private:
    KDReports::AutoTableElement m_autoTableElement;
    QString m_modelKey;
    QTextTable* m_textTable;
};

#endif // AUTOTABLEELEMENTDATA_H
