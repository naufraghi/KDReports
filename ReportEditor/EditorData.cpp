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

#include "EditorData.h"
#include <QUndoStack>
#include "structure/ReportData.h"
#include "structure/HLineElementData.h"
#include "structure/ParagraphData.h"
#include "structure/CellData.h"
#include "structure/ParagraphChild.h"
#include "ReportStructureModel.h"
#include "ReportObjectXmlElementHandler.h"
#include <QDebug>
#include "StructureChangeCommand.h"
#include "PropertyChangeCommand.h"
#include "TableModificationCommand.h"
#include <KDReportsTextDocument_p.h>
#include "structure/PageBreakTextObject.h"

EditorData::EditorData(QObject *parent)
    : QObject(parent)
    , m_report(0)
    , m_reportData(0)
    , m_model(new ReportStructureModel(this))
    , m_undoStack(new QUndoStack(this))
    , m_currentObject(0)
    , m_saved(false)
    , m_currentCmd(0)
    , m_composedCommand(0)
    , m_changeObject(true)
    , m_scope(Body)
{
}

EditorData::~EditorData()
{
    delete m_undoStack;
    delete m_model;
    delete m_reportData;
    delete m_report;
}

QUndoStack * EditorData::undoStack() const
{
    return m_undoStack;
}

ReportData * EditorData::reportData() const
{
    return m_reportData;
}

ObjectData * EditorData::topHierarchyElement() const
{
    if (m_scope == Body)
        return m_reportData;
    else if (m_scope == Header)
        return m_reportData->headerMap().value(m_locations);
    else if (m_scope == Footer)
        return m_reportData->footerMap().value(m_locations);
    return 0;
}

bool EditorData::loadReport(QIODevice *file)
{
    KDReports::Report* newReport = new KDReports::Report( this );
    ReportObjectXmlElementHandler handler;
    newReport->setXmlElementHandler(&handler);

    KDReports::ErrorDetails details;
    m_error = QString();
    if ( newReport->loadFromXML( file, &details ) ) {
        closeReport();
        m_report = newReport;
        m_reportData = handler.reportData();
        m_model->setReportData(m_reportData);
        registerSpecialTextObject();
        m_saved = true;
        m_undoStack->setClean();
        emit headerChanged();
        return true;
    } else {
        m_error = details.message();
        delete newReport;
        return false;
    }
}

void EditorData::newReport()
{
    closeReport();
    m_report = new KDReports::Report( this );
    m_reportData = new ReportData(*m_report);
    m_model->setReportData(m_reportData);
    registerSpecialTextObject();
    changeObject(m_reportData);
    m_saved = false;
    emit headerChanged();
}

void EditorData::registerSpecialTextObject()
{
    PageBreakTextObject::registerPageBreakObjectHandler(&m_reportData->textDocumentData().document());
}

bool EditorData::isSaved()
{
    if (!m_reportData)
        return true;
    return m_undoStack->isClean() && m_saved;
}

void EditorData::setSaved()
{
    m_saved = true;
    m_undoStack->setClean();
}

void EditorData::closeReport()
{
    if (!m_reportData)
        return;
    m_undoStack->clear();
    m_model->setReportData(0);
    delete m_reportData;
    m_reportData = 0;
    m_report->deleteLater();
    m_report = 0;
    m_undoStack->setClean();
}

KDReports::Report * EditorData::report() const
{
    return m_report;
}

QString EditorData::errorMessage() const
{
    return m_error;
}

ReportStructureModel * EditorData::model()
{
    return m_model;
}

