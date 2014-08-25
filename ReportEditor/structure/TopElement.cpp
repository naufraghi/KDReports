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

#include "TopElement.h"

TopElement::TopElement()
    : ObjectData(0)
    , m_lineReturn(false)
{
}

void TopElement::validate()
{
    if (nextChild()) {
        m_lineReturn = true;
    }
}

void TopElement::insertLineBefore(QTextCursor cursor)
{
    ObjectData* previousObject = previousChild();
    if (previousObject) {
        TopElement* previous = qobject_cast<TopElement*>(previousObject);
        if (previous && !previous->m_lineReturn) {
            cursor.insertBlock();
            previous->m_lineReturn = true;
        }
    }
}

void TopElement::insertLineAfter(QTextCursor cursor, const QTextBlockFormat &f)
{
    if ( nextChild() && nextChild()->isOnDocument()){
        cursor.insertBlock();
        cursor.setBlockFormat(f);
        m_lineReturn = true;
    } else {
        m_lineReturn = false;
    }
}

void TopElement::doRemove()
{
    ObjectData::doRemove();
    TopElement* previous = qobject_cast<TopElement*>(previousChild());
    if (previous && previous->isOnDocument()) {
        if (ObjectData* next = nextChild())
            previous->m_lineReturn = next->isOnDocument();
        else
            previous->m_lineReturn = false;
    }
}
