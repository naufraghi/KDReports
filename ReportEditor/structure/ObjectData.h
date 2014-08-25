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

#ifndef OBJECTDATA_H
#define OBJECTDATA_H

#include <QObject>
#include <QXmlStreamWriter>
#include <KDReportsTextDocumentData_p.h>
#include "editor_export.h"
#include <QMetaEnum>

class ReportTextEdit;

class EDITOR_EXPORT ObjectData : public QObject
{
    Q_OBJECT
public:
    enum ValueType {MIN, MAX};
    explicit ObjectData(ObjectData *parent = 0);
    virtual ~ObjectData();
    virtual void writeXml(QXmlStreamWriter& writer) = 0;
    virtual int startPosition() const;
    virtual int endPosition() const = 0;
    void update();
    void remove();
    void addToReport();

    virtual KDReports::TextDocumentData& textDocumentData() const;
    /**
      * \return the top element (header / footer / report)
      */
    virtual ObjectData* topContainer() const;
    const QList<ObjectData*>& childrenElements() const;
    ObjectData* parentElement() const;
    void setParentElement(ObjectData *parent, int position = -1);
    virtual QRect elementRect(ReportTextEdit *reportTextEdit);
    virtual void doUpdate();
    virtual void doAddToReport();
    virtual void doRemove();
    virtual bool supportParagraph() const;
    virtual bool insertText(EditorData* editorData, const QString &text, int cursorPosition);
    virtual void addToHierarchy(ObjectData* parent, int cursorPosition, EditorData * editor);
    void setUpdateEnabled(bool enable);
    ObjectData * nextChild() const;
    ObjectData * previousChild() const;
    int childPosition() const;
    bool isOnDocument() const;
    virtual void validate();
    virtual QString translatePropertyName(const QString &name) const;
    virtual QVariant propertyValue(const QString &name, ValueType type) const;
    virtual QString translatedEnumValue(const QMetaEnum &metaEnum, int key) const;

protected:
    QRect calculateRect(ReportTextEdit *editor, int firstCursorPosition, int lastCursorPosition);
    bool m_updateEnabled;
    bool m_onDocument;

signals:
    void updated();

private:
    ObjectData* m_parentElement;
    QList<ObjectData*> m_childrenElements;
};

#endif // OBJECTDATA_H