bool EditorData::insertSubElement(ObjectData* parent, ObjectData* child, int position)
{
    if (qobject_cast<CellData*>(child))
        return false;
    ObjectData* p = parent;
    QList<RemoveObjectCommand*> list;
    beginCommand();
    QUndoCommand* command = new QUndoCommand(m_currentCmd);
    if (qobject_cast<ParagraphChild*>(child) && !qobject_cast<ParagraphData*>(parent)) {
        if (qobject_cast<ReportData*>(parent) || qobject_cast<HeaderData*>(parent) || qobject_cast<CellData*>(parent)) {
            //TODO set the previous/next paragraph
            p = new ParagraphData(0);
            list.append(new AddObjectCommand(p, parent, position, m_model, command));
            position = 0;
            connect(p, SIGNAL(updated()), m_reportData, SLOT(slotUpdate()));
        } else if (qobject_cast<HLineElementData*>(parent)) {
            int pos = parent->parentElement()->childrenElements().indexOf(parent);
            p = new ParagraphData(0);
            list.append(new AddObjectCommand(p, parent->parentElement(), pos + 1, m_model, command));
            connect(p, SIGNAL(updated()), m_reportData, SLOT(slotUpdate()));
        }
    }
    if (TableElementData* table = qobject_cast<TableElementData*>(child))
        new InsertTableCommand(table, p, position, m_model, command);
    else if (qobject_cast<HLineElementData*>(child) && qobject_cast<ParagraphData*>(parent) && parent->childrenElements().isEmpty()) {
        p = parent->parentElement();
        position = p->childrenElements().indexOf(parent);
        list.append(new RemoveObjectCommand(parent, m_model, command));
        list.append(new AddObjectCommand(child, p, position, m_model, command));
    } else {
        list.append(new AddObjectCommand(child, p, position, m_model, command));
    }
    connect(child, SIGNAL(updated()), m_reportData, SLOT(slotUpdate()));
    command->setText(QString::fromLatin1("Add %1").arg(child->objectName()));
    endCommand();
    bool valid = true;
    foreach (RemoveObjectCommand* addCommand, list) {
        valid = valid && addCommand->isValid();
    }
    QVariant id = child->property("id");
    if (id.isValid()) {
        connect(child, SIGNAL(idChanged(QString, QString)), m_reportData, SLOT(slotIdChanged(QString, QString)));
    }
    QVariant model = child->property("modelKey");
    if (model.isValid()) {
        m_reportData->addModelKey(model.toString(), child);
        connect(child, SIGNAL(modelKeyChanged(QString, QString)), m_reportData, SLOT(slotModelKeyChanged(QString,QString)));
    }
    return valid;
}

void EditorData::insertHeader(HeaderData::Locations loc)
{
    beginCommand();
    HeaderData* header = new HeaderData(m_report->header(KDReports::HeaderLocations((int)loc)));
    new InsertHeaderCommand(header, m_reportData, m_currentCmd);
    connect(header, SIGNAL(updated()), m_reportData, SLOT(slotUpdate()));
    endCommand();
    m_locations = loc;
    emit headerAdded(header);
}

void EditorData::insertFooter(FooterData::Locations loc)
{
    beginCommand();
    FooterData* footer = new FooterData(m_report->footer(KDReports::HeaderLocations((int)loc)));
    new InsertHeaderCommand(footer, m_reportData, m_currentCmd);
    connect(footer, SIGNAL(updated()), m_reportData, SLOT(slotUpdate()));
    endCommand();
    m_locations = loc;
    emit headerAdded(footer);
}

void EditorData::moveElement(ObjectData* object, ObjectData* newParent, int position)
{
    beginCommand();
    new MoveObjectCommand(object, newParent, position, m_model, m_currentCmd);
    endCommand();
}

bool EditorData::insertSubElement(ObjectData *parent, ObjectData *child, EditorData::SubElementPosition position)
{
    int pos = 0;
    int posChild = 0;
    switch (position) {
    case EditorData::FirstChild:
        pos = 0;
        break;
    case EditorData::PreviousSibling:
        posChild = parent->childrenElements().indexOf(child);
        pos = qMax(posChild - 1, 0);
        break;
    case EditorData::NextSibling:
        posChild = parent->childrenElements().indexOf(child);
        pos = qMax(posChild + 1, parent->childrenElements().size());
        break;
    case EditorData::LastChild:
        pos = parent->childrenElements().size();
        break;
    }
    return insertSubElement(parent, child, pos);
}

void EditorData::changeProperty(ObjectData *object, QMetaProperty property, const QVariant& newValue, int id)
{
    beginCommand();
    new PropertyChangeCommand(object, property, newValue, id, m_currentCmd);
    endCommand();
}

void EditorData::beginCommand()
{
    if (m_composedCommand == 0)
        m_currentCmd = new QUndoCommand();
    m_composedCommand++;
}

