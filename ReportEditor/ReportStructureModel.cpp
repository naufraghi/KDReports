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

#include "ReportStructureModel.h"

#include "structure/ReportData.h"

ReportStructureModel::ReportStructureModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_reportData(0)
{
}

QModelIndex ReportStructureModel::index(int row, int column, const QModelIndex &parent) const
{
    if(m_reportData){
        if (!hasIndex(row, column, parent))
            return QModelIndex();
        if(!parent.isValid())
            return createIndex(row, column, m_reportData);

        ObjectData* parentItem;
        parentItem = static_cast<ObjectData*>(parent.internalPointer());
        ObjectData *childItem = parentItem->childrenElements().at(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex ReportStructureModel::parent(const QModelIndex &child) const
{
    if(m_reportData){
        if (!child.isValid())
            return QModelIndex();
        ObjectData* childItem = static_cast<ObjectData*>(child.internalPointer());
        if(!childItem)
            return QModelIndex();
        ObjectData* parentItem = childItem->parentElement();

        if(!parentItem)
            return QModelIndex();
        if (parentItem && parentItem == m_reportData)
            return createIndex(0, 0, m_reportData);
        return createIndex(parentItem->parentElement()->childrenElements().indexOf(parentItem), 0, parentItem);
    }
    return QModelIndex();
}

int ReportStructureModel::rowCount(const QModelIndex &parent) const
{
    if(m_reportData){
        ObjectData *parentItem;
        if (parent.column() > 0)
            return 0;
        if (!parent.isValid())
            return 1;
        else
            parentItem = static_cast<ObjectData*>(parent.internalPointer());
        return parentItem->childrenElements().size();
    }
    return 0;
}

int ReportStructureModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ReportStructureModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
          return QVariant();
    switch (role) {
    case Qt::DisplayRole:
        ObjectData *item = static_cast<ObjectData*>(index.internalPointer());
        return item->objectName();
    }
    return QVariant();
}

void ReportStructureModel::setReportData(ObjectData *reportData)
{
    beginResetModel();
    m_reportData = reportData;
    endResetModel();
}

QVariant ReportStructureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        switch(section){
        case 0: // First column, type
            return tr("Type");
        }

    }
    return QVariant();
}

ObjectData * ReportStructureModel::objectData(const QModelIndex &index)
{
    return static_cast<ObjectData*>(index.internalPointer());
}

bool ReportStructureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    objectData(parent.child(row, 0))->setParentElement(0);
    endRemoveRows();
    return true;
}

bool ReportStructureModel::removeObjectData(ObjectData* object)
{
    if (m_reportData) {
        if (!object || object == m_reportData)
            return false;
        ObjectData* parentItem = object->parentElement();
        if (!parentItem) // can't delete report
            return false;

        QModelIndex idx(index(object));
        return removeRow(idx.row(), idx.parent());
    }
    return false;
}

bool ReportStructureModel::insertObjectData(ObjectData* object, ObjectData* parent, int position)
{
    if (m_reportData) {
        if (!object)
            return false;
        if (!parent)
            return false;
        beginInsertRows(index(parent), position, position);
        object->setParentElement(parent, position);
        endInsertRows();
        return true;
    }
    return false;
}

QModelIndex ReportStructureModel::index(ObjectData* object) const
{
    if (!object)
        return QModelIndex();
    if (!object->parentElement())
        return createIndex(0, 0, object);
    else
        return createIndex(object->parentElement()->childrenElements().indexOf(object), 0, object);
}

bool ReportStructureModel::moveObjectData(ObjectData* object, ObjectData* newParent, int position)
{
    if (m_reportData) {
        if (!object || !newParent)
            return false;
        const int oldPosition = object->parentElement()->childrenElements().indexOf(object);
        beginMoveRows(index(object->parentElement()), oldPosition, oldPosition, index(newParent), position);
        object->setParentElement(newParent, position);
        endMoveRows();
    }
    return true;
}
