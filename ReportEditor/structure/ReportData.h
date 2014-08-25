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

#ifndef REPORTDATA_H
#define REPORTDATA_H

#include <QStack>
#include "ObjectData.h"
#include "HeaderData.h"
#include "FooterData.h"
#include <KDReportsReport.h>
#include "KDReportsTableElement.h"
#include <QBrush>
#include <QSet>

class ParagraphData;
class TableElementData;

class EDITOR_EXPORT ReportData : public ObjectData
{
    Q_OBJECT
    Q_PROPERTY (QFont defaultFont READ defaultFont WRITE setDefaultFont)
    Q_PROPERTY (double headerBodySpacing READ headerBodySpacing WRITE setHeaderBodySpacing)
    Q_PROPERTY (double footerBodySpacing READ footerBodySpacing WRITE setFooterBodySpacing)
    Q_PROPERTY (double marginTop READ marginTop WRITE setMarginTop)
    Q_PROPERTY (double marginLeft READ marginLeft WRITE setMarginLeft)
    Q_PROPERTY (double marginBottom READ marginBottom WRITE setMarginBottom)
    Q_PROPERTY (double marginRight READ marginRight WRITE setMarginRight)

public:
    explicit ReportData(KDReports::Report& report);
    void addObjectData(ObjectData* object);
    void addObjectElementData(ObjectData* object, bool inlineElement = false, Qt::AlignmentFlag alignment = Qt::AlignLeft, const QBrush& bgColor = QColor());
    void endHeader();
    void endFooter();
    void endTableElement(KDReports::TableElement& tableElement);
    void endCell();
    void setTabs(const QList<QTextOption::Tab> &tabs);
    void setParagraphMargin(qreal left, qreal top, qreal right, qreal bottom);
    QFont defaultFont() const;
    void setDefaultFont(const QFont& font);
    double headerBodySpacing() const;
    void setHeaderBodySpacing(double spacing);
    double footerBodySpacing() const;
    void setFooterBodySpacing(double spacing);
    double marginTop() const;
    void setMarginTop(double marginTop);
    double marginRight() const;
    void setMarginRight(double marginRight);
    double marginLeft() const;
    void setMarginLeft(double marginLeft);
    double marginBottom() const;
    void setMarginBottom(double marginBottom);
    void saveToXml(QIODevice *device);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    virtual KDReports::TextDocumentData& textDocumentData() const;
    ParagraphData * createParagraph(Qt::AlignmentFlag alignment=Qt::AlignLeft, const QBrush& bgcolor = QColor());
    virtual void doUpdate();
    ObjectData * topContainer() const;
    TableElementData* currentTable() const;
    virtual bool supportParagraph() const;
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    virtual void validate();
    QMap<HeaderData::Locations, HeaderData*> headerMap() const;
    QMap<HeaderData::Locations, FooterData*> footerMap() const;
    QMultiMap<QString, ObjectData*> idMap() const;
    QMultiMap<QString, ObjectData *> modelKeyMap() const;
    void addModelKey(const QString &modelKey, ObjectData *o);
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;
    void addHeader(HeaderData* header);
    void addFooter(FooterData* footer);
    void updateHeaderLocation(HeaderData *header);

private slots:
    void slotUpdate();
    void slotIdChanged(const QString &newId, const QString &oldId);
    void slotModelKeyChanged(const QString &newModelKey, const QString &oldModelKey);

signals:
    void updated(ObjectData*);
    void idAdded(const QString&);
    void idRemoved(const QString&);
    void idModified(const QString&, const QString&);
    void modelAdded(const QString&);
    void modelRemoved(const QString&);
    void modelModified(const QString&, const QString&);

private:
    friend class HeaderData;
    friend class FooterData;
    KDReports::Report& m_report;
    QStack<ObjectData*> m_parentStack;
    ObjectData* m_lastObject;
    HeaderData* m_header;
    QMap<ObjectData*, ParagraphData*> m_lastParagraph;
    QList<TableElementData*> m_tableList;
    QMap<HeaderData::Locations, HeaderData*> m_headerMap;
    QMap<HeaderData::Locations, FooterData*> m_footerMap;
    QMultiMap<QString, ObjectData*> m_idMap;
    QMultiMap<QString, ObjectData*> m_modelMap;
};

#endif // REPORTDATA_H