void EditorData::endCommand()
{
    if (!--m_composedCommand) {
        if (m_currentCmd->childCount() == 1) {
            QUndoCommand* command = const_cast<QUndoCommand*>(m_currentCmd->child(0));
            if (dynamic_cast<PropertyChangeCommand*>(command))
                m_changeObject = false;
            m_undoStack->push(command);
            m_changeObject = true;
        } else {
            m_undoStack->push(m_currentCmd);
        }
    }
}

void EditorData::changeProperty(ObjectData *object, const QString &propertyName, const QVariant &newValue, int id)
{
    QMetaProperty metaProperty;
    for(int i=1; i< object->metaObject()->propertyCount();i++){
        if(object->metaObject()->property(i).name() == propertyName.toLatin1()){
            metaProperty = object->metaObject()->property(i);
            break;
        }
    }

    changeProperty(object, metaProperty, newValue, id);
}

void EditorData::changeIndex(const QModelIndex &index)
{
    if (m_currentIndex != index) {
        m_currentIndex = index;
        m_currentObject = m_model->objectData(index);
        emit indexChanged(m_currentIndex);
    }
}

void EditorData::changeObject(ObjectData* object)
{
    QModelIndex index(m_model->index(object));
    if (index != m_currentIndex) {
        m_currentIndex = index;
        m_currentObject = object;
        emit indexChanged(m_currentIndex);
    }
}

ObjectData * EditorData::selectedObject() const
{
    return m_currentObject;
}

QModelIndex EditorData::selectedIndex() const
{
    return m_currentIndex;
}

void EditorData::removeCurrentObject()
{
    if(!m_currentObject || qobject_cast<ReportData*>(m_currentObject))
        return;
    ObjectData* remove = m_currentObject;
    removeObject(remove);
}

void EditorData::removeObject(ObjectData* object)
{
    if (m_currentObject == object) {
        m_currentObject = 0;
        m_currentIndex = QModelIndex();
    }
    beginCommand();
    if (TableElementData* table = qobject_cast<TableElementData*>(object)) {
        new RemoveTableCommand(table, m_model, m_currentCmd);
    } else if (HeaderData * header = qobject_cast<HeaderData*>(object)) {
        new RemoveHeaderCommand(header, m_reportData, m_currentCmd);
    } else {
        new RemoveObjectCommand(object, m_model, m_currentCmd);
    }
    endCommand();
}

void EditorData::insertRow(TableElementData *table, int position)
{
    beginCommand();
    new InsertRowCommand(table, position, m_model, m_currentCmd);
    endCommand();
}

void EditorData::insertColumn(TableElementData *table, int position)
{
    beginCommand();
    new InsertColumnCommand(table, position, m_model, m_currentCmd);
    endCommand();
}

void EditorData::removeRow(TableElementData *table, int position)
{
    beginCommand();
    new RemoveRowCommand(table, position, m_model, m_currentCmd);
    endCommand();
}

void EditorData::removeColumn(TableElementData *table, int position)
{
    beginCommand();
    new RemoveColumnCommand(table, position, m_model, m_currentCmd);
    endCommand();
}

QTextDocument& EditorData::document()
{
    if (m_scope == Body)
        return report()->doc().contentDocument();
    else if (m_scope == Header)
        return reportData()->headerMap().value(m_locations)->textDocumentData().document();
    else // (m_scope == Footer)
        return reportData()->footerMap().value(m_locations)->textDocumentData().document();
}

bool EditorData::canChangeObject() const
{
    return m_changeObject;
}

void EditorData::setScope(EditorData::Scope scope)
{
    m_scope = scope;
    if (m_scope == Body)
        m_model->setReportData(m_reportData);
    updateModel();
}

void EditorData::setLocations(HeaderData::Locations locations)
{
    m_locations = locations;
}

void EditorData::updateModel()
{
    if (m_scope == Body)
        m_model->setReportData(m_reportData);
    else if (m_scope == Header)
        m_model->setReportData(m_reportData->headerMap().value(m_locations));
    else if (m_scope == Footer)
        m_model->setReportData(m_reportData->footerMap().value(m_locations));
}
