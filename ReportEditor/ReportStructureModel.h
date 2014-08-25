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

#ifndef REPORTSTRUCTUREMODEL_H
#define REPORTSTRUCTUREMODEL_H

#include <QAbstractItemModel>
#include "editor_export.h"

class ReportData;
class ObjectData;

/**
  * Model for report structure
  */
class EDITOR_EXPORT ReportStructureModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ReportStructureModel(QObject *parent = 0);

    /**
      * \reimp
      */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    /**
      * \reimp
      */
    virtual QModelIndex parent(const QModelIndex &child) const;

    /**
      * \reimp
      */
    virtual int rowCount(const QModelIndex &parent) const;

    /**
      * \reimp
      */
    virtual int columnCount(const QModelIndex &parent) const;

    /**
      * \reimp
      */
    virtual QVariant data(const QModelIndex &index, int role) const;

    /**
      * \reimp
      */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
      * \reimp
      */
    virtual bool removeRows(int row, int count, const QModelIndex &parent);

    /**
      * Set the model data
      */
    void setReportData(ObjectData* reportData);

    /**
      * \return object data associated with index
      */
    ObjectData* objectData(const QModelIndex& index);
    bool removeObjectData(ObjectData *object);
    bool insertObjectData(ObjectData *object, ObjectData *parent, int position);

    QModelIndex index(ObjectData *object) const;
    bool moveObjectData(ObjectData *object, ObjectData *newParent, int position);
private:
    ObjectData* m_reportData;
};

#endif // REPORTSTRUCTUREMODEL_H
