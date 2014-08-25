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

#include "ObjectData.h"
#include "HeaderData.h"
#include "ReportTextEdit.h"
#include "EditorData.h"
#include "structure/ParagraphData.h"
#include "structure/VSpaceData.h"
#include "structure/HLineElementData.h"
#include "structure/TextElementData.h"
#include "structure/ReportData.h"
#include "structure/CellData.h"
#include <QDebug>

ObjectData::ObjectData(ObjectData *parent)
    : QObject(parent)
    , m_updateEnabled(true)
    , m_onDocument(true)
    , m_parentElement(parent)
{
    if (m_parentElement)
        m_parentElement->m_childrenElements.append(this);
}

ObjectData::~ObjectData()
{
    setParentElement(0);

    // qDeleteAll behavior changed in Qt 4.8, do not use here
    while (!m_childrenElements.isEmpty()) {
      delete m_childrenElements.takeFirst();
    }
}

int ObjectData::startPosition() const
{
    // start position is end position of the previous element
    if (previousChild())
        return previousChild()->endPosition();
    if (parentElement())
        return parentElement()->startPosition();
    return 0; //ReportData
}

void ObjectData::update()
{
    if (m_updateEnabled) {
        doUpdate();
        emit updated();
    }
}

void ObjectData::doUpdate()
{
    doRemove();
    doAddToReport();
}

KDReports::TextDocumentData& ObjectData::textDocumentData() const
{
    return topContainer()->textDocumentData();
}

ObjectData* ObjectData::topContainer() const
{
    return parentElement();
}

void ObjectData::remove()
{
    doRemove();
}

void ObjectData::addToReport()
{
    doAddToReport();
    emit updated();
}

const QList<ObjectData*>& ObjectData::childrenElements() const
{
    return m_childrenElements;
}

ObjectData * ObjectData::parentElement() const
{
    return m_parentElement;
}

void ObjectData::setParentElement(ObjectData* parent, int position)
{
    if (!parent) {
        if (m_parentElement) {
            m_parentElement->m_childrenElements.removeAll(this);
            m_parentElement = parent;
        }
        return;
    }

    if (m_parentElement == parent) {
        if (position == parent->childrenElements().indexOf(this) || position < 0) {
            return;
        } else {
            parent->m_childrenElements.move(childPosition(), position);
        }
    } else {
        if (m_parentElement)
            m_parentElement->m_childrenElements.removeAll(this);
        m_parentElement = parent;
        if (position < 0)
            m_parentElement->m_childrenElements.append(this);
        else
            m_parentElement->m_childrenElements.insert(position, this);
    }
}

QRect ObjectData::elementRect(ReportTextEdit *reportTextEdit)
{
    return calculateRect(reportTextEdit, startPosition(), endPosition());
}

QRect ObjectData::calculateRect(ReportTextEdit *editor, int firstCursorPosition, int lastCursorPosition)
{
    QTextCursor cursor(editor->document());
    int xTopLeft = -1;
    int yTopLeft = -1;
    int xBottomRight = -1;
    int yBottomRight = -1;

    for (int i = firstCursorPosition; i <= lastCursorPosition; i++) {
        cursor.setPosition(i);
        QRect r(editor->cursorRect(cursor));
        if (xTopLeft != -1)
            xTopLeft = qMin(r.topLeft().x(), xTopLeft);
        else
            xTopLeft = r.topLeft().x();

        if (yTopLeft != -1)
            yTopLeft = qMin(r.topLeft().y(), yTopLeft);
        else
            yTopLeft = r.topLeft().y();

        if (xBottomRight != -1)
            xBottomRight = qMax(r.bottomRight().x(), xBottomRight);
        else
            xBottomRight = r.bottomRight().x();

        if (yBottomRight != -1)
            yBottomRight = qMax(r.bottomRight().y(), yBottomRight);
        else
            yBottomRight = r.bottomRight().y();
    }
    return QRect(QPoint(xTopLeft, yTopLeft), QPoint(xBottomRight, yBottomRight));
}

void ObjectData::doRemove()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.beginEditBlock();
    int start = startPosition();
    if (qobject_cast<TopElement*>(this) && nextChild() == 0 && previousChild() != 0)
        start = qMax(start - 1, 0);
    cursor.setPosition(start);
    QTextBlockFormat f = cursor.blockFormat();
    cursor.setPosition( endPosition(), QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    cursor.setBlockFormat(f);
    cursor.endEditBlock();
    m_onDocument = false;
}

void ObjectData::doAddToReport()
{
}

bool ObjectData::supportParagraph() const
{
    return false;
}

bool ObjectData::insertText(EditorData* editorData, const QString &text, int cursorPosition)
{
    Q_UNUSED(editorData);
    Q_UNUSED(text);
    Q_UNUSED(cursorPosition);
    return false;
}

void ObjectData::addToHierarchy(ObjectData *parent, int cursorPosition, EditorData * editor)
{
    Q_UNUSED(cursorPosition)
    int pos = 0;
    ObjectData *p = editor->reportData();
    if (parent == p)
        pos = p->childrenElements().length();
    else {
        ObjectData* o = parent;
        while (o->parentElement() != p)
            o = o->parentElement();
        if (parent->startPosition() == cursorPosition) {
            pos = o->childPosition();
        } else if (parent->endPosition() == cursorPosition) {
            pos = o->childPosition() + 1;
        } else {
            if (TextElementData *textData1 = qobject_cast<TextElementData*>(parent)) {
                textData1->splitText(cursorPosition, editor);
                ParagraphData* oldParagraph = qobject_cast<ParagraphData*>(textData1->parentElement());
                oldParagraph->splitParagraph(textData1->childPosition() + 1, editor);
                editor->insertSubElement(oldParagraph->parentElement(), this, oldParagraph->childPosition() + 1);
            }
            return;
        }
    }
     if (qobject_cast<ParagraphData*>(parent) && parent->childrenElements().isEmpty()) {
         p = parent;
     }
     editor->insertSubElement(p, this, pos);
}

void ObjectData::setUpdateEnabled(bool enable)
{
    m_updateEnabled = enable;
}

ObjectData* ObjectData::previousChild() const
{
    int pos = childPosition();
    if (pos > 0)
        return parentElement()->childrenElements().at(pos - 1);
    return 0;
}


ObjectData* ObjectData::nextChild() const
{
    int pos = childPosition();
    QList<ObjectData*> childrenList(parentElement()->childrenElements());
    if (pos < childrenList.size() - 1)
        return childrenList.at(pos + 1);
    return 0;
}


int ObjectData::childPosition() const
{
    if (parentElement())
        return parentElement()->childrenElements().indexOf(const_cast<ObjectData*>(this));
    return -1;
}

bool ObjectData::isOnDocument() const
{
    return m_onDocument;
}

void ObjectData::validate()
{
}

QString ObjectData::translatePropertyName(const QString &name) const
{
    return name;
}

QVariant ObjectData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    Q_UNUSED(name);
    Q_UNUSED(type);
    return QVariant();
}

QString ObjectData::translatedEnumValue(const QMetaEnum &metaEnum, int key) const
{
    Q_UNUSED(metaEnum);
    Q_UNUSED(key);
    return QString();
}
