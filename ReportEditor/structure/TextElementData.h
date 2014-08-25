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

#ifndef TEXTELEMENTDATA_H
#define TEXTELEMENTDATA_H

#include "KDReportsTextElement.h"
#include "ParagraphChild.h"
#include "editor_export.h"

class EDITOR_EXPORT TextElementData : public ParagraphChild
{
    Q_OBJECT
    Q_PROPERTY (QString text READ text WRITE setText)
    Q_PROPERTY (QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY (QFont font READ font WRITE setFont)
    // Only background in editor, no bgred, bgblue, bggreen
    Q_PROPERTY (QBrush background READ background WRITE setBackground)
    Q_PROPERTY (QColor textColor READ textColor WRITE setTextColor)
    //model property
    Q_PROPERTY (QString modelKey READ modelKey WRITE setModelKey NOTIFY modelKeyChanged)
    Q_PROPERTY (int modelRow READ modelRow WRITE setModelRow)
    Q_PROPERTY (int modelColumn READ modelColumn WRITE setModelColumn)

public:
    struct ModelData {
        ModelData() : modelKey(QString())
          , modelRow(0)
          , hasModelRow(false)
          , modelColumn(0)
          , hasModelColumn(false)
        {}

        QString modelKey;
        int modelRow;
        bool hasModelRow;
        int modelColumn;
        bool hasModelColumn;
    };

    explicit TextElementData(const KDReports::TextElement &textElement, bool fontModified = false, ModelData modelData = ModelData());
    explicit TextElementData(const QString &text);
    QString id() const;
    void setId(const QString &id);
    QString text() const;
    void setText(const QString &text);
    QFont font() const;
    void setFont(const QFont& font);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    QColor textColor() const;
    void setTextColor(const QColor& color);
    QString modelKey() const;
    void setModelKey(const QString& modelKey);
    int modelRow() const;
    void setModelRow(int modelRow);
    int modelColumn() const;
    void setModelColumn(int modelColumn);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual void doAddToReport();
    virtual int elementSize() const;
    virtual bool insertText(EditorData* editorData, const QString &text, int cursorPosition);
    virtual void addToHierarchy(ObjectData *parent, int cursorPosition, EditorData *editor);
    void splitText(int cursorPosition, EditorData *editor);
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;

signals:
    void idChanged(const QString &newId, const QString &oldId);
    void modelKeyChanged(const QString &newModelKey, const QString &oldModelKey);

private:
    KDReports::TextElement m_textElement;
    int m_oldSize;
    bool m_fontModified;
    ModelData m_modelData;
};

#endif // TEXTELEMENTDATA_H
