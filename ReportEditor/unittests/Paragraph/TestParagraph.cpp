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
#include "EditorData.h"
#include "ReportTextEdit.h"
#include "structure/ParagraphData.h"
#include "structure/TextElementData.h"
#include "structure/ReportData.h"
#include <QFile>

using namespace KDReports;
namespace KDReports { class Test; }

class KDReports::Test : public QObject {

    Q_OBJECT

private:
    void checkParagraph(ParagraphData* p, QString text)
    {
        QVERIFY(p);
        TextElementData *textElement = qobject_cast<TextElementData*>(p->childrenElements().at(0));
        QVERIFY(textElement);
        QCOMPARE(textElement->text(), text);
    }

private slots:

    void testParagraph()
    {
        EditorData editorData;
        ReportTextEdit editor(&editorData);
        QFile f(":/paragraph2.xml");
        editorData.loadReport(&f);
        editor.setDocument(&editorData.document());
        ReportData *report = editorData.reportData();
        ParagraphData *p1 = qobject_cast<ParagraphData*>(report->childrenElements().at(0));
        checkParagraph(p1, "Paragraph1");
        ParagraphData *p2 = qobject_cast<ParagraphData*>(report->childrenElements().at(1));
        checkParagraph(p2, "Paragraph2");

        // verify option data are the same for p1 and p2
        QCOMPARE(p1->usedOptionData(), p2->usedOptionData());

        editor.changeCursorPosition(report->endPosition());
        QTest::keyClick(&editor, Qt::Key_Return);
        QTest::keyClicks(&editor, "Paragraph3");

        ParagraphData *p3 = qobject_cast<ParagraphData*>(report->childrenElements().at(2));
        checkParagraph(p3, "Paragraph3");


        // verify option data are the same for p1 and p2 and p3
        QCOMPARE(p1->usedOptionData(), p2->usedOptionData());
        QCOMPARE(p1->usedOptionData(), p3->usedOptionData());

        // change margin of p2, then p1 != p2 and p2 = p3
        p2->setMarginBottom(17.0);
        QVERIFY(p1->usedOptionData() != p2->usedOptionData());
        QCOMPARE(p2->usedOptionData(), p3->usedOptionData());

        // insert a paragraph after p1
        // expect p1 = p1a and p1 != p2 and p2 = p3
        editor.changeCursorPosition(p1->endPosition() - 1);
        QTest::keyClick(&editor, Qt::Key_Return);
        QTest::keyClicks(&editor, "Paragraph1a");

        ParagraphData *p1a = qobject_cast<ParagraphData*>(report->childrenElements().at(1));
        checkParagraph(p1a, "Paragraph1a");

        QCOMPARE(p1->usedOptionData(), p1a->usedOptionData());
        QVERIFY(p1->usedOptionData() != p2->usedOptionData());
        QCOMPARE(p2->usedOptionData(), p3->usedOptionData());

        // insert paragraph before p1
        // expect p0 != p1, p1 = p1a, p1!= p2 and p2 =p3
        editor.changeCursorPosition(0);
        QTest::keyClick(&editor, Qt::Key_Return);
        editor.changeCursorPosition(0);
        QTest::keyClicks(&editor, "Paragraph0");

        ParagraphData *p0 = qobject_cast<ParagraphData*>(report->childrenElements().at(0));
        checkParagraph(p0, "Paragraph0");

        QVERIFY(p0->usedOptionData() != p1->usedOptionData());
        QCOMPARE(p1->usedOptionData(), p1a->usedOptionData());
        QVERIFY(p1->usedOptionData() != p2->usedOptionData());
        QCOMPARE(p2->usedOptionData(), p3->usedOptionData());
    }

    void testParagraph2()
    {
        EditorData editorData;
        ReportTextEdit editor(&editorData);
        editorData.newReport();
        editor.setDocument(&editorData.document());
        ReportData *report = editorData.reportData();

        editor.changeCursorPosition(0);
        QTest::keyClicks(&editor, "Paragraph1");
        QTest::keyClick(&editor, Qt::Key_Return);
        QTest::keyClicks(&editor, "Paragraph2");
        QTest::keyClick(&editor, Qt::Key_Return);
        QTest::keyClicks(&editor, "Paragraph3");
        QTest::keyClick(&editor, Qt::Key_Return);

        ParagraphData *p1 = qobject_cast<ParagraphData*>(report->childrenElements().at(0));
        checkParagraph(p1, "Paragraph1");
        ParagraphData *p2 = qobject_cast<ParagraphData*>(report->childrenElements().at(1));
        checkParagraph(p2, "Paragraph2");
        ParagraphData *p3 = qobject_cast<ParagraphData*>(report->childrenElements().at(2));
        checkParagraph(p3, "Paragraph3");

        QVERIFY(p1->ownOptionData());
        QVERIFY(!p2->ownOptionData());
        QVERIFY(!p3->ownOptionData());

        p1->setMarginTop(15.0);
        QVERIFY(p1->ownOptionData());
        QVERIFY(!p2->ownOptionData());
        QVERIFY(!p3->ownOptionData());

        QCOMPARE(p1->marginTop(), 15.0);
        QCOMPARE(p2->marginTop(), 15.0);
        QCOMPARE(p3->marginTop(), 15.0);

        p3->setMarginTop(11.0);
        QVERIFY(p1->ownOptionData());
        QVERIFY(!p2->ownOptionData());
        QVERIFY(p3->ownOptionData());

        QCOMPARE(p1->marginTop(), 15.0);
        QCOMPARE(p2->marginTop(), 15.0);
        QCOMPARE(p3->marginTop(), 11.0);

        p2->setMarginTop(13.0);
        QVERIFY(p1->ownOptionData());
        QVERIFY(p2->ownOptionData());
        QVERIFY(p3->ownOptionData());

        QCOMPARE(p1->marginTop(), 15.0);
        QCOMPARE(p2->marginTop(), 13.0);
        QCOMPARE(p3->marginTop(), 11.0);

        p3->setOwnOptionData(false);
        QVERIFY(p2->ownOptionData());
        QVERIFY(!p3->ownOptionData());

        QCOMPARE(p2->marginTop(), 13.0);
        QCOMPARE(p3->marginTop(), 13.0);
    }

};

QTEST_MAIN(Test)

#include "TestParagraph.moc"
