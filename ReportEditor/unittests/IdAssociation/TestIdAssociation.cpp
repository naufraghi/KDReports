/****************************************************************************
** Copyright (C) 2012-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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
#include "IdModel.h"
#include "structure/ReportData.h"
#include "structure/TextElementData.h"
#include "structure/HtmlElementData.h"
#include <KDReports>
#include <KDReportsHtmlElement>

using namespace KDReports;

class TestIdAssociation : public QObject {
    Q_OBJECT
private slots:

    void testModelColumnCount()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);
        QCOMPARE(model.columnCount(), 2);
    }

    void testModelAddId()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);
        QCOMPARE(model.rowCount(), 0);

        model.slotAddId("first");
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("first"));

        model.slotAddId("second");
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(1, 0)), QVariant("second"));

        // Add existing id
        model.slotAddId("first");
        QCOMPARE(model.rowCount(), 2);
    }

    void testModelRemoveId()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);

        model.slotAddId("first");
        model.slotAddId("second");
        model.slotAddId("third");

        model.slotRemoveId("second");
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("first"));
        QCOMPARE(model.data(model.index(1, 0)), QVariant("third"));
    }

    void testModelModifyId()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);

        model.slotAddId("first");
        model.slotAddId("second");
        model.slotAddId("third");

        model.slotModifyId("newValue", "second");
        QCOMPARE(model.rowCount(), 3);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("first"));
        QCOMPARE(model.data(model.index(1, 0)), QVariant("newValue"));
        QCOMPARE(model.data(model.index(2, 0)), QVariant("third"));

        // modify with existing id
        model.slotModifyId("third", "first");
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("newValue"));
        QCOMPARE(model.data(model.index(1, 0)), QVariant("third"));
    }

    void testModelSetData()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);

        model.slotAddId("first");
        model.slotAddId("second");
        model.slotAddId("third");

        model.setData(model.index(0, 1), "valueFirst");
        model.setData(model.index(1, 1), "valueSecond");
        model.setData(model.index(2, 1), "valueThird");

        QCOMPARE(model.data(model.index(0, 1)), QVariant("valueFirst"));
        QCOMPARE(model.data(model.index(1, 1)), QVariant("valueSecond"));
        QCOMPARE(model.data(model.index(2, 1)), QVariant("valueThird"));
    }

    void testModelValueAfterModification()
    {
        IdModel model;
        Report r;
        ReportData rd(r);
        model.setReportData(&rd);

        model.slotAddId("first");
        model.slotAddId("second");
        model.slotAddId("third");

        model.setData(model.index(0, 1), "valueFirst");
        model.setData(model.index(1, 1), "valueSecond");
        model.setData(model.index(2, 1), "valueThird");

        model.slotModifyId("newFirst", "first");
        QCOMPARE(model.data(model.index(0, 1)), QVariant("valueFirst"));

        // modify with existing id
        model.slotModifyId("third", "second");
        QCOMPARE(model.data(model.index(0, 1)), QVariant("valueFirst"));
        QCOMPARE(model.data(model.index(1, 1)), QVariant("valueThird"));
    }

    void testReportDataIdChanged()
    {
        IdModel model;
        Report r;

        // Create document
        ReportData rd(r);
        TextElementData *t1 = new TextElementData("text1");
        t1->setId("idT1");
        TextElementData *t2 = new TextElementData("text2");
        t2->setId("idT2");
        TextElementData *t3 = new TextElementData("text3");
        TextElementData *t4 = new TextElementData("text4");
        rd.addObjectElementData(t1);
        rd.addObjectElementData(t2);
        rd.addObjectElementData(t3);
        rd.addObjectElementData(t4);

        model.setReportData(&rd);

        // test initial value
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("idT1"));
        QCOMPARE(model.data(model.index(1, 0)), QVariant("idT2"));

        // test modify id
        t1->setId("idT1New");
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("idT1New"));

        // test add id
        t3->setId("idT3");
        QCOMPARE(model.rowCount(), 3);
        QCOMPARE(model.data(model.index(2, 0)), QVariant("idT3"));

        // test add existing id
        t4->setId("idT2");
        QCOMPARE(model.rowCount(), 3);
        QCOMPARE(model.data(model.index(2, 0)), QVariant("idT3"));

        // test moidy id with existing id
        t4->setId("idT1New");
        QCOMPARE(model.rowCount(), 3);

        t3->setId("idT1New");
        QCOMPARE(model.rowCount(), 2);

        // test delete id
        t2->setId("");
        QCOMPARE(model.rowCount(), 1);
    }

    void testWithHtmlElement()
    {
        IdModel model;
        Report r;

        // Create document
        ReportData rd(r);
        HtmlElement html;
        HtmlElementData *t1 = new HtmlElementData(html);
        t1->setId("idT1");
        rd.addObjectElementData(t1);

        model.setReportData(&rd);

        // test initial value
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("idT1"));

        // test modify id
        t1->setId("idT1New");
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0)), QVariant("idT1New"));
    }
};

QTEST_MAIN(TestIdAssociation)
#include "TestIdAssociation.moc"
