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

#ifndef HTMLELEMENTDATA_H
#define HTMLELEMENTDATA_H

#include "KDReportsHtmlElement.h"
#include "ParagraphChild.h"
#include <QBrush>
#include "editor_export.h"

class EDITOR_EXPORT HtmlElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QString html READ html WRITE setHtml)
    Q_PROPERTY (QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY (QBrush background READ background WRITE setBackground)

public:
    explicit HtmlElementData(const KDReports::HtmlElement& htmlElement);
    QString id() const;
    void setId(const QString &id);
    QString html() const;
    void setHtml(const QString &html);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual void doUpdate();
    virtual int elementSize() const;
    QString translatePropertyName(const QString &name) const;

signals:
    void idChanged(const QString &newId, const QString &oldId);

private:
    KDReports::HtmlElement m_htmlElement;
};

#endif // HTMLELEMENTDATA_H
