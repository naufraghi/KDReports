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
#include <QtTest/QtTest>
#include <ReportTextEdit.h>
#include <EditorData.h>
#include "structure/TableElementData.h"
#include "structure/CellData.h"
#include "structure/ReportData.h"
#include <KDReports>
#include <KDReportsTextDocument_p.h>

#include <QDebug>

using namespace KDReports;
namespace KDReports { class Test; }

class KDReports::Test : public QObject {

    Q_OBJECT

private slots:

    void testCellWritingWhenEmpty()
    {
        // Create default table in order to compare
        Report report;
        TableElement table;
        table.cell(0, 0).addElement(TextElement("0:0"));
        table.cell(0, 1).addElement(TextElement("0:1"));
        report.addElement(table);

        EditorData editorData;
        ReportTextEdit editor(&editorData);
        editorData.newReport();
        editor.setDocument(&editorData.document());

        TableElementData *tableElement = new TableElementData;
        tableElement->addToHierarchy(editorData.reportData(), 0, &editorData);
        editorData.insertColumn(tableElement, 0);

        CellData *cell = tableElement->cellAt(0, 0);
        editor.changeCursorPosition(cell->startPosition());
        QTest::keyClicks(&editor, "0:0");

        cell = tableElement->cellAt(0, 1);
        editor.changeCursorPosition(cell->startPosition());
        QTest::keyClicks(&editor, "0:1");

        QCOMPARE(editor.document()->toPlainText(), report.doc().contentDocument().toPlainText());
    }
};

QTEST_MAIN(Test)

#include "TestTable.moc"
