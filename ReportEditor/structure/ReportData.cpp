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

#include "ReportData.h"

#include "HeaderData.h"
#include "FooterData.h"
#include "TableElementData.h"
#include "TextElementData.h"
#include "CellData.h"
#include "../EditorData.h"
#include "ParagraphData.h"
#include <QDebug>
#include <QFile>
#include <KDReportsTextDocument_p.h>
#include <QMetaProperty>

ReportData::ReportData(KDReports::Report& report)
    : ObjectData(0)
    , m_report(report)
    , m_lastObject(0)
    , m_header(0)
{
    setObjectName(tr("report"));
    connect(this, SIGNAL(updated()), this, SLOT(slotUpdate()));
    m_parentStack.push(this);
}

void ReportData::addObjectData(ObjectData* object)
{
    // If the last item on the stack is a Paragraph we pop it.
    if (!m_parentStack.isEmpty() && qobject_cast<ParagraphData*>(m_parentStack.top())) {
        m_parentStack.pop();
    }

    if (!m_parentStack.isEmpty() && !qobject_cast<HeaderData*>(object))
        object->setParentElement(m_parentStack.top());

    // Push new parent
    // Header and footer case is the same, footer inherits header
    if (HeaderData* header = qobject_cast<HeaderData*>(object)) {
        if(m_parentStack.size() == 1){
            m_header = header;
            m_header->setAssociatedReportData(this);
            m_parentStack.push(header);
            if (FooterData* footer = qobject_cast<FooterData*>(header)) {
                m_footerMap.insert(footer->location(), footer);
            } else {
                m_headerMap.insert(header->location(), header);
            }
        } else {
            return;
        }
    } else if(CellData* cell = qobject_cast<CellData*>(object)) {
        if(m_parentStack.size() > 0 && qobject_cast<TableElementData*>(m_parentStack.top())){
            m_parentStack.push(cell);
        } else {
            return;
        }
    }
    connect(object, SIGNAL(updated()), this, SLOT(slotUpdate()));
}

void ReportData::slotUpdate()
{
    QObject* o = sender();
    if (ObjectData* sender = qobject_cast<ObjectData* >(o)) {
        emit updated(sender);
    }
}

void ReportData::addObjectElementData(ObjectData *object, bool inlineElement, Qt::AlignmentFlag alignment, const QBrush &bgColor)
{
    ParagraphData* p = 0;

    if(!inlineElement){
        // Check if the last item on the stack is a Paragraph
        if(!m_parentStack.isEmpty() && qobject_cast<ParagraphData*>(m_parentStack.top())){
            p = qobject_cast<ParagraphData*>(m_parentStack.top());
            if(!p->childrenElements().isEmpty()){
                m_parentStack.pop();
                p = 0;
            }
        }
        if(!p){
            p = createParagraph(alignment, bgColor);
        } else {
            p->setAlignment(alignment);
            p->setBackground(bgColor);
        }
    } else {
        if(!m_parentStack.isEmpty() && !qobject_cast<ParagraphData*>(m_parentStack.top())){
            p = createParagraph(alignment, bgColor);
        }
    }

    if(!m_parentStack.isEmpty())
        object->setParentElement(m_parentStack.top());
     // Push new parent
    if(TableElementData* table = qobject_cast<TableElementData*>(object)){
        if(m_parentStack.size() >=1){
            m_parentStack.push(table);
        } else {
            return;
        }
    }

    QVariant id = object->property("id");
    if (id.isValid()) {
        QString stringId = id.toString();
        if (!stringId.isEmpty()) {
            m_idMap.insert(stringId, object);
        }
        connect(object, SIGNAL(idChanged(QString, QString)), SLOT(slotIdChanged(QString, QString)));
    }

    QVariant model = object->property("modelKey");
    if (model.isValid()) {
        QString stringModel = model.toString();
        if (!stringModel.isEmpty()) {
            m_modelMap.insert(stringModel, object);
        }
        connect(object, SIGNAL(modelKeyChanged(QString, QString)), SLOT(slotModelKeyChanged(QString, QString)));
    }

    connect(object, SIGNAL(updated()), this, SLOT(slotUpdate()));
}

void ReportData::slotIdChanged(const QString &newId, const QString &oldId)
{
    ObjectData* o = qobject_cast<ObjectData*>(sender());
    m_idMap.remove(oldId, o);
    if (!m_idMap.uniqueKeys().contains(oldId) && (newId.isEmpty()
            || m_idMap.uniqueKeys().contains(newId))) {
        // remove
        emit idRemoved(oldId);
    }
    // add
    if (!newId.isEmpty()) {
        if (!m_idMap.uniqueKeys().contains(newId)) {
            if (oldId.isEmpty())
                emit idAdded(newId);
            else
                emit idModified(newId, oldId);
        }
        m_idMap.insert(newId, o);
    }
}

