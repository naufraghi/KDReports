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

#include "ParagraphData.h"

#include "CellData.h"
#include "../EditorData.h"
#include "ReportData.h"
#include "TextElementData.h"
#include "VSpaceData.h"
#include <QTextDocument>
#include <QDebug>
#include <QTextBlock>
#include "PageBreakData.h"
#include "ReportTextEdit.h"
#include <QAbstractTextDocumentLayout>

ParagraphData::ParagraphData(ParagraphData* previous, Qt::AlignmentFlag alignment, const QBrush &bgcolor)
    : TopElement()
    , m_alignment(ParagraphData::Alignment(static_cast<int>(alignment)))
    , m_background(bgcolor)
    , m_optionData(new ParagraphOptionData())
    , m_ownOptionData(false)
    , m_previous(previous)
    , m_next(0)
    , m_update(false)
{
    setObjectName(tr("paragraph"));
}

ParagraphData::~ParagraphData()
{
    delete m_optionData;
}

ParagraphOptionData* ParagraphData::usedOptionData() const
{
    if (m_ownOptionData || !m_previous)
        return m_optionData;
    return m_previous->usedOptionData();
}

void ParagraphData::setAlignment(Qt::Alignment alignment)
{
    m_alignment = ParagraphData::Alignment(static_cast<int>(alignment));
}

void ParagraphData::setAlignment(ParagraphData::Alignment alignment)
{
    m_alignment = alignment;
}

ParagraphData::Alignment ParagraphData::alignment() const
{
    return m_alignment;
}

QList<QTextOption::Tab> ParagraphData::tabs() const
{
    return usedOptionData()->m_tabs;
}
void ParagraphData::setTabs(QList<QTextOption::Tab> tabs)
{
    if(!m_ownOptionData)
        setOptionData(new ParagraphOptionData);
    m_optionData->m_tabs = tabs;
}

double ParagraphData::marginTop() const
{
    return usedOptionData()->m_topMargin;
}

void ParagraphData::setMarginTop(double marginTop)
{
    if(!m_ownOptionData)
        setOptionData(new ParagraphOptionData);
    m_optionData->m_topMargin = marginTop;
}

double ParagraphData::marginBottom() const
{
    return usedOptionData()->m_bottomMargin;
}

void ParagraphData::setMarginBottom(double marginBottom)
{
    if(!m_ownOptionData)
        setOptionData(new ParagraphOptionData);
    m_optionData->m_bottomMargin = marginBottom;
}

double ParagraphData::marginLeft() const
{
    return usedOptionData()->m_leftMargin;
}

void ParagraphData::setMarginLeft(double marginLeft)
{
    if(!m_ownOptionData)
        setOptionData(new ParagraphOptionData);
    m_optionData->m_leftMargin = marginLeft;
}

double ParagraphData::marginRight() const
{
    return usedOptionData()->m_rightMargin;
}

void ParagraphData::setMarginRight(double marginRight)
{
    if(!m_ownOptionData)
        setOptionData(new ParagraphOptionData);
    m_optionData->m_rightMargin = marginRight;
}

QBrush ParagraphData::background() const
{
    return m_background;
}

void ParagraphData::setBackground(const QBrush &brush)
{
    m_background = brush;
}

