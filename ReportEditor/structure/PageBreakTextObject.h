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

#ifndef PAGEBREAKTEXTOBJECT_H
#define PAGEBREAKTEXTOBJECT_H

#include <QTextObjectInterface>

class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;

class PageBreakTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum { PageBreakTextFormat = QTextFormat::UserObject + 2 };

    /**
     * Registers the PageBreakTextObject (a QTextObjectInterface)
     * handler with a QTextDocument in order to enable the
     * painting of PageBreakTextObjects
     */
    static void registerPageBreakObjectHandler(QTextDocument *doc);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);

    void drawObject(QPainter *painter, const QRectF &rect,
                    QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);
};

#endif // PAGEBREAKTEXTOBJECT_H
