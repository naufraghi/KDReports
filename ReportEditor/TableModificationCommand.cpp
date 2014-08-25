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

#include "TableModificationCommand.h"

#include "ReportStructureModel.h"
#include <QDebug>
//
// InsertRowCommand
//

InsertRowCommand::InsertRowCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_table(table)
    , m_pos(pos)
    , m_model(model)
{
    setText("InsertRowCommand");
}

void InsertRowCommand::undo()
{
    m_cellList.clear();
    for (int i = 0; i < m_table->columns(); i++) {
        CellData *cell = m_table->cellAt(m_pos, i);
        m_model->removeObjectData(cell);
        cell->setParentElement(0);
        m_cellList.append(cell);
    }
    m_table->removeRows(m_pos, 1);
}

void InsertRowCommand::redo()
{
    m_table->insertRows(m_pos, 1);
    // update cell from m_cellList
    for (int i = 0; i < m_cellList.size(); i++) {
        m_cellList.at(i)->updateCell(m_pos, i);
        m_cellList.at(i)->doAddToReport();
    }

    // Add cell in the model
    for (int c = 0; c < m_table->columns(); c++) {
        CellData *cell = m_table->cellAt(m_pos, c);
        m_model->insertObjectData(cell, m_table, m_table->childrenElements().indexOf(cell));
    }
}

//
// RemoveRowCommand
//

RemoveRowCommand::RemoveRowCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand* parent)
    : InsertRowCommand(table, pos, model, parent)
{
}

void RemoveRowCommand::undo()
{
    InsertRowCommand::redo();
}

void RemoveRowCommand::redo()
{
    InsertRowCommand::undo();
}

//
// InsertColumnCommand
//

InsertColumnCommand::InsertColumnCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_table(table)
    , m_pos(pos)
    , m_model(model)
{
    setText("InsertColumnCommand");
}

void InsertColumnCommand::undo()
{
    m_cellList.clear();
    for (int i = 0; i < m_table->rows(); i++) {
        CellData *cell = m_table->cellAt(i, m_pos);
        m_model->removeObjectData(cell);
        cell->setParentElement(0);
        m_cellList.append(cell);
    }
    m_table->removeColumns(m_pos, 1);
}

void InsertColumnCommand::redo()
{
    m_table->insertColumn(m_pos, 1);

    // update cell from m_cellList
    for (int i = 0; i < m_cellList.size(); i++) {
        m_cellList.at(i)->updateCell(i, m_pos);
        m_cellList.at(i)->doAddToReport();
    }

    // Add cell in the model
    for (int r = 0; r < m_table->rows(); r++) {
        CellData *cell = m_table->cellAt(r, m_pos);
        m_model->insertObjectData(cell, m_table, m_table->childrenElements().indexOf(cell));
    }
}

//
// RemoveColumnCommand
//

RemoveColumnCommand::RemoveColumnCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand *parent)
    : InsertColumnCommand(table, pos, model, parent)
{
}

void RemoveColumnCommand::undo()
{
    InsertColumnCommand::redo();
}

void RemoveColumnCommand::redo()
{
    InsertColumnCommand::undo();
}
