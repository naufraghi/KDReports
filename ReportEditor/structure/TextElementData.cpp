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

#include "TextElementData.h"
#include "ParagraphData.h"
#include "HLineElementData.h"
#include "../EditorData.h"
#include <QTextCursor>
#include <QTextDocument>
#include <KDReportsReportBuilder_p.h>
#include <QTextBlock>

TextElementData::TextElementData(const KDReports::TextElement &textElement, bool fontModified, ModelData modelData)
    : ParagraphChild()
    , m_textElement(textElement)
    , m_oldSize(textElement.text().size())
    , m_fontModified(fontModified)
    , m_modelData(modelData)
{
    setObjectName(tr("text"));
}

TextElementData::TextElementData(const QString &text)
    : ParagraphChild()
    , m_textElement(KDReports::TextElement(text))
    , m_oldSize(text.size())
    , m_fontModified(false)
{
    setObjectName(tr("text"));
}

QFont TextElementData::font() const
{
    return m_textElement.font();
}

void TextElementData::setFont(const QFont& font)
{
    m_fontModified = true;
    m_textElement.setFont(font);
}

QString TextElementData::id() const
{
    return m_textElement.id();
}

void TextElementData::setId(const QString &id)
{
    QString oldId = m_textElement.id();
    m_textElement.setId(id);
    emit idChanged(id, oldId);
}

QString TextElementData::text() const
{
    return m_textElement.text();
}

void TextElementData::setText(const QString &newText)
{
    m_textElement.setText(newText);
}

QBrush TextElementData::background() const
{
    return m_textElement.background();
}

void TextElementData::setBackground(const QBrush &brush)
{
    m_textElement.setBackground(brush);
}

QColor TextElementData::textColor() const
{
    return m_textElement.textColor();
}

void TextElementData::setTextColor(const QColor &color)
{
    m_textElement.setTextColor(color);
}

QString TextElementData::modelKey() const
{
    return m_modelData.modelKey;
}

void TextElementData::setModelKey(const QString &modelKey)
{
    const QString oldKey = m_modelData.modelKey;
    m_modelData.modelKey = modelKey;
    emit modelKeyChanged(m_modelData.modelKey, oldKey);
}

int TextElementData::modelRow() const
{
    return m_modelData.modelRow;
}

void TextElementData::setModelRow(int modelRow)
{
    m_modelData.hasModelRow = true;
    m_modelData.modelRow = modelRow;
}

int TextElementData::modelColumn() const
{
    return m_modelData.modelColumn;
}

void TextElementData::setModelColumn(int modelColumn)
{
    m_modelData.hasModelColumn = true;
    m_modelData.modelColumn = modelColumn;
}

void TextElementData::writeXml(QXmlStreamWriter &writer)
{
    QString trueValue(QString::fromLatin1("true"));
    writer.writeStartElement(QString::fromLatin1("text"));
    if(!id().isEmpty())
        writer.writeAttribute(QString::fromLatin1("id"), id());
    if(!modelKey().isEmpty()) {
        writer.writeAttribute(QString::fromLatin1("model"), modelKey());
        if (m_modelData.hasModelRow)
            writer.writeAttribute(QString::fromLatin1("row"), QString::number(modelRow()));
        if (m_modelData.hasModelColumn)
            writer.writeAttribute(QString::fromLatin1("column"), QString::number(modelColumn()));
    }
    if (background().style()!=Qt::NoBrush && background().color().isValid())
        writer.writeAttribute(QString::fromLatin1("background"), background().color().name());
    if (m_fontModified) {
        writer.writeAttribute(QString::fromLatin1("font"), font().family());
        writer.writeAttribute(QString::fromLatin1("pointsize"), QString::number(font().pointSizeF()));
    }
    if (textColor().isValid())
        writer.writeAttribute(QString::fromLatin1("color"), textColor().name());
    if(font().weight() == QFont::Bold)
        writer.writeAttribute(QString::fromLatin1("bold"), trueValue);
    if(font().italic())
        writer.writeAttribute(QString::fromLatin1("italic"), trueValue);
    if(font().strikeOut())
        writer.writeAttribute(QString::fromLatin1("strikeout"), trueValue);
    if(font().underline())
        writer.writeAttribute(QString::fromLatin1("underline"), trueValue);
    if(isInline())
        writer.writeAttribute(QString::fromLatin1("inline"), trueValue);
    else
        writer.writeAttribute(QString::fromLatin1("alignment"), alignment());
    if (paragraphBackground().isValid())
        writer.writeAttribute(QString::fromLatin1("paragraph-background"), paragraphBackground().name());
    writer.writeCharacters(text());
    writer.writeEndElement();
}

