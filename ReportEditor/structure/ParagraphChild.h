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

#ifndef PARAGRAPHCHILD_H
#define PARAGRAPHCHILD_H

#include "ObjectData.h"
#include <QColor>

class EDITOR_EXPORT ParagraphChild : public ObjectData
{
    Q_OBJECT
public:
    explicit ParagraphChild();
    bool isInline() const;
    QString alignment() const;
    QColor paragraphBackground() const;
    virtual int endPosition() const;
    virtual int elementSize() const = 0;
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    virtual void addToHierarchy(ObjectData *parent, int cursorPosition, EditorData * editor);
    virtual void doRemove();
    void addToHierarchyPrevious(ObjectData *parent, EditorData *editor);
    void addToHierarchyNext(ObjectData *parent, EditorData *editor);
protected:
    int m_cursorSize;
};

#endif // PARAGRAPHCHILD_H
