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

#include "DesignerWidget.h"
#include "ReportTextEdit.h"
#include "HeaderEditorWidget.h"
#include <QBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include "EditorData.h"
#include "structure/ReportData.h"
#include <KDReportsPreviewWidget.h>
#include <QBuffer>

DesignerWidget::DesignerWidget(EditorData* editorData, QWidget *parent)
    : QTabWidget(parent)
    , m_editorData(editorData)
    , m_previewReport(0)
{
    // Body
    m_bodyWidget = new ReportTextEdit(editorData, this);
    addTab(m_bodyWidget, tr("Body"));

    // Header
    m_headerWidget = new HeaderEditorWidget(editorData, EditorData::Header, this);
    addTab(m_headerWidget, tr("Header"));

    // Footer
    m_footerWidget = new HeaderEditorWidget(editorData, EditorData::Footer, this);
    addTab(m_footerWidget, tr("Footer"));

    // Preview
    m_preview = new KDReports::PreviewWidget();
    addTab(m_preview, tr("Preview"));

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
}

void DesignerWidget::setBodyDocument(QTextDocument *document)
{
    m_bodyWidget->cleanSelection();
    m_bodyWidget->setDocument(document);
}

QTextDocument * DesignerWidget::bodyDocument() const
{
    return m_bodyWidget->document();
}

int DesignerWidget::bodyCursorPosition() const
{
    return m_bodyWidget->textCursor().position();
}

void DesignerWidget::updatePreview()
{
    delete m_previewReport;
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    m_editorData->reportData()->saveToXml(&buffer);
    m_previewReport = new KDReports::Report(m_preview);
    if (m_previewReport->loadFromXML(&buffer))
        m_preview->setReport(m_previewReport);
}

KDReports::Report *DesignerWidget::report() const
{
    return m_previewReport;
}

void DesignerWidget::slotTabChanged(int index)
{
    if (m_editorData->reportData()) {
        // Preview tab
        if (index == indexOf(m_preview)) {
            updatePreview();
        } else if (index == indexOf(m_bodyWidget)) {
            m_editorData->setScope(EditorData::Body);
        } else if (index == indexOf(m_headerWidget)) {
            m_editorData->setScope(EditorData::Header);
            m_headerWidget->updateDocument();
        } else if (index == indexOf(m_footerWidget)) {
            m_editorData->setScope(EditorData::Footer);
            m_footerWidget->updateDocument();
        }
    }
    if (index == indexOf(m_preview))
        emit changedView(Preview);
    else
        emit changedView(Editor);
}

void DesignerWidget::repaintPrev()
{
    m_preview->repaint();
}