void ParagraphData::writeXml(QXmlStreamWriter &writer)
{
    if (m_ownOptionData) {
        writer.writeStartElement(QString::fromLatin1("paragraph-margins"));
        writer.writeAttribute(QString::fromLatin1("left"), QString::number(m_optionData->m_leftMargin));
        writer.writeAttribute(QString::fromLatin1("top"), QString::number(m_optionData->m_topMargin));
        writer.writeAttribute(QString::fromLatin1("right"), QString::number(m_optionData->m_rightMargin));
        writer.writeAttribute(QString::fromLatin1("bottom"), QString::number(m_optionData->m_bottomMargin));
        writer.writeEndElement();
        if (m_optionData->m_tabs.size()) {
            writer.writeStartElement(QString::fromLatin1("tabs"));
            foreach (QTextOption::Tab tab, m_optionData->m_tabs) {
                writer.writeStartElement(QString::fromLatin1("tab"));
                QString tabType = "left";
                if (tab.type == QTextOption::RightTab) {
                    tabType = QLatin1String("right");
                } else if (tab.type == QTextOption::CenterTab) {
                    tabType = QLatin1String("center");
                } else if (tab.type == QTextOption::DelimiterTab) {
                    tabType = QLatin1String("delimiter");
                }
                writer.writeAttribute(QString::fromLatin1("type"), tabType);

                QString position = "";
                QString delimiter = tab.delimiter;
                if (tab.delimiter == QChar::fromLatin1( 'P' )) {
                    position = QLatin1String("page");
                } else {
                    position = QString::number(tab.position);
                }
                if (!position.isEmpty())
                    writer.writeAttribute(QString::fromLatin1("position"), position);
                if (!delimiter.isEmpty())
                    writer.writeAttribute(QString::fromLatin1("delimiter"), delimiter);
                writer.writeEndElement();
            }
            writer.writeEndElement();
        }
    }
    foreach(ObjectData* od, childrenElements()){
            od->writeXml(writer);
    }
}

int ParagraphData::endPosition() const
{
    if (!m_onDocument)
        return startPosition();
    if (CellData* cell = qobject_cast<CellData*>(parentElement())) {
        if (!nextChild())
            return cell->endPosition() - 1;
        else {
            if (!childrenElements().isEmpty()) {
                return qMin(childrenElements().last()->endPosition() + 1, cell->endPosition() - 1);
            }
            return qMin(startPosition() + 1, cell->endPosition() - 1);
        }
    }
    QTextBlock lastBlock = textDocumentData().document().lastBlock();
    int lineReturn = 0;
    if (m_lineReturn)
        lineReturn = 1;
    if (!childrenElements().isEmpty()) {
        return qMin(childrenElements().last()->endPosition() + lineReturn, lastBlock.position() + lastBlock.length()-1);
    }
    return qMin(startPosition() + lineReturn, lastBlock.position() + lastBlock.length()-1);
}

void ParagraphData::doUpdate()
{
    m_update = true;
    doRemove();
    doAddToReport();
    m_update = false;
}

void ParagraphData::setNextParagraph(ParagraphData* paragraph)
{
    m_next = paragraph;
}

void ParagraphData::setPreviousParagraph(ParagraphData* paragraph)
{
    if (paragraph != this) {
        m_previous = paragraph;
    } else {
        qDebug() << "error" << this << paragraph;
        Q_ASSERT(false);
    }
}

void ParagraphData::initBuilder(KDReports::ReportBuilder& builder)
{
    builder.setParagraphMargins(marginLeft(), marginTop(), marginRight(), marginBottom());
    builder.setTabPositions(tabs());
}

void ParagraphData::doRemove()
{
    if (!m_update) {
        if (m_previous)
            m_previous->setNextParagraph(m_next);
        else {
            if (m_next) {
                if (!m_next->m_ownOptionData)
                    m_next->setOptionData(new ParagraphOptionData(m_optionData));
            }
        }
        if (m_next)
            m_next->setPreviousParagraph(m_previous);

        m_previous = 0;
        m_next = 0;
    }
    foreach (ObjectData* od, childrenElements()) {
        od->doRemove();
    }

    TopElement::doRemove();
}

void ParagraphData::doAddToReport()
{
    if (!m_update) {
        //TODO set previous and next paragraph
        if (!m_previous)
            setPreviousParagraph(findPrevious());
        if (m_previous)
            m_previous->setNextParagraph(this);
        if (!m_next)
            setNextParagraph(findNext());
        if (m_next)
            m_next->setPreviousParagraph(this);

        if (!m_previous)
            m_ownOptionData = true;
    }

    KDReports::TextDocumentData& docData = textDocumentData();
    QTextCursor cursor(&docData.document());
    cursor.setPosition(startPosition());
    QTextBlockFormat f = cursor.blockFormat();
    cursor.beginEditBlock();
    insertLineBefore(cursor);
    foreach (ObjectData* od, childrenElements()) {
        od->doAddToReport();
    }
    insertLineAfter(cursor, f);
    cursor.endEditBlock();
    m_onDocument = true;

    if (m_update && m_next && (m_next->usedOptionData() == usedOptionData())) {
        m_next->update();
    }
}

