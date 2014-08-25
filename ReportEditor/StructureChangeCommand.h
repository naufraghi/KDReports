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

#ifndef STRUCTURECHANGECOMMAND_H
#define STRUCTURECHANGECOMMAND_H

#include <QUndoCommand>
#include <QPair>
#include "structure/CellData.h"

class ObjectData;
class TableElementData;
class ReportStructureModel;
class HeaderData;

class RemoveObjectCommand : public QUndoCommand
{
public:
    RemoveObjectCommand(ObjectData *object, ReportStructureModel *model, QUndoCommand * parent = 0);
    RemoveObjectCommand(ObjectData *object, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand * parent = 0);
    virtual ~RemoveObjectCommand();
    virtual void undo();
    virtual void redo();
    virtual void add();
    virtual void remove();
    virtual bool isValid();

protected:
    ObjectData* m_objectData;
    ObjectData* m_oldParent;
    int m_position;
    ReportStructureModel* m_model;
    bool m_valid;
};

class AddObjectCommand : public RemoveObjectCommand
{
public:
    AddObjectCommand(ObjectData *object, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand * parent = 0);
    virtual void undo();
    virtual void redo();
};

class MoveObjectCommand : public QUndoCommand
{
public:
    MoveObjectCommand(ObjectData *object, ObjectData *newParent, int position, ReportStructureModel *model, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();

protected:
    ObjectData* m_object;
    ObjectData* m_newParent;
    int m_newPosition;
    ObjectData* m_oldParent;
    int m_oldPosition;
    ReportStructureModel* m_model;
};

class RemoveTableCommand : public QUndoCommand
{
public:
    RemoveTableCommand(TableElementData *table, ReportStructureModel *model, QUndoCommand * parent = 0);
    RemoveTableCommand(TableElementData *table, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand * parent = 0);
    virtual void undo();
    virtual void redo();

protected:
    struct CellSave
    {
        int row;
        int column;
        CellData* cellData;
    };
    TableElementData* m_table;
    ObjectData* m_oldParent;
    int m_position;
    ReportStructureModel* m_model;
    QList<CellSave> m_cellList;
};

class InsertTableCommand : public RemoveTableCommand
{
public:
    InsertTableCommand(TableElementData *table, ObjectData *objectParent, int position, ReportStructureModel *model, QUndoCommand * parent = 0);
    virtual void undo();
    virtual void redo();
};

class RemoveHeaderCommand : public QUndoCommand
{
public:
    RemoveHeaderCommand(HeaderData* header, ReportData* report, QUndoCommand * parent = 0);
    virtual void undo();
    virtual void redo();
private:
    HeaderData* m_header;
    ReportData* m_report;
};

class InsertHeaderCommand : public RemoveHeaderCommand
{
public:
    InsertHeaderCommand(HeaderData* header, ReportData *report, QUndoCommand *parent = 0);
    virtual void undo();
    virtual void redo();
};

#endif // STRUCTURECHANGECOMMAND_H
