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

#ifndef HLINEELEMENTDATA_H
#define HLINEELEMENTDATA_H

#include "KDReportsHLineElement.h"
#include "TopElement.h"
#include <QColor>
#include "editor_export.h"

class ReportTextEdit;

class EDITOR_EXPORT HLineElementData : public TopElement
{
    Q_OBJECT
    Q_PROPERTY (QColor color READ color WRITE setColor)
    Q_PROPERTY (int thickness READ thickness WRITE setThickness)
    Q_PROPERTY (int margin READ margin WRITE setMargin)

public:
    explicit HLineElementData(const KDReports::HLineElement& hLineElement);
    QColor color() const;
    void setColor(const QColor& color);
    int thickness() const;
    void setThickness(int thickness);
    int margin();
    void setMargin(int margin);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    virtual QRect elementRect(ReportTextEdit *reportTextEdit);
    virtual void doAddToReport();
    virtual void doUpdate();
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    virtual void doRemove();
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

private:
    KDReports::HLineElement m_hLineElement;
};

#endif // HLINEELEMENTDATA_H