void ParagraphData::setOptionData(ParagraphOptionData *option) {
    delete m_optionData;
    m_ownOptionData = true;
    m_optionData = option;
}

ParagraphData* ParagraphData::findNext() const
{
    ObjectData *o = const_cast<ParagraphData*>(this);
    while ((o = o->nextChild()))
        if (ParagraphData* p = qobject_cast<ParagraphData*>(o))
            return p;
    return 0;
}

ParagraphData* ParagraphData::findPrevious() const
{
    ObjectData *o = const_cast<ParagraphData*>(this);
    while ((o = o->previousChild()))
        if (ParagraphData* p = qobject_cast<ParagraphData*>(o))
            return p;
    return 0;
}

int ParagraphData::startPosition() const
{
    if (CellData* cell = qobject_cast<CellData*>(parentElement())) {
        if (!previousChild())
            return cell->startPosition();
    }
    return ObjectData::startPosition();
}

QRect ParagraphData::elementRect(ReportTextEdit *reportTextEdit)
{
    return reportTextEdit->document()->documentLayout()->blockBoundingRect(reportTextEdit->document()->findBlock(startPosition())).toRect().adjusted(-2,-2,2,2);
}

bool ParagraphData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    if (startPosition() == cursorPosition)
        return editorData->insertSubElement(this, new TextElementData(text), 0);
    else {
        qWarning() << "write text here not yet allowed" << this << cursorPosition;
        return false;
    }
}

void ParagraphData::splitParagraph(int atChildPosition, EditorData* editor)
{
    if (atChildPosition == 0 || atChildPosition == childrenElements().size())
        return;
    ParagraphData *newParagraph = new ParagraphData(const_cast<ParagraphData*>(this), Qt::AlignmentFlag(static_cast<int>(alignment())), background());
    editor->insertSubElement(parentElement(), newParagraph, childPosition() + 1);
    for (int i = atChildPosition; i < childrenElements().size();i++) {
        ObjectData* o = childrenElements().at(i);
        editor->moveElement(o, newParagraph, i - atChildPosition);
    }
}

void ParagraphData::validate()
{
    TopElement::validate();
    foreach (ObjectData* object, childrenElements())
        object->validate();
}

QString ParagraphData::translatePropertyName(const QString &name) const
{
    if (name == "alignment")
        return tr("Alignment");
    else if (name == "background")
        return tr("Background");
    else if (name == "tabs")
        return tr("Tabs");
    else if (name == "marginTop")
        return tr("Margin top");
    else if (name == "marginBottom")
        return tr("Margin bottom");
    else if (name == "marginLeft")
        return tr("Margin Left");
    else if (name == "marginRight")
        return tr("Margin right");
    return ObjectData::translatePropertyName(name);
}

QVariant ParagraphData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "marginTop" || name == "marginBottom" || name == "marginLeft"
         || name == "marginRight") && type == ObjectData::MIN)
        return 0.0;
    return ObjectData::propertyValue(name, type);
}

QString ParagraphData::translatedEnumValue(const QMetaEnum &metaEnum, int key) const
{
    if (QString::fromLatin1(metaEnum.name()) == "Location")
    {
        switch (metaEnum.value(key)) {
        case ParagraphData::AlignHCenter:
            return tr("Center");
        case ParagraphData::AlignLeft:
            return tr("Left");
        case ParagraphData::AlignRight:
            return tr("Right");
        }
    }
    return ObjectData::translatedEnumValue(metaEnum, key);
}

bool ParagraphData::ownOptionData() const
{
    return m_ownOptionData;
}

void ParagraphData::setOwnOptionData(bool ownOptionData)
{
    m_ownOptionData = ownOptionData;
}
