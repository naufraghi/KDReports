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

#ifndef PROPERTYCHANGECOMMAND_H
#define PROPERTYCHANGECOMMAND_H

#include <QUndoCommand>
#include <QMetaProperty>

class ObjectData;

class PropertyChangeCommand : public QUndoCommand
{
public:
    PropertyChangeCommand(ObjectData *object, QMetaProperty property, const QVariant &newValue, int id, QUndoCommand* parent);
    virtual void undo();
    virtual void redo();
    virtual bool mergeWith(const QUndoCommand *other);
    virtual int id() const;
    void performChange(const QVariant &value);
    QString text() const;

private:
    ObjectData* m_objectData;
    QMetaProperty m_property;
    QVariant m_oldValue;
    QVariant m_newValue;
    int m_id;
};

#endif // PROPERTYCHANGECOMMAND_H
