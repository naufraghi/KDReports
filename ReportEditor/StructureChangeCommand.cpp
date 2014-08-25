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

#include "StructureChangeCommand.h"
#include "ReportStructureModel.h"
#include "structure/ObjectData.h"
#include "structure/TableElementData.h"
#include "structure/TextElementData.h"
#include "structure/ReportData.h"
#include "structure/HeaderData.h"
#include <QStack>
#include <QObject>

//
// RemoveObjectCommand
//

RemoveObjectCommand::RemoveObjectCommand(ObjectData *object, ReportStructureModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_objectData(object)
    , m_oldParent(m_objectData->parentElement())
    , m_position(m_oldParent->childrenElements().indexOf(m_objectData))
    , m_model(model)
    , m_valid(true)
{
    setText(QObject::tr("Remove %1").arg(m_objectData->objectName()));
}

RemoveObjectCommand::RemoveObjectCommand(ObjectData *object, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_objectData(object)
    , m_oldParent(objectParent)
    , m_position(position)
    , m_model(model)
    , m_valid(true)
{
    setText(QObject::tr("Remove %1").arg(m_objectData->objectName()));
}

RemoveObjectCommand::~RemoveObjectCommand()
{
    if(!m_objectData->parentElement())
        m_objectData->deleteLater();
}

void RemoveObjectCommand::undo()
{
    add();
}

void RemoveObjectCommand::redo()
{
    remove();
}

void RemoveObjectCommand::add()
{
    if (!m_oldParent)
        return;
    m_model->insertObjectData(m_objectData, m_oldParent, m_position);
    m_objectData->addToReport();
}

void RemoveObjectCommand::remove()
{
    if (!m_oldParent)
        return;
    m_position = m_oldParent->childrenElements().indexOf(m_objectData);
    m_objectData->remove();
    m_model->removeObjectData(m_objectData);
}

bool RemoveObjectCommand::isValid()
{
    return m_valid;
}

//
// AddObjectCommand
//

AddObjectCommand::AddObjectCommand(ObjectData *object, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand *parent)
    : RemoveObjectCommand(object, objectParent, position, model, parent)
{
    setText(QObject::tr("Add %1").arg(m_objectData->objectName()));
}

void AddObjectCommand::undo()
{
    RemoveObjectCommand::redo();
}

void AddObjectCommand::redo()
{
    RemoveObjectCommand::undo();
}

//
// MoveObjectCommand
//

MoveObjectCommand::MoveObjectCommand(ObjectData* object, ObjectData* newParent, int position, ReportStructureModel *model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_object(object)
    , m_newParent(newParent)
    , m_newPosition(position)
    , m_oldParent(0)
    , m_oldPosition(-1)
    , m_model(model)
{
    if (m_object) {
        m_oldParent = object->parentElement();
        if (m_oldParent)
            m_oldPosition = m_oldParent->childrenElements().indexOf(object);
    }
}

void MoveObjectCommand::undo()
{
    if (!m_oldParent || !m_newParent)
        return;
    m_object->doRemove();
    m_model->moveObjectData(m_object, m_oldParent, m_oldPosition);
    m_object->addToReport();
}

void MoveObjectCommand::redo()
{
    if (!m_oldParent || !m_newParent)
        return;
    m_object->doRemove();
    m_model->moveObjectData(m_object, m_newParent, m_newPosition);
    m_object->addToReport();
}

//
// RemoveTableCommand
//

RemoveTableCommand::RemoveTableCommand(TableElementData *table, ReportStructureModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_table(table)
    , m_oldParent(table->parentElement())
    , m_position(m_oldParent->childrenElements().indexOf(table))
    , m_model(model)
{}

RemoveTableCommand::RemoveTableCommand(TableElementData *table, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_table(table)
    , m_oldParent(objectParent)
    , m_position(position)
    , m_model(model)
{}

void RemoveTableCommand::redo()
{
    m_cellList.clear();
    if (!m_oldParent)
        return;
    m_position = m_oldParent->childrenElements().indexOf(m_table);
    foreach (ObjectData *object, m_table->childrenElements()) {
        CellData* cell = qobject_cast<CellData*>(object);
        CellSave cellSave;
        cellSave.row = cell->row();
        cellSave.column = cell->column();
        cellSave.cellData = m_table->cellAt(cellSave.row, cellSave.column);
        m_cellList.append(cellSave);
    }
    foreach (CellSave cell, m_cellList) {
        m_model->removeObjectData(cell.cellData);
        cell.cellData->setParentElement(0);
    }
    m_model->removeObjectData(m_table);
    m_table->remove();
}

void RemoveTableCommand::undo()
{
    if (!m_oldParent)
        return;
    if (m_position >= m_oldParent->childrenElements().size())
        m_model->insertObjectData(m_table, m_oldParent, m_position);
    else {
        QStack<ObjectData *> stack;
        for (int i = m_oldParent->childrenElements().size() - 1;  i >= m_position; i--) {
            stack.append(m_oldParent->childrenElements().at(i));
            m_model->removeObjectData(stack.top());
        }
        m_model->insertObjectData(m_table, m_oldParent, m_position);
        while (!stack.isEmpty()) {
            m_model->insertObjectData(stack.pop(), m_oldParent, m_oldParent->childrenElements().size());
        }
    }

    m_table->addToReport();
    foreach (CellSave cell, m_cellList) {
        if (CellData* c = m_table->cellAt(cell.row, cell.column))
            delete c;
        int nbRows = m_table->rows();
        if (nbRows <= cell.row)
            m_table->insertRows(nbRows, cell.row - (nbRows - 1));
        int nbColumns = m_table->columns();
        if (nbColumns <= cell.column)
            m_table->insertColumn(nbColumns, cell.column - (nbColumns - 1));
        cell.cellData->updateCell(cell.row, cell.column);
        cell.cellData->doAddToReport();
    }
    for (int i = 0; i < m_table->childrenElements().size(); i++) {
        m_model->insertObjectData(m_table->childrenElements().at(i), m_table, i);
    }
    m_table->update();
}

//
// InsertTableCommand
//

InsertTableCommand::InsertTableCommand(TableElementData *table, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand *parent)
    : RemoveTableCommand(table, objectParent, position, model, parent)
{
}

void InsertTableCommand::undo()
{
    RemoveTableCommand::redo();
}

void InsertTableCommand::redo()
{
    RemoveTableCommand::undo();
}

RemoveHeaderCommand::RemoveHeaderCommand(HeaderData *header, ReportData* report, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_header(header)
    , m_report(report)
{
}

void RemoveHeaderCommand::redo()
{
    if (FooterData* footer = qobject_cast<FooterData*>(m_header)) {
        m_report->footerMap().remove(footer->location());
    } else {
        m_report->headerMap().remove(m_header->location());
    }
}

void RemoveHeaderCommand::undo()
{
    if (FooterData* footer = qobject_cast<FooterData*>(m_header)) {
        m_report->addFooter(footer);
    } else {
        m_report->addHeader(m_header);
    }
}

InsertHeaderCommand::InsertHeaderCommand(HeaderData *header, ReportData* report, QUndoCommand *parent)
    : RemoveHeaderCommand(header, report, parent)
{
}

void InsertHeaderCommand::undo()
{
    RemoveHeaderCommand::redo();
}

void InsertHeaderCommand::redo()
{
    RemoveHeaderCommand::undo();
}
