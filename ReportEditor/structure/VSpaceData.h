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

#ifndef VSPACEDATA_H
#define VSPACEDATA_H

#include "TopElement.h"

class EDITOR_EXPORT VSpaceData : public TopElement
{
    Q_OBJECT
    Q_PROPERTY( int size READ size WRITE setSize )
public:
    explicit VSpaceData(int size);
    int size() const;
    void setSize(int size);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    virtual void doUpdate();
    virtual void doAddToReport();
    virtual void doRemove();
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

private:
    int m_size;
};

#endif // VSPACEDATA_H
