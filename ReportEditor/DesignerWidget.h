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

#ifndef DESIGNERWIDGET_H
#define DESIGNERWIDGET_H

#include <QTabWidget>
class QTextEdit;
class ReportTextEdit;
class QTextDocument;
class EditorData;
class HeaderEditorWidget;
namespace KDReports {
class PreviewWidget;
class Report;
}
class DesignerWidget : public QTabWidget
{
    Q_OBJECT
    Q_FLAGS(View Views)

public:
    enum View {NoView = 0, Preview = 0x01, Editor = 0x02};
    Q_DECLARE_FLAGS(Views, View)

    explicit DesignerWidget(EditorData *editorData, QWidget *parent = 0);
    void setBodyDocument(QTextDocument *bodyDocument);
    QTextDocument * bodyDocument() const;
    int bodyCursorPosition() const;
    void updatePreview();
    KDReports::Report * report() const;
    void repaintPrev();
protected slots:
    void slotTabChanged(int index);
signals:
    void changedView(DesignerWidget::View v);
private:
    EditorData* m_editorData;
    ReportTextEdit* m_bodyWidget;
    HeaderEditorWidget* m_headerWidget;
    HeaderEditorWidget* m_footerWidget;
    KDReports::PreviewWidget* m_preview;
    KDReports::Report *m_previewReport;
};

#endif // DESIGNERWIDGET_H
