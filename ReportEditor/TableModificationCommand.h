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

#ifndef TABLEMODIFICATIONCOMMAND_H
#define TABLEMODIFICATIONCOMMAND_H

#include <QUndoCommand>
#include "structure/TableElementData.h"
#include "structure/CellData.h"

class ReportStructureModel;

class InsertRowCommand : public QUndoCommand
{
public:
    InsertRowCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand *parent);
    virtual void undo();
    virtual void redo();
protected:
    TableElementData *m_table;
    int m_pos;
    QList<CellData*> m_cellList;
    ReportStructureModel* m_model;
};

class RemoveRowCommand : public InsertRowCommand
{
public:
    RemoveRowCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand *parent);
    virtual void undo();
    virtual void redo();
};

class InsertColumnCommand : public QUndoCommand
{
public:
    InsertColumnCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand* parent);
    virtual void undo();
    virtual void redo();
protected:
    TableElementData *m_table;
    int m_pos;
    QList<CellData*> m_cellList;
    ReportStructureModel* m_model;
};

class RemoveColumnCommand : public InsertColumnCommand
{
public:
    RemoveColumnCommand(TableElementData *table, int pos, ReportStructureModel *model, QUndoCommand* parent);
    virtual void undo();
    virtual void redo();
};
#endif // TABLEMODIFICATIONCOMMAND_H
