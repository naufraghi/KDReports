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

#ifndef REPORTTEXTEDIT_H
#define REPORTTEXTEDIT_H

#include <QTextEdit>
#include "editor_export.h"

class EditorData;
class ObjectData;

enum HandlePosition{Right, Bottom, None};
typedef QPair<QPoint, HandlePosition> Handle;

class EDITOR_EXPORT ReportTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit ReportTextEdit(EditorData* editorData, QWidget *parent = 0);
    void objectAtCursorPosition(ObjectData *top, int position, QList<ObjectData *> &list);
    void cleanSelection();
    void deleteSelection();
    void changeCursorPosition(int position, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
    bool cut();
    bool paste();
    bool copy();
protected:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual bool event(QEvent *e);
public slots:
    void updateSelectedItem();
protected slots:
    void slotCursorPositionChanged();

private:
    ObjectData * currentObject();
    bool insertText(const QString &text);
    void newParagraph();
    void newLine();
    bool backspace();
    void keyDelete();
    EditorData* m_editorData;
    ObjectData* m_currentObject;
    ObjectData* m_currentSelectedObject;
    QList<Handle> m_handle;
    HandlePosition m_handlePosition;
    bool m_onResize;
    QRect m_resizeRect;
};

#endif // REPORTTEXTEDIT_H
