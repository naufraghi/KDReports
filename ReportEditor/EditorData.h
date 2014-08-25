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

#ifndef EDITORDATA_H
#define EDITORDATA_H

#include <QObject>
#include <QString>
#include <QModelIndex>
#include <QTextDocument>
#include <structure/HeaderData.h>
#include "editor_export.h"

class ReportData;
class ObjectData;
class ReportStructureModel;
class QUndoStack;
class QIODevice;
class TableElementData;
class QUndoCommand;
namespace KDReports {
class Report;
}

class EDITOR_EXPORT EditorData : public QObject
{
    Q_OBJECT
public:
    enum SubElementPosition {
        FirstChild,
        LastChild,
        PreviousSibling,
        NextSibling
    };

    enum Scope {Body, Header, Footer};

    EditorData(QObject *parent = 0);
    ~EditorData();
    QUndoStack* undoStack() const;
    ReportData* reportData() const;
    KDReports::Report* report() const;
    bool loadReport(QIODevice *file);
    void newReport();
    QString errorMessage() const;
    ReportStructureModel* model();
    bool insertSubElement(ObjectData *parent, ObjectData *child, int position);
    bool insertSubElement(ObjectData *parent, ObjectData *child, EditorData::SubElementPosition position);
    void changeProperty(ObjectData *object, QMetaProperty property, const QVariant &newValue, int id = -1);
    void changeProperty(ObjectData *object, const QString &propertyName, const QVariant &newValue, int id = -1);
    QModelIndex selectedIndex() const;
    ObjectData* selectedObject() const;
    void insertRow(TableElementData *table, int position);
    void insertColumn(TableElementData *table, int position);
    void removeRow(TableElementData *table, int position);
    void removeColumn(TableElementData *table, int position);
    void moveElement(ObjectData *object, ObjectData *newParent, int position);
    QTextDocument& document();
    void changeObject(ObjectData *object);
    void removeObject(ObjectData *object);
    bool isSaved();
    void closeReport();
    void setSaved();
    void beginCommand();
    void endCommand();
    bool canChangeObject() const;
    void setScope(EditorData::Scope scope);
    void setLocations(HeaderData::Locations locations);
    void updateModel();
    ObjectData *topHierarchyElement() const;
    void registerSpecialTextObject();
    void insertHeader(HeaderData::Locations loc);
    void insertFooter(HeaderData::Locations loc);

public slots:
    void changeIndex(const QModelIndex &index);
    void removeCurrentObject();
signals:
    void indexChanged(const QModelIndex&);
    void headerChanged();
    void headerAdded(HeaderData*);
private:
    KDReports::Report* m_report;
    ReportData *m_reportData;
    ReportStructureModel *m_model;
    QUndoStack* m_undoStack;
    QString m_error;
    ObjectData* m_currentObject;
    QModelIndex m_currentIndex;
    bool m_saved;
    QUndoCommand *m_currentCmd;
    int m_composedCommand;
    bool m_changeObject;
    Scope m_scope;
    HeaderData::Locations m_locations;
};

#endif // EDITORDATA_H
