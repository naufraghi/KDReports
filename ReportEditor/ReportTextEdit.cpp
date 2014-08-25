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

#include "ReportTextEdit.h"

#include "EditorData.h"
#include "structure/ReportData.h"
#include "structure/ImageElementData.h"
#include "structure/CellData.h"
#include "structure/TableElementData.h"
#include "structure/TextElementData.h"
#include "structure/HLineElementData.h"
#include "structure/ParagraphData.h"
#include "structure/VSpaceData.h"
#include "ReportStructureModel.h"

#include <QPainter>
#include <QList>
#include <QMimeData>
#include <QMouseEvent>
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QTextDocumentFragment>

ReportTextEdit::ReportTextEdit(EditorData *editorData, QWidget *parent)
    : QTextEdit(parent)
    , m_editorData(editorData)
    , m_currentObject(0)
    , m_currentSelectedObject(0)
    , m_handlePosition(None)
    , m_onResize(false)
{
    setMouseTracking(true);
    setUndoRedoEnabled(false);
    connect(m_editorData, SIGNAL(indexChanged(QModelIndex)), SLOT(updateSelectedItem()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
}

void ReportTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_onResize) {
        QTextEdit::mouseMoveEvent(e);
        if (!m_editorData->topHierarchyElement()) {
            return;
        }
        QTextCursor c(cursorForPosition(e->pos()));
        QList<ObjectData*> list;
        objectAtCursorPosition(m_editorData->topHierarchyElement(), c.position(), list);
        ObjectData* current = 0;
        foreach (ObjectData* item, list) {
            QRect r(item->elementRect(this));
            if (r.contains(e->pos()))
                current = item;
        }
        if (current != m_currentObject) {
            m_currentObject = current;
        }
        QRect r(e->pos().x() - 3, e->pos().y() - 3, 6, 6);
        m_handlePosition = None;
        foreach (Handle h, m_handle) {
            if (r.contains(h.first)) {
                m_handlePosition = h.second;
                break;
            }
        }
        switch (m_handlePosition) {
        case Bottom:
            viewport()->setCursor(Qt::SizeVerCursor);
            break;
        case Right:
            viewport()->setCursor(Qt::SizeHorCursor);
            break;
       default:
            viewport()->setCursor(Qt::IBeamCursor);
            break;
        }
    } else {
        if (m_currentSelectedObject && qobject_cast<ImageElementData*>(m_currentSelectedObject)) {
            QRect r = m_currentSelectedObject->elementRect(this);
            m_resizeRect = r;
            int x = e->pos().x();
            int y = e->pos().y();
            switch (m_handlePosition) {
            case Bottom:
                if (y > m_resizeRect.top())
                    m_resizeRect.setBottom(y);
                else
                    m_resizeRect.setBottom(m_resizeRect.top() + 1);
                m_resizeRect.setWidth((y - r.y()) / (r.height()*1.0) * r.width());
                break;
            case Right:
                if (x > m_resizeRect.left())
                    m_resizeRect.setRight(x);
                else
                    m_resizeRect.setRight(m_resizeRect.left() + 1);
                m_resizeRect.setHeight((x - r.x()) / (r.width()*1.0) * r.height());
                break;
            default:
                break;
            }
        }
    }
    viewport()->update();
}

void ReportTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!m_editorData->topHierarchyElement())
        QTextEdit::mouseDoubleClickEvent(e);
    QTextCursor c(cursorForPosition(e->pos()));
    QList<ObjectData*> list;
    objectAtCursorPosition(m_editorData->topHierarchyElement(), c.position(), list);
    ObjectData* current = 0;
    foreach (ObjectData* item, list) {
        QRect r(item->elementRect(this));
        if (r.contains(e->pos()))
            current = item;
    }
    if (current != m_currentSelectedObject) {
       if (current) {
            m_editorData->changeObject(current);
       } else {
           m_editorData->changeObject(m_editorData->topHierarchyElement());
       }
    }
    QTextEdit::mouseDoubleClickEvent(e);
    viewport()->update();
}

void ReportTextEdit::mousePressEvent(QMouseEvent *e)
{
    if (m_handlePosition != None) {
        m_onResize = true;
    }
    QTextEdit::mousePressEvent(e);
}

void ReportTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_currentSelectedObject && qobject_cast<ImageElementData*>(m_currentSelectedObject)) {
        qreal width = m_currentSelectedObject->property("width").toDouble();
        qreal height = m_currentSelectedObject->property("height").toDouble();
        QRect r = m_currentSelectedObject->elementRect(this);
        qreal ratio = m_resizeRect.width() / (r.width()*1.0);
        if (width == 0)
            width = height * (r.width() / (r.height() * 1.0));
        else if (height == 0)
            height = width * (r.height() / (r.width() * 1.0));
        switch (m_handlePosition) {
        case Right:
            m_editorData->changeProperty(m_currentSelectedObject, "width", ratio * width);
            break;
        case Bottom:
            m_editorData->changeProperty(m_currentSelectedObject, "height", ratio * height);
            break;
        default:
            break;
        }
    }

    m_onResize = false;
    QTextEdit::mouseReleaseEvent(e);
}

void ReportTextEdit::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
    QPainter painter(this->viewport());

    m_handle.clear();
    // paint image selection rect
    if (m_currentSelectedObject && qobject_cast<ImageElementData*>(m_currentSelectedObject)) {
        painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QRect imageRect;
        if (m_onResize) {
            painter.setBrush(QBrush(QColor(0, 0, 255, 30), Qt::SolidPattern));
            imageRect = m_resizeRect;
        }
        else {
            painter.setBrush(QBrush());
            imageRect = m_currentSelectedObject->elementRect(this);
        }
        painter.drawRect(imageRect);
        // Draw image handle
        painter.setPen(QPen(Qt::blue, 6, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
        m_handle.append(qMakePair(QPoint((imageRect.right() + imageRect.left()) / 2, imageRect.bottom()), Bottom));
        m_handle.append(qMakePair(QPoint(imageRect.right(), (imageRect.bottom() + imageRect.top()) / 2), Right));
        foreach (Handle h, m_handle) {
            painter.drawPoint(h.first);
        }
    }
}

void ReportTextEdit::objectAtCursorPosition(ObjectData *top, int position, QList<ObjectData*> &list)
{
    if (!top || top->startPosition() > position)
        return;
    foreach(ObjectData *child, top->childrenElements()) {
        if (child->startPosition() <= position && child->endPosition() >= position) {
            // Need to take the next cell
            if (!(qobject_cast<CellData*>(child) && child->endPosition() == position)) {
                list.append(child);
            }
            objectAtCursorPosition(child, position, list);
        }
    }
}


void ReportTextEdit::updateSelectedItem()
{
    if (m_currentSelectedObject == m_editorData->selectedObject())
        return;

    m_currentSelectedObject = m_editorData->selectedObject();
    viewport()->update();
}

void ReportTextEdit::newLine()
{
    QList<ObjectData*> list;
    int cursorPosition = textCursor().position();
    objectAtCursorPosition(m_editorData->topHierarchyElement(), cursorPosition, list);
    ObjectData* current = 0;
    if(!list.isEmpty())
        current = list.last();
    else
        return;

    if (TextElementData* textElementData = qobject_cast<TextElementData*>(current)) {
        int pos = cursorPosition - current->startPosition();
        QString newText(textElementData->text());
        newText.insert(pos, QChar(10));
        m_editorData->changeProperty(textElementData, "text", newText);
        changeCursorPosition(cursorPosition + 1);
    }
}

ObjectData* ReportTextEdit::currentObject()
{
    QList<ObjectData*> list;
    int cursorPosition = textCursor().position();
    objectAtCursorPosition(m_editorData->topHierarchyElement(), cursorPosition, list);
    ObjectData* current = m_editorData->topHierarchyElement();
    if(!list.isEmpty())
        current = list.last();
    return current;
}

void ReportTextEdit::newParagraph()
{
    if (ObjectData* current = currentObject()) {
        int cursorPosition = textCursor().position();
        int position = 0;
        int start = current->startPosition();

        if (cursorPosition < current->endPosition() && cursorPosition > start) {
            if (TextElementData* text = qobject_cast<TextElementData*>(current)) {
                text->splitText(cursorPosition, m_editorData);
                ParagraphData* oldParagraph = qobject_cast<ParagraphData*>(current->parentElement());
                oldParagraph->splitParagraph(text->childPosition() + 1, m_editorData);
            }
        } else {

            if (current->supportParagraph()) {
                position = current->childPosition();
                if (current->startPosition() < cursorPosition)
                    position++;
                //TODO manage if the cursor position is between start and end position
            } else {
                while (true) {
                    ObjectData *parent = current->parentElement();
                    if (parent->supportParagraph()) {
                        position = current->childPosition();
                        start = current->startPosition();
                        if (start < cursorPosition)
                            position++;
                        current = parent;
                        break;
                    }
                    current = parent;
                }

            }
            ParagraphData *p = new ParagraphData(0);
            m_editorData->insertSubElement(current, p, position);
        }
        changeCursorPosition(cursorPosition + 1);
    }
}

bool ReportTextEdit::backspace()
{
    int cursorPosition = textCursor().position();

    if (ObjectData* current = currentObject()) {
        int pos = cursorPosition - current->startPosition();
        if (qobject_cast<ParagraphChild*>(current)) {
            if (cursorPosition > 0 && pos == 0) {
                ObjectData* parent = current->parentElement();
                QList<ObjectData*> list(parent->childrenElements());
                if (list.first() == current) {
                    ObjectData* newParent = parent->previousChild();
                    if (qobject_cast<ParagraphData*>(newParent)) {
                        m_editorData->beginCommand();
                        foreach (ObjectData* o, list) {
                            m_editorData->moveElement(o, newParent, newParent->childrenElements().size());
                        }
                        m_editorData->removeObject(parent);
                        m_editorData->endCommand();
                        changeCursorPosition(cursorPosition - 1);
                        return true;
                    }
                } else {
                    // Apply backspace at end position of the previous item
                    current = current->previousChild();
                    pos = cursorPosition - current->endPosition();
                    if (ParagraphChild* child = qobject_cast<ParagraphChild*>(current)) {
                        if (child->elementSize() == 1) {
                            m_editorData->removeObject(child);
                            return true;
                        }
                    }
                }
            }
            if (TextElementData* textElementData = qobject_cast<TextElementData*>(current)) {
                if (cursorPosition != 0) {
                    QString id(textElementData->id());
                    if (id.isEmpty()) {
                        QString newText(textElementData->text());
                        newText.remove(pos - 1, 1);
                        m_editorData->changeProperty(textElementData, "text", newText, 2);
                        changeCursorPosition(cursorPosition - 1);
                        return true;
                    } else {
                        if (pos == 0 || pos == id.length()) {
                            return false;
                        } else {
                            id.remove(pos - 2, 1);
                            m_editorData->changeProperty(textElementData, "id", id);
                            if (!id.isEmpty())
                                changeCursorPosition(cursorPosition - 1);
                            else
                                changeCursorPosition(textElementData->startPosition());
                            return true;
                        }
                    }
                }
            }
        } else if (qobject_cast<ParagraphData*>(current) && current->childrenElements().isEmpty()) {
                m_editorData->removeObject(current);
                return true;
        } else if (qobject_cast<HLineElementData*>(current)) {
            m_editorData->removeObject(current);
            return true;
        }
    }
    return false;
}

void ReportTextEdit::keyDelete()
{
    QTextCursor cursor(textCursor());
    QTextCursor cursorEnd(textCursor());
    cursorEnd.movePosition(QTextCursor::End);
    if (cursor.position() == cursorEnd.position())
        return;
    moveCursor(QTextCursor::NextCharacter);
    if (!backspace()) {
        moveCursor(QTextCursor::PreviousCharacter);
    }
    return;
}

void ReportTextEdit::deleteSelection()
{
    QTextCursor cursor(textCursor());
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    Q_ASSERT(start != end);
    QList<ObjectData*> listStart;
    QList<ObjectData*> listEnd;
    objectAtCursorPosition(m_editorData->topHierarchyElement(), start, listStart);
    ObjectData* startObject = listStart.last();
    objectAtCursorPosition(m_editorData->topHierarchyElement(), end - 1, listEnd);
    ObjectData* endObject = listEnd.last();
    if (startObject == endObject) {
        if (startObject->startPosition() == start && startObject->endPosition() == end) {
            m_editorData->removeObject(startObject);
            return;
        }
        if (TextElementData* text = qobject_cast<TextElementData*>(startObject)) {
            QString id(text->id());
            int startPosition = start - text->startPosition();
            int length = end - start;
            if (id.isEmpty()) {
                QString newText(text->text());
                newText.remove(startPosition, length);
                m_editorData->changeProperty(text, "text", newText);
            } else {
                if (startPosition == 1 && end == text->endPosition() - 1) {
                    if (text->text().isEmpty())
                        m_editorData->removeObject(text);
                    else {
                        m_editorData->changeProperty(text, "id", QString());
                    }
                    changeCursorPosition(start - 1);
                    return;
                } else if (startPosition == 0  || end == text->endPosition()) {
                    return;
                } else {
                    id.remove(startPosition - 1, length);
                    m_editorData->changeProperty(text, "id", id);
                    changeCursorPosition(start);
                    return;
                }
            }
        }
    } else {
        // build ancestor list
        QList<ObjectData*> startObjectAncestor;
        ObjectData *parent = startObject;
        while (parent) {
            startObjectAncestor.prepend(parent);
            parent = parent->parentElement();
        }

        QList<ObjectData*> endObjectAncestor;
        parent = endObject;
        while (parent) {
            endObjectAncestor.prepend(parent);
            parent = parent->parentElement();
        }

        // Find common ancestor
        int sizeEnd = endObjectAncestor.size();
        int sizeStart = startObjectAncestor.size();
        ObjectData *commonAncestor = 0;
        int ancestorPosition = -1;
        for (int i = 0; i < qMin(sizeEnd, sizeStart); i++) {
            ObjectData* oStart = startObjectAncestor.at(i);
            ObjectData* oEnd = endObjectAncestor.at(i);
            if (oStart == oEnd) {
                commonAncestor = oStart;
                ancestorPosition = i;
            } else {
                break;
            }
        }

        if (commonAncestor) {
            if (commonAncestor->startPosition() == start && commonAncestor->endPosition() == end) {
                m_editorData->beginCommand();
                if (qobject_cast<ReportData*>(commonAncestor) || qobject_cast<HeaderData*>(commonAncestor)) {
                    foreach (ObjectData* o, commonAncestor->childrenElements())
                        m_editorData->removeObject(o);
                } else {
                    m_editorData->removeObject(commonAncestor);
                }
                m_editorData->endCommand();
            } else {
                int currentPos = ancestorPosition + 1;
                startObject = startObjectAncestor.at(currentPos);
                endObject = endObjectAncestor.at(currentPos);

                m_editorData->beginCommand();
                // Remove object between the startObject and endObject
                ObjectData *o = startObject->nextChild();
                QList<ObjectData*> listToRemove;
                while (o != endObject) {
                    ObjectData* tmp = o->nextChild();
                    m_editorData->removeObject(o);
                    o = tmp;
                }

                // Remove object after the startObject
                for (int i = currentPos + 1; i < startObjectAncestor.size(); i++) {
                    startObject = startObjectAncestor.at(i);
                    o = startObject->nextChild();
                    while (o) {
                        ObjectData* tmp = o->nextChild();
                        m_editorData->removeObject(o);
                        o = tmp;
                    }
                }

                // Remove object before the endObject
                for (int i = currentPos + 1; i < endObjectAncestor.size(); i++) {
                    endObject = endObjectAncestor.at(i);
                    o = endObject->previousChild();
                    while (o) {
                        ObjectData* tmp = o->previousChild();
                        m_editorData->removeObject(o);
                        o = tmp;
                    }
                }

                QList<ObjectData*> listToMove;
                ObjectData* moveParent = 0;
                int positionToMove;
                // Check if we need to move end object
                if(startObject->parentElement() != endObject->parentElement()) {
                    if (qobject_cast<ParagraphData*>(startObject->parentElement())) {
                        if (qobject_cast<ParagraphData*>(endObject->parentElement())) {
                            moveParent = startObject->parentElement();
                            positionToMove = startObject->childPosition() + 1;
                            ObjectData* o = endObject;
                            while(o) {
                                ObjectData* tmp = o->nextChild();
                                m_editorData->moveElement(o, moveParent, positionToMove);
                                o = tmp;
                            }
                            m_editorData->removeObject(endObject->parentElement());
                        }
                    }
                }

                // Check if we need to remove start object
                if (startObject->startPosition() == start) {
                    end = end - (startObject->endPosition() - startObject->startPosition());
                    positionToMove--;
                    m_editorData->removeObject(startObject);
                } else {
                    if (TextElementData* text = qobject_cast<TextElementData*>(startObject)) {
                        QString newText(text->text());
                        int startText = start - text->startPosition();
                        int length = text->endPosition() - start;
                        newText.remove(startText, length);
                        m_editorData->changeProperty(text, "text", newText);
                    }
                }

                // Check if we need to remove end object
                if (endObject->endPosition() == end) {
                    listToMove.removeAt(0);
                    m_editorData->removeObject(endObject);
                } else {
                    if (TextElementData* text = qobject_cast<TextElementData*>(endObject)) {
                        QString newText(text->text());
                        int length = end - endObject->startPosition();
                        newText.remove(0, length);
                        m_editorData->changeProperty(text, "text", newText);
                    }
                }

                m_editorData->endCommand();
                changeCursorPosition(start);
            }
        }
    }
}

bool ReportTextEdit::insertText(const QString &text)
{
    ObjectData *current = currentObject();
    int cursorPosition = textCursor().position();
    if (!current)
        current = m_editorData->topHierarchyElement();
    bool result = current->insertText(m_editorData, text, cursorPosition);
    if (!result)
        qWarning() << "write text on this object" << current << "not yet supported";
    return result;
}

void ReportTextEdit::keyPressEvent(QKeyEvent *e)
{
    QTextCursor cursor(textCursor());
    bool selection = cursor.selectionStart() != cursor.selectionEnd();
    if (!m_editorData->topHierarchyElement())
        return;
    int cursorPosition = cursor.position();
    ObjectData *current = currentObject();
    switch(e->key()) {
    case Qt::Key_Backspace:
        if (!selection)
            backspace();
        else
            deleteSelection();
        e->accept();
        return;
    case Qt::Key_Delete:
        if (!selection)
            keyDelete();
        else
            deleteSelection();
        e->accept();
        return;
    case Qt::Key_Return:
        if (e->modifiers() == Qt::ShiftModifier) {
            newLine();
            e->accept();
            return;
        } else {
            newParagraph();
            e->accept();
            return;
        }
    case Qt::Key_V:
        if (e->modifiers() == Qt::ControlModifier) {
            paste();
            e->accept();
            return;
        }
    case Qt::Key_C:
        if (e->modifiers() == Qt::ControlModifier) {
            copy();
            e->accept();
            return;
        }
    case Qt::Key_X:
        if (e->modifiers() == Qt::ControlModifier) {
            cut();
            e->accept();
            return;
        }
    }
    if (!current)
        current = m_editorData->topHierarchyElement();
    if (!e->text().isEmpty() && (e->text()[0].isPrint() || e->text()[0].isSpace())) {
        int start = current->startPosition();
        int nbChilds = 0;
        if (qobject_cast<ParagraphChild*>(current))
            nbChilds = current->parentElement()->childrenElements().size();
        if (insertText(e->text())) {
            if (qobject_cast<ParagraphChild*>(current) && current->parentElement()->childrenElements().size() == nbChilds) {
                QTextEdit::keyPressEvent(e);
                return;
            }
            int p = cursorPosition;
            if (qobject_cast<HLineElementData*>(current) && start != cursorPosition)
                p = current->endPosition();
            if (qobject_cast<VSpaceData*>(current))
                p = current->endPosition();
            changeCursorPosition(p + e->text().size());
            e->accept();
        } else {
            qDebug() << "Problem to insert text here" << current << cursorPosition;
        }
    } else {
        QTextEdit::keyPressEvent(e);
    }
}

bool ReportTextEdit::copy()
{
    QTextCursor cursor(textCursor());
    int startPosition = cursor.selectionStart();
    int endPosition = cursor.selectionEnd();
    if (startPosition == endPosition)
        return false;

    QList<ObjectData*> list;
    ObjectData* startObject = 0;
    objectAtCursorPosition(m_editorData->topHierarchyElement(), startPosition, list);
    if (!list.isEmpty())
        startObject = list.last();
    ObjectData* endObject = 0;
    list.clear();
    objectAtCursorPosition(m_editorData->topHierarchyElement(), endPosition, list);
    if (!list.isEmpty())
        endObject = list.last();
    if (startObject == endObject) {
        if (qobject_cast<TextElementData*>(startObject)) {
            QApplication::clipboard()->setText(cursor.selectedText());
            return true;
        }
    }
    return false;

}

bool ReportTextEdit::paste()
{
    // TODO manage to paste over a selection
    deleteSelection();
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (mimeData) {
        // paste text has plain text
        if (mimeData->hasText()) {
            if (insertText(mimeData->text())) {
                QTextCursor cursor(textCursor());
                cursor.insertText(mimeData->text());
                return true;
            }
        }
    }
    return false;
}

bool ReportTextEdit::cut()
{
    if (copy()) {
        deleteSelection();
        return true;
    }
    return false;
}

void ReportTextEdit::cleanSelection()
{
    m_currentObject = 0;
    m_currentSelectedObject = 0;
}

void ReportTextEdit::slotCursorPositionChanged()
{
    ObjectData* object = currentObject();
    if (object != m_editorData->selectedObject() && m_editorData->canChangeObject())
        m_editorData->changeObject(object);
}

void ReportTextEdit::focusInEvent(QFocusEvent *e)
{
    slotCursorPositionChanged();
    QTextEdit::focusInEvent(e);
}

void ReportTextEdit::changeCursorPosition(int position, QTextCursor::MoveMode mode)
{
    QTextCursor c = textCursor();
    c.setPosition(position, mode);
    setTextCursor(c);
}

bool ReportTextEdit::event(QEvent *e)
{
    if (QKeyEvent* k = dynamic_cast<QKeyEvent*>(e))
        if (k == QKeySequence::Undo || k == QKeySequence::Redo)
            return false;
    return QTextEdit::event(e);
}