void ReportData::slotModelKeyChanged(const QString &newModelKey, const QString &oldModelKey)
{
    ObjectData* o = qobject_cast<ObjectData*>(sender());
    m_modelMap.remove(oldModelKey, o);
    if (!m_modelMap.uniqueKeys().contains(oldModelKey) && (newModelKey.isEmpty()
            || m_modelMap.uniqueKeys().contains(newModelKey))) {
        // remove
        emit modelRemoved(oldModelKey);
    }
    // add
    if (!newModelKey.isEmpty()) {
        if (!m_modelMap.uniqueKeys().contains(newModelKey)) {
            if (m_modelMap.uniqueKeys().contains(oldModelKey) || oldModelKey.isEmpty())
                emit modelAdded(newModelKey);
            else
                emit modelModified(newModelKey, oldModelKey);
        }
        m_modelMap.insert(newModelKey, o);
    }

}

void ReportData::addModelKey(const QString &modelKey, ObjectData* o)
{
    if (!m_modelMap.uniqueKeys().contains(modelKey))
        emit modelAdded(modelKey);
    m_modelMap.insert(modelKey, o);
}

QString ReportData::translatePropertyName(const QString &name) const
{
    if (name == "defaultFont")
        return tr("Default font");
    else if (name == "headerBodySpacing")
        return tr("Header body spacing");
    else if (name == "footerBodySpacing")
        return tr("Footer body spacing");
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

QVariant ReportData::propertyValue(const QString &name, ObjectData::ValueType type) const
{
    if ((name == "headerBodySpacing" || name == "footerBodySpacing"
         || name == "marginTop" || name == "marginBottom" || name == "marginLeft"
         || name == "marginRight") && type == ObjectData::MIN)
        return 0.0;
    return ObjectData::propertyValue(name, type);
}

void ReportData::addHeader(HeaderData *header)
{
    header->setAssociatedReportData(this);
    m_headerMap.insert(header->location(), header);
}

void ReportData::addFooter(FooterData *footer)
{
    footer->setAssociatedReportData(this);
    m_footerMap.insert(footer->location(), footer);
}

void ReportData::updateHeaderLocation(HeaderData* header)
{
    if (FooterData* footer = qobject_cast<FooterData* >(header)) {
        m_footerMap.remove(m_footerMap.key(footer));
        m_footerMap.insert(footer->location(), footer);
    } else if (header) {
        m_headerMap.remove(m_headerMap.key(header));
        m_headerMap.insert(header->location(), header);
    }
}

void ReportData::endHeader()
{
    while(!m_parentStack.isEmpty()){
        if(qobject_cast<HeaderData*>(m_parentStack.pop())){
            m_header = 0;
            break;
        }
    }
}

void ReportData::endFooter()
{
    endHeader();
}

void ReportData::endCell()
{
    while(!m_parentStack.isEmpty()){
        if(qobject_cast<CellData*>(m_parentStack.pop())){
            break;
        }
    }
}

void ReportData::endTableElement(KDReports::TableElement& tableElement)
{
    Q_UNUSED(tableElement);
    while(!m_parentStack.isEmpty()){
        if(TableElementData* t = qobject_cast<TableElementData*>(m_parentStack.pop())){
            m_tableList.append(t);
            break;
        }
    }
}

void ReportData::setTabs(const QList<QTextOption::Tab> &tabs)
{
    ParagraphData* p = 0;
    // Check if the last item on the stack is a Paragraph
    if(!m_parentStack.isEmpty() && qobject_cast<ParagraphData*>(m_parentStack.top())){
        p = qobject_cast<ParagraphData*>(m_parentStack.top());
        if(!p->childrenElements().isEmpty()){
            m_parentStack.pop();
            p = 0;
        }
    }
    if(!p){
        p = createParagraph();
    }
    p->setTabs(tabs);
}

ParagraphData* ReportData::createParagraph(Qt::AlignmentFlag alignment, const QBrush &bgcolor)
{
    ParagraphData* last = m_lastParagraph.value(m_parentStack.top(), 0);
    ParagraphData* p = new ParagraphData(last, alignment, bgcolor);
    if(last)
        last->setNextParagraph(p);
    m_lastParagraph.insert(m_parentStack.top(), p);
    connect(p, SIGNAL(updated()), this, SLOT(slotUpdate()));
    p->setParentElement(m_parentStack.top());
    m_parentStack.push(p);
    return p;
}

void ReportData::setParagraphMargin(qreal left, qreal top, qreal right, qreal bottom)
{
    ParagraphData* p = 0;
    // Check if the last item on the stack is a Paragraph
    if(!m_parentStack.isEmpty() && qobject_cast<ParagraphData*>(m_parentStack.top())){
        p = qobject_cast<ParagraphData*>(m_parentStack.top());
        if(!p->childrenElements().isEmpty()){
            m_parentStack.pop();
            p = 0;
        }
    }
    if(!p){
        p = createParagraph();
    }
    p->setMarginLeft(left);
    p->setMarginTop(top);
    p->setMarginRight(right);
    p->setMarginBottom(bottom);
}

QFont ReportData::defaultFont() const
{
    return m_report.defaultFont();
}

void ReportData::setDefaultFont(const QFont &font)
{
    m_report.setDefaultFont(font);
}

double ReportData::headerBodySpacing() const
{
    return m_report.headerBodySpacing();
}

void ReportData::setHeaderBodySpacing(double spacing)
{
    m_report.setHeaderBodySpacing(spacing);
}

double ReportData::footerBodySpacing() const
{
    return m_report.footerBodySpacing();
}

void ReportData::setFooterBodySpacing(double spacing)
{
    m_report.setFooterBodySpacing(spacing);
}

double ReportData::marginTop() const
{
    return m_report.topPageMargins();
}

void ReportData::setMarginTop(double marginTop)
{
    m_report.setTopPageMargin(marginTop);
}

double ReportData::marginBottom() const
{
    return m_report.bottomPageMargins();
}

void ReportData::setMarginBottom(double marginBottom)
{
    m_report.setBottomPageMargin(marginBottom);
}

double ReportData::marginLeft() const
{
    return m_report.leftPageMargins();
}

void ReportData::setMarginLeft(double marginLeft)
{
    m_report.setLeftPageMargin(marginLeft);
}

double ReportData::marginRight() const
{
    return m_report.rightPageMargins();
}

void ReportData::setMarginRight(double marginRight)
{
    m_report.setRightPageMargin(marginRight);
}

void ReportData::saveToXml(QIODevice *device)
{
    QXmlStreamWriter writer(device);
    writeXml(writer);
}

void ReportData::writeXml(QXmlStreamWriter &writer)
{
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDefaultNamespace(QString::fromLatin1("http://www.kdab.com/kdreports"));
    writer.writeStartElement(QString::fromLatin1("report"));
    writer.writeAttribute(QString::fromLatin1("font"), defaultFont().family());
    writer.writeAttribute(QString::fromLatin1("pointsize"), QString::number(defaultFont().pointSizeF()));
    writer.writeAttribute(QString::fromLatin1("header-body-spacing"), QString::number(headerBodySpacing()));
    writer.writeAttribute(QString::fromLatin1("footer-body-spacing"), QString::number(footerBodySpacing()));
    writer.writeAttribute(QString::fromLatin1("margin-top"), QString::number(marginTop()));
    writer.writeAttribute(QString::fromLatin1("margin-left"), QString::number(marginLeft()));
    writer.writeAttribute(QString::fromLatin1("margin-bottom"), QString::number(marginBottom()));
    writer.writeAttribute(QString::fromLatin1("margin-right"), QString::number(marginRight()));


    // save header
    foreach (HeaderData* h, m_headerMap.values())
        h->writeXml(writer);

    // save footer
    foreach (FooterData* f, m_footerMap.values())
        f->writeXml(writer);

    foreach(ObjectData* od, childrenElements()){
            od->writeXml(writer);
    }
    writer.writeEndElement();
    writer.writeEndDocument();
}

int ReportData::endPosition() const
{
    if (!childrenElements().isEmpty()) {
            return childrenElements().last()->endPosition();
    }
    return -1;
}

KDReports::TextDocumentData & ReportData::textDocumentData() const
{
    return m_report.doc().contentDocumentData();
}

void ReportData::doUpdate()
{
    foreach (ObjectData* od, childrenElements()) {
        od->doUpdate();
    }
}

ObjectData* ReportData::topContainer() const
{
    return const_cast<ReportData*>(this);
}

TableElementData * ReportData::currentTable() const
{
    return qobject_cast<TableElementData *>(m_parentStack.top());
}

void ReportData::validate()
{
    foreach (ObjectData* object, childrenElements())
        object->validate();
}

QMap<HeaderData::Locations, HeaderData *> ReportData::headerMap() const
{
    return m_headerMap;
}

QMap<HeaderData::Locations, FooterData *> ReportData::footerMap() const
{
    return m_footerMap;
}

QMultiMap<QString, ObjectData *> ReportData::idMap() const
{
    return m_idMap;
}

QMultiMap<QString, ObjectData *> ReportData::modelKeyMap() const
{
    return m_modelMap;
}

bool ReportData::supportParagraph() const
{
    return true;
}

bool ReportData::insertText(EditorData *editorData, const QString &text, int cursorPosition)
{
    if (startPosition() == cursorPosition)
        return editorData->insertSubElement(this, new TextElementData(text), 0);
    else {
        qWarning() << "write text here not yet allowed" << this << cursorPosition;
        return false;
    }
}
