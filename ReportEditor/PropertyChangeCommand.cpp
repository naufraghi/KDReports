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

#include "PropertyChangeCommand.h"
#include "structure/ObjectData.h"
#include "structure/TextElementData.h"
#include <QDebug>

PropertyChangeCommand::PropertyChangeCommand(ObjectData *object, QMetaProperty property, const QVariant& newValue, int id, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_objectData(object)
    , m_property(property)
    , m_oldValue(m_objectData->property(m_property.name()))
    , m_newValue(newValue)
    , m_id(id)
{
    setText(object->metaObject()->className() + QString::fromLatin1(": ") + QString::fromLatin1(property.name()));
}

void PropertyChangeCommand::undo()
{
    performChange(m_oldValue);
}

void PropertyChangeCommand::redo()
{
    performChange(m_newValue);
}

QString PropertyChangeCommand::text() const
{
    return QString::fromLatin1("Property");
}

void PropertyChangeCommand::performChange(const QVariant& value)
{
    if (m_property.isEnumType()) {
        QMetaEnum enumerator = m_property.enumerator();
        if (!enumerator.isFlag()) {
            m_objectData->setProperty(m_property.name(), value);
        } else {
            QList<QVariant> values = qVariantValue<QList<QVariant> >(value);
            int flag = 0;
            foreach(const QVariant &v, values){
                int value = enumerator.value(v.toInt());
                    flag |= value;
            }
            m_objectData->setProperty(m_property.name(), flag);
        }
    } else {
        m_objectData->setProperty(m_property.name(), value);
    }
    m_objectData->update();
}

bool PropertyChangeCommand::mergeWith(const QUndoCommand *other)
{
    if (m_id != other->id())
        return false;
    PropertyChangeCommand* command = dynamic_cast<PropertyChangeCommand*>(const_cast<QUndoCommand*>(other));
    // insert text (1), backspace/delete(2)
    if ((m_id == 1 || m_id == 2) && m_objectData == command->m_objectData) {
        m_newValue = command->m_newValue;
        return true;
    }
    return false;
}

int PropertyChangeCommand::id() const
{
    return m_id;
}