void TextElementData::doAddToReport()
{
    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    int start = startPosition();
    cursor.setPosition(start);
    cursor.beginEditBlock();
    KDReports::ReportBuilder builder( docData,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    //Needs to take in consideration paragraph option data
    if (ParagraphData* paragraph = qobject_cast<ParagraphData*>(parentElement())) {
        paragraph->initBuilder(builder);
        if (id().isEmpty()) {
            builder.addBlockElement(m_textElement, Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())), paragraph->background().color());
            m_oldSize = text().size();
        } else {
            builder.addBlockElement(KDReports::TextElement("{"+id()+"}"), Qt::AlignmentFlag(static_cast<int>(paragraph->alignment())), paragraph->background().color());
            m_oldSize = id().size() + 2;
        }
    }
    cursor.endEditBlock();
    m_onDocument = true;
}

int TextElementData::elementSize() const
{
    return m_oldSize;
}

bool TextElementData::insertText(EditorData* editorData, const QString &insertedText, int cursorPosition)
{
    int position = cursorPosition - startPosition();
    if (position == 0) {
        if (TextElementData* previousText = qobject_cast<TextElementData*>(previousChild()))
            return previousText->insertText(editorData, insertedText, cursorPosition);
    }

    m_updateEnabled = false;
    if (id().isEmpty()) {
        QString newText(text());
        newText.insert(position, insertedText);
        editorData->changeProperty(this, "text", newText, 1);
        m_oldSize = m_textElement.text().size();
    } else {
        if (position==m_oldSize) {
            // Create a new text
            return ParagraphChild::insertText(editorData, insertedText, cursorPosition);
        }
        position--;
        QString newId(id());
        newId.insert(position, insertedText);
        editorData->changeProperty(this, "id", newId);
        m_oldSize = id().size() + 2;
    }
    emit updated();
    m_updateEnabled = true;
    return true;
}

void TextElementData::addToHierarchy(ObjectData *parent, int cursorPosition, EditorData *editor)
{
    if (parent->startPosition() == cursorPosition) {
        addToHierarchyPrevious(parent, editor);
    } else if (parent->endPosition() == cursorPosition) {
        addToHierarchyNext(parent, editor);
    } else {
        if (TextElementData *textData1 = qobject_cast<TextElementData*>(parent)) {
            textData1->splitText(cursorPosition, editor);
            editor->insertSubElement(textData1->parentElement(), this, textData1->childPosition() + 1);
        }
    }
}

void TextElementData::splitText(int cursorPosition, EditorData *editor)
{
    TextElementData *textData2 = new TextElementData(m_textElement);
    int position = cursorPosition - startPosition();
    QString text1 = text().mid(0, position);
    QString text2 = text().mid(position);
    ObjectData* paragraph = parentElement();
    textData2->setText(text2);
    int childPos = childPosition();
    editor->insertSubElement(paragraph, textData2, childPos + 1);
    editor->changeProperty(this, "text", text1);
}

QString TextElementData::translatePropertyName(const QString &name) const
{
    if (name == "text")
        return tr("Text");
    else if (name == "id")
        return tr("id");
    else if (name == "font")
        return tr("Font");
    else if (name == "background")
        return tr("Background");
    else if (name == "textColor")
        return tr("Text color");
    else if (name == "modelKey")
        return tr("Model key");
    else if (name == "modelRow")
        return tr("Model row");
    else if (name == "modelColumn")
        return tr("Model column");
    return ObjectData::translatePropertyName(name);
}

QVariant TextElementData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "modelRow" || name == "modelColumn") && type == ObjectData::MIN)
        return 0;
    return ObjectData::propertyValue(name, type);
}
