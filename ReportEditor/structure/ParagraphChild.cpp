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

#include "ParagraphChild.h"
#include "ParagraphData.h"
#include "TextElementData.h"
#include "../EditorData.h"
#include "CellData.h"
#include <QMetaEnum>
#include <QDebug>

ParagraphChild::ParagraphChild()
    : ObjectData(0)
    , m_cursorSize(0)
{
}

bool ParagraphChild::isInline() const
{
    return childPosition() > 0;
}

QString ParagraphChild::alignment() const
{
    QString loc(QString::fromLatin1("left"));
    if (ParagraphData* p = qobject_cast<ParagraphData*>(parentElement())) {
        int idx = p->metaObject()->indexOfEnumerator("Alignment");
        QMetaEnum meta = p->metaObject()->enumerator(idx);
        loc = QString::fromLatin1(meta.valueToKeys(p->alignment())).toLower();
        loc.remove(QString::fromLatin1("align"));
    }
    return loc;
}

QColor ParagraphChild::paragraphBackground() const
{
    if (ParagraphData* p = qobject_cast<ParagraphData*>(parentElement()))
        return p->background().color();
    return QColor();
}

int ParagraphChild::endPosition() const
{
    if (m_onDocument)
        return startPosition() + elementSize();
    else
        return startPosition();
}

bool ParagraphChild::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    int position = -1;
    if (cursorPosition == startPosition()) {
        if (TextElementData* textElementData = qobject_cast<TextElementData*>(previousChild())) {
            return textElementData->insertText(editorData, text, cursorPosition);
        } else {
            position = childPosition();
        }
    } else if (cursorPosition == endPosition()) {
        position = childPosition() + 1;
    } else {
        qWarning() << "write text on this object" << this << "not yet supported";
        return false;
    }
    if (position >= 0)
        return editorData->insertSubElement(parentElement(), new TextElementData(text), position);
    return false;
}

void ParagraphChild::addToHierarchy(ObjectData *parent, int cursorPosition, EditorData * editor)
{
    /* if the parent is a paragraph or a cell we directly insert object at end of it
    else if is a paragraph child insertion depends on cursor position (TODO) */
    if (parent->startPosition() == cursorPosition) {
        addToHierarchyPrevious(parent, editor);
    } else if (parent->endPosition() == cursorPosition) {
        addToHierarchyNext(parent, editor);
    } else {
        qDebug() << "need to support that";
    }
}

void ParagraphChild::addToHierarchyPrevious(ObjectData *parent, EditorData *editor)
{
    int pos = 0;
    ObjectData *p = parent;
    if (qobject_cast<ParagraphChild*>(parent)) {
        p = parent->parentElement();
        pos = parent->childPosition();
    } else if (!qobject_cast<ParagraphData*>(p)) {
        if (p->parentElement()) {
            pos = p->childPosition();
            p = p->parentElement();
        }
    }
    editor->insertSubElement(p, this, pos);
}

void ParagraphChild::addToHierarchyNext(ObjectData *parent, EditorData *editor)
{
    int pos = 0;
    ObjectData *p = parent;
    if (qobject_cast<ParagraphChild*>(parent)) {
        p = parent->parentElement();
        pos = parent->childPosition() + 1;
    } else if (!qobject_cast<ParagraphData*>(p)) {
        if (p->parentElement()) {
            pos = p->childPosition() + 1;
            p = p->parentElement();
        }
    }
    editor->insertSubElement(p, this, pos);
}

void ParagraphChild::doRemove()
{
    ObjectData::doRemove();
    m_onDocument = false;
}
