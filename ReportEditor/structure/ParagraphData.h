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

#ifndef PARAGRAPHDATA_H
#define PARAGRAPHDATA_H

#include "TopElement.h"
#include "ParagraphOptionData.h"
#include <QBrush>
#include <KDReportsReportBuilder_p.h>
#include "ParagraphChild.h"
#include "editor_export.h"

typedef QList<QTextOption::Tab> Tabs;
Q_DECLARE_METATYPE(Tabs)

class EDITOR_EXPORT ParagraphData : public TopElement
{
    Q_OBJECT
    Q_PROPERTY(ParagraphData::Alignment alignment READ alignment WRITE setAlignment )
    Q_PROPERTY (QBrush background READ background WRITE setBackground)
    Q_PROPERTY (bool ownOptionData READ ownOptionData WRITE setOwnOptionData)
    Q_PROPERTY(Tabs tabs READ tabs WRITE setTabs )
    Q_PROPERTY (double marginTop READ marginTop WRITE setMarginTop)
    Q_PROPERTY (double marginLeft READ marginLeft WRITE setMarginLeft)
    Q_PROPERTY (double marginBottom READ marginBottom WRITE setMarginBottom)
    Q_PROPERTY (double marginRight READ marginRight WRITE setMarginRight)
public:
    enum Alignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };
    Q_ENUMS(Alignment)
    explicit ParagraphData(ParagraphData* previous,Qt::AlignmentFlag alignment=Qt::AlignLeft, const QBrush& bgcolor = QColor());
    ~ParagraphData();
    ParagraphOptionData * usedOptionData() const;
    void setAlignment(ParagraphData::Alignment alignment);
    void setAlignment(Qt::Alignment alignment);
    ParagraphData::Alignment alignment() const;
    QList<QTextOption::Tab> tabs() const;
    void setTabs(QList<QTextOption::Tab> tabs);
    double marginTop() const;
    void setMarginTop(double marginTop);
    double marginRight() const;
    void setMarginRight(double marginRight);
    double marginLeft() const;
    void setMarginLeft(double marginLeft);
    double marginBottom() const;
    void setMarginBottom(double marginBottom);
    QBrush background() const;
    void setBackground(const QBrush& brush);
    bool ownOptionData() const;
    void setOwnOptionData(bool ownOptionData);
    virtual void writeXml(QXmlStreamWriter &writer);
    virtual int endPosition() const;
    virtual int startPosition() const;
    void initBuilder(KDReports::ReportBuilder &builder);
    void setNextParagraph(ParagraphData *paragraph);
    void setPreviousParagraph(ParagraphData *paragraph);
    void setOptionData(ParagraphOptionData *option);
    virtual QRect elementRect(ReportTextEdit *reportTextEdit);
    virtual void doUpdate();
    virtual void doRemove();
    virtual void doAddToReport();
    virtual bool insertText(EditorData *editorData, const QString &text, int cursorPosition);
    void splitParagraph(int childPosition, EditorData *editor);
    virtual void validate();
    QString translatePropertyName(const QString &name) const;
    QVariant propertyValue(const QString &name, ValueType type) const;
    QString translatedEnumValue(const QMetaEnum &metaEnum, int key) const;

private:
    ParagraphData * findNext() const;
    ParagraphData * findPrevious() const;

    ParagraphData::Alignment m_alignment;
    QBrush m_background;
    ParagraphOptionData *m_optionData;
    bool m_ownOptionData;
    ParagraphData* m_previous;
    ParagraphData* m_next;
    bool m_update;
};

#endif // PARAGRAPHDATA_H
