/****************************************************************************
** Copyright (C) 2012-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#include "PageBreakTextObject.h"

#include <QTextDocument>
#include <QTextFormat>
#include <QTextFrame>
#include <QPainter>
#include <QRectF>
#include <QFontMetrics>

void PageBreakTextObject::registerPageBreakObjectHandler(QTextDocument *doc)
{
    PageBreakTextObject *pageBreakInterface = new PageBreakTextObject;

     //This assert is here because a bad build environment can cause this to fail. There is a note
    // in the Qt source that indicates an error should be output, but there is no such output.
    Q_ASSERT(qobject_cast<QTextObjectInterface *>(pageBreakInterface));

    doc->documentLayout()->registerHandler( PageBreakTextObject::PageBreakTextFormat, pageBreakInterface );
}


QSizeF PageBreakTextObject::intrinsicSize(QTextDocument *doc, int posInDocument,
            const QTextFormat &format)
{
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);

    QTextFrameFormat f = doc->rootFrame()->frameFormat();
    const qreal width = doc->pageSize().width() - (f.leftMargin() + f.rightMargin());
    QString pageBreak = tr("Page Break");
    QFont font;
    QFontMetrics fm(font);
    QSize s = fm.size(Qt::TextSingleLine, pageBreak);
    //Add 8 px of margin
    return QSizeF(width, s.height() + 8);
}

void PageBreakTextObject::drawObject(QPainter *painter, const QRectF &r,
            QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);

    const qreal y = r.top() + ( r.height() / 2 );

    QString pageBreak = tr("Page Break");
    painter->drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, pageBreak);

    QSize s = painter->fontMetrics().size(Qt::TextSingleLine, pageBreak);

    QPen p;
    p.setStyle(Qt::DashLine);
    painter->setPen(p);

    // Draw first line, end 4 px before text
    int right = (r.right() + r.left() - s.width()) / 2 - 4;
    QLineF line( r.left(), y, right, y );
    painter->drawLine(line);

    // Draw second line, start 4 px after text
    int left = (r.right() + r.left() + s.width()) / 2 + 4;
    QLineF line2(left, y, r.right(), y);
    painter->drawLine(line2);
}
