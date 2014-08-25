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
#include <QDebug>
#include <KDReports>
#include <KDReportsTextDocument_p.h>
#include <ReportTextEdit.h>
#include <EditorData.h>
#include <structure/ObjectData.h>
#include <structure/ReportData.h>
#include <structure/TextElementData.h>
#include <structure/HLineElementData.h>
#include <structure/ImageElementData.h>
#include <structure/VSpaceData.h>
#include <QTextBlock>


static void initResource() { //needed for static builds
#ifdef KDREPORTS_STATICLIB
    Q_INIT_RESOURCE(ReportViewer);
#endif
}

using namespace KDReports;
namespace KDReports { class Test; }

class KDReports::Test : public QObject {
    Q_OBJECT

private:
    ReportTextEdit* m_editor;
    EditorData* m_editorData;
    void loadReport(const QString& f)
    {
        QFile file( f );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        QVERIFY(m_editorData->loadReport(&file));
        m_editor->cleanSelection();
        m_editor->setDocument(&m_editorData->document());
    }

    void newReport()
    {
        m_editorData->newReport();
        m_editor->setDocument(&m_editorData->document());
    }

    void createActualStructure(ObjectData *o, QString &s)
    {
        s.append(QString::fromLatin1(o->metaObject()->className()));
        if(o->childrenElements().isEmpty())
            return;
        for (int i = 0; i < o->childrenElements().size();i++) {
            if (i==0) {
                s.append("[");
            }
            createActualStructure(o->childrenElements().at(i), s);
            if (i==o->childrenElements().size()-1){
                s.append("]");
            } else {
                s.append("-");
            }
        }
    }

    void compareReportStructure(ReportData* report, QString structure)
    {
        QString actualStructure;
        createActualStructure(report, actualStructure);
        QCOMPARE(actualStructure, structure);
    }

private slots:
    void initTestCase()
    {
        initResource();
        m_editor = 0;
        m_editorData = 0;
    }

    void init()
    {
        m_editorData = new EditorData();
        m_editor = new ReportTextEdit(m_editorData);
    }

    void cleanup()
    {
        delete m_editor;
        m_editor = 0;
        delete m_editorData;
        m_editorData = 0;
    }

    void testInsertTextInEmptyDocument()
    {
        newReport();
        QTest::keyClick(m_editor, Qt::Key_A);
        // Need to have text 'a' and cursor position == 1.
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("a"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        // Compare the report structure
        QString structure("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testRemoveWithBackspace()
    {
        loadReport(":/simpletextelement.xml");
        // Test current structure
        QString structure("ReportData[ParagraphData[TextElementData]]");
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcde"));

        // Remove the last char
        m_editor->moveCursor(QTextCursor::End);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("abcd"));
        QCOMPARE(m_editor->textCursor().position(), 4);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcd"));

        // Remove the first char
        m_editor->changeCursorPosition(1);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("bcd"));
        QCOMPARE(m_editor->textCursor().position(), 0);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("bcd"));

        // Remove char in the middle
        m_editor->changeCursorPosition(2);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("bd"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("bd"));
    }

    void testRemoveWithDelete()
    {
        loadReport(":/simpletextelement.xml");
        // Test current structure
        QString structure("ReportData[ParagraphData[TextElementData]]");
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcde"));

        // Remove the last char
        m_editor->changeCursorPosition(4);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("abcd"));
        QCOMPARE(m_editor->textCursor().position(), 4);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcd"));

        // Remove the first char
        m_editor->changeCursorPosition(0);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("bcd"));
        QCOMPARE(m_editor->textCursor().position(), 0);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("bcd"));

        // Remove char in the middle
        m_editor->changeCursorPosition(1);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("bd"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("bd"));
    }

    void testInsertingChar()
    {
        loadReport(":/simpletextelement.xml");
        // Test current structure
        QString structure("ReportData[ParagraphData[TextElementData]]");
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcde"));

        // Insert char after the element
        m_editor->moveCursor(QTextCursor::End);
        QTest::keyClick(m_editor, Qt::Key_F);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("abcdef"));
        QCOMPARE(m_editor->textCursor().position(), 6);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("abcdef"));

        // Insert char before the element
        m_editor->moveCursor(QTextCursor::Start);
        QTest::keyClick(m_editor, Qt::Key_Z);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("zabcdef"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("zabcdef"));

        // Insert char in the middle of the element
        m_editor->changeCursorPosition(4);
        QTest::keyClick(m_editor, Qt::Key_Y);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("zabcydef"));
        QCOMPARE(m_editor->textCursor().position(), 5);
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("zabcydef"));
    }

    void testInsertingTextBeforeAndAfterHline()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "a" ) );
        HLineElement elem2;
        report.addElement( elem1 );
        report.addElement( elem2 );

        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Insert char before hline
        m_editor->changeCursorPosition(0);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
        QCOMPARE(m_editor->textCursor().position(), 1);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("a"));

        report.addElement(elem1);
        // Insert char after hline
        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
        QCOMPARE(m_editor->textCursor().position(), 5);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
         o = m_editorData->reportData()->childrenElements().at(2)->childrenElements().at(0);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("a"));
    }

    void testInsertingParagraphBetweenTwoParagraph()
    {
        newReport();
        // Creating first paragraph
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("a"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        // Compare the report structure
        QString structure("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Creating second paragraph
        QTest::keyClick(m_editor, Qt::Key_Return);
        QTest::keyClick(m_editor, Qt::Key_C);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("a\nc"));

        // Inserting a paragraph between the first and second
        m_editor->changeCursorPosition(1);
        QTest::keyClick(m_editor, Qt::Key_Return);
        QTest::keyClick(m_editor, Qt::Key_B);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("a\nb\nc"));
    }

    void testInsertingTwoTextBeforeHline()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "a" ) );
        TextElement elem2( QString::fromLatin1( "b" ) );
        HLineElement elem3;
        report.addElement( elem1 );
        report.addElement( elem3 );

        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Insert char before hline
        m_editor->changeCursorPosition(0);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
        QCOMPARE(m_editor->textCursor().position(), 1);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("a"));

        Report report2;
        QCOMPARE( report2.doc().contentDocument().blockCount(), 1 );
        report2.addElement( elem1 );
        report2.addElement( elem2 );
        report2.addElement( elem3 );
        // Insert char before hline
        m_editor->changeCursorPosition(1);
        QTest::keyClick(m_editor, Qt::Key_Return);
        QTest::keyClick(m_editor, Qt::Key_B);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), report2.doc().contentDocument().toPlainText());
        QCOMPARE(m_editor->textCursor().position(), 3);
        o = m_editorData->reportData()->childrenElements().at(1)->childrenElements().at(0);
        QCOMPARE(o->property("text").toString(), QString::fromLatin1("b"));
    }

    void testDeletingHLineWithBackspace()
    {
        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Remove hline with backspace
        m_editor->changeCursorPosition(1);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1(""));
        QCOMPARE(m_editor->textCursor().position(), 0);
        structure = QString::fromLatin1("ReportData");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testDeletingHLineWithDeleteKey()
    {
        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Remove hline with delete key
        m_editor->changeCursorPosition(0);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1(""));
        QCOMPARE(m_editor->textCursor().position(), 0);
        structure = QString::fromLatin1("ReportData");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testDeletingHLineAfterParagraph()
    {
        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        m_editor->changeCursorPosition(0);
        QTest::keyClick(m_editor, Qt::Key_A);

        // Remove hline with backspace
        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("a"));
        QCOMPARE(m_editor->textCursor().position(), 1);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testInsertingTextBetweenTwoText()
    {
        newReport();
        TextElementData* t1 = new TextElementData("foo");
        TextElementData* t2 = new TextElementData("bar");
        m_editorData->insertSubElement(m_editorData->reportData(), t1, 0);
        m_editorData->insertSubElement(t1->parentElement(), t2, 1);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));

        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("fooabar"));
        QCOMPARE(t1->property("text").toString(), QString::fromLatin1("fooa"));
        QCOMPARE(t2->property("text").toString(), QString::fromLatin1("bar"));
    }

    void testInsertingTextBetweenTextAndImage()
    {
        newReport();
        TextElementData* text = new TextElementData("foo");
        QPixmap pix( 200, 200 );
        ImageElement imageElement(pix);
        ImageElementData* image = new ImageElementData(imageElement, QString::fromLatin1("file.png"));
        m_editorData->insertSubElement(m_editorData->reportData(), text, 0);
        m_editorData->insertSubElement(text->parentElement(), image, 1);

        Report report;
        report.addElement(KDReports::TextElement("foo"));
        report.addInlineElement(imageElement);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());

        Report report2;
        report2.addElement(KDReports::TextElement("fooa"));
        report2.addInlineElement(imageElement);
        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), report2.doc().contentDocument().toPlainText());
        QCOMPARE(text->property("text").toString(), QString::fromLatin1("fooa"));
    }

    void testInsertingTextAfterTextId()
    {
        newReport();
        TextElementData* text = new TextElementData("foo");
        text->setId("id");
        m_editorData->insertSubElement(m_editorData->reportData(), text, 0);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("{id}"));

        m_editor->changeCursorPosition(4);
        QTest::keyClick(m_editor, Qt::Key_A);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("{id}a"));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testRemoveSelectionInsideAText()
    {
        newReport();
        TextElementData* text = new TextElementData("foobar");
        m_editorData->insertSubElement(m_editorData->reportData(), text, 0);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));
        QCOMPARE(text->property("text").toString(), QString::fromLatin1("foobar"));

        m_editor->changeCursorPosition(2);
        m_editor->changeCursorPosition(4, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foar"));
        QCOMPARE(text->property("text").toString(), QString::fromLatin1("foar"));
    }

    void testRemoveSelectedText()
    {
        newReport();
        TextElementData* text1 = new TextElementData("foo");
        TextElementData* text2 = new TextElementData("bar");
        TextElementData* text3 = new TextElementData("word");
        m_editorData->insertSubElement(m_editorData->reportData(), text1, 0);
        m_editorData->insertSubElement(m_editorData->reportData(), text2, 1);
        m_editorData->insertSubElement(m_editorData->reportData(), text3, 2);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo\nbar\nword"));

        m_editor->changeCursorPosition(text2->startPosition());
        m_editor->changeCursorPosition(text2->endPosition(), QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo\n\nword"));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        m_editor->changeCursorPosition(0);
        m_editor->changeCursorPosition(text1->endPosition(), QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("\n\nword"));
        structure = QString::fromLatin1("ReportData[ParagraphData-ParagraphData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        m_editor->changeCursorPosition(text3->startPosition());
        m_editor->changeCursorPosition(text3->endPosition(), QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("\n\n"));
        structure = QString::fromLatin1("ReportData[ParagraphData-ParagraphData-ParagraphData]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testRemoveSelectedHLine()
    {
        loadReport(":/hline.xml");
        // Test current structure
        QString structure("ReportData[HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Remove hline with backspace
        m_editor->changeCursorPosition(0);
        m_editor->changeCursorPosition(1, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1(""));
        QCOMPARE(m_editor->textCursor().position(), 0);
        structure = QString::fromLatin1("ReportData");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testRemoveImage()
    {
        loadReport(":/image.xml");
        // Test current structure
        QString structure("ReportData[ParagraphData[ImageElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Remove hline with backspace
        m_editor->changeCursorPosition(0);
        m_editor->changeCursorPosition(1, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1(""));
        QCOMPARE(m_editor->textCursor().position(), 0);
        structure = QString::fromLatin1("ReportData[ParagraphData]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testToCollapseTwoParagraphWithBackspace()
    {
        newReport();
        QTest::keyClicks(m_editor, "foo");
        QTest::keyClick(m_editor, Qt::Key_Return);
        QTest::keyClicks(m_editor, "bar");

        QString structure("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        m_editor->changeCursorPosition(4);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));
        QCOMPARE(m_editor->textCursor().position(), 3);
    }

    void testToCollapseTwoParagraphWithDelete()
    {
        newReport();
        QTest::keyClicks(m_editor, "foo");
        QTest::keyClick(m_editor, Qt::Key_Return);
        QTest::keyClicks(m_editor, "bar");

        QString structure("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));
        QCOMPARE(m_editor->textCursor().position(), 3);
    }

    void testDeleteTwoTextInline()
    {
        newReport();
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        TextElementData *text3 = new TextElementData(TextElement("test"));
        TextElementData *text4 = new TextElementData(TextElement("word"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text2, 1));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text3, 2));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text4, 1));
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobartest\nword"));

        m_editor->changeCursorPosition(1);
        m_editor->changeCursorPosition(8, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Delete);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("fst\nword"));
        QCOMPARE(m_editor->textCursor().position(), 1);
    }

    void testDeleteOneCharText()
    {
        newReport();
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("."));
        TextElementData *text3 = new TextElementData(TextElement("bar"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text2, 1));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text3, 2));
        QString structure("ReportData[ParagraphData[TextElementData-TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo.bar"));
        m_editor->changeCursorPosition(4);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void testDeletingMultipleObject()
    {
        loadReport(":/hline.xml");
        QTest::keyClicks(m_editor, "foo");
        m_editor->changeCursorPosition(5);
        QTest::keyClicks(m_editor, "bar");
        QString structure("ReportData[ParagraphData[TextElementData]-HLineElementData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        m_editor->changeCursorPosition(1);
        m_editor->changeCursorPosition(8, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("fr"));
    }

    void testInsertTextObject()
    {
        newReport();
        TextElementData *text = new TextElementData("foo");
        text->addToHierarchy(m_editorData->reportData(), 0, m_editorData);
        QString structure("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo"));

        TextElementData *text2 = new TextElementData("bar");
        text2->addToHierarchy(text, 0, m_editorData);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("barfoo"));

        TextElementData *text3 = new TextElementData("word");
        text3->addToHierarchy(text2, 3, m_editorData);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("barwordfoo"));

        // Create new paragraph
        m_editor->moveCursor(QTextCursor::End);
        QTest::keyClick(m_editor, Qt::Key_Return);
        TextElementData *text4 = new TextElementData("test");
        text4->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData-TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("barwordfoo\ntest"));
    }

    void testInsertHLine()
    {
        newReport();
        QTest::keyClicks(m_editor, "foo");
        m_editor->changeCursorPosition(0);
        HLineElementData* hline = new HLineElementData(KDReports::HLineElement());
        hline->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        QString structure("ReportData[HLineElementData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        Report report;
        report.addElement(KDReports::HLineElement());
        report.addElement(KDReports::TextElement("foo"));

        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());

        // Add HLine after the text
        report.addElement(KDReports::HLineElement());
        m_editor->changeCursorPosition(5);
        HLineElementData* hline2 = new HLineElementData(KDReports::HLineElement());
        hline2->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        structure = QString::fromLatin1("ReportData[HLineElementData-ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
    }

    void addTextAfterHline()
    {
        newReport();
        // Add text after hline
        Report report;
        report.addElement(KDReports::HLineElement());
        report.addElement(KDReports::TextElement("bar"));
        HLineElementData* hline = new HLineElementData(KDReports::HLineElement());
        hline->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        m_editor->changeCursorPosition(1);
        TextElementData *text = new TextElementData("bar");
        text->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        QString structure("ReportData[HLineElementData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
    }

    void addTextBeforeHline()
    {
        newReport();
        // Add text before hline
        Report report;
        report.addElement(KDReports::TextElement("foo"));
        report.addElement(KDReports::HLineElement());
        HLineElementData* hline = new HLineElementData(KDReports::HLineElement());
        hline->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        m_editor->changeCursorPosition(0);
        TextElementData *text = new TextElementData("foo");
        text->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        QString structure("ReportData[ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), report.doc().contentDocument().toPlainText());
    }

    void insertTextElementInsideExistingText()
    {
        newReport();
        QTest::keyClicks(m_editor, "foobar");
        QString structure("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));

        m_editor->changeCursorPosition(3);
        TextElementData *text = new TextElementData("new");
        text->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foonewbar"));
    }

    void insertHLineElementInsideExistingText()
    {
        newReport();
        QTest::keyClicks(m_editor, "foobar");
        QString structure("ReportData[ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobar"));

        Report report;
        report.addElement(KDReports::TextElement("foo"));
        report.addElement(KDReports::HLineElement());
        report.addElement(KDReports::TextElement("bar"));

        m_editor->changeCursorPosition(3);
        HLineElementData* hline = new HLineElementData(KDReports::HLineElement());
        hline->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
    }

    void textId()
    {
        newReport();
        QTest::keyClicks(m_editor, "foo");
        m_editor->changeCursorPosition(3);
        TextElementData *t1 = new TextElementData("bar");
        t1->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        m_editor->changeCursorPosition(6);
        TextElementData *t2 = new TextElementData("word");
        t2->addToHierarchy(m_editorData->selectedObject(), m_editor->textCursor().position(), m_editorData);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobarword"));

        // Add id to t1
        m_editorData->changeProperty(t1, "id", QString("idbarid"));
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo{idbarid}word"));

        // remove a char in the id
        m_editor->changeCursorPosition(7);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo{idarid}word"));
        // Check if text is still ok and if id property too
        QCOMPARE(t1->text(), QString::fromLatin1("bar"));
        QCOMPARE(t1->id(), QString::fromLatin1("idarid"));
        // Check cursor position
        QCOMPARE(m_editor->textCursor().position(), 6);

        // remove a selection of the id
        m_editor->changeCursorPosition(8, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo{idid}word"));
        // Check if text is still ok and if id property too
        QCOMPARE(t1->text(), QString::fromLatin1("bar"));
        QCOMPARE(t1->id(), QString::fromLatin1("idid"));
        // Check cursor position
        QCOMPARE(m_editor->textCursor().position(), 6);

        // remove id
        m_editor->changeCursorPosition(4);
        m_editor->changeCursorPosition(8, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(t1->text(), QString::fromLatin1("bar"));
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foobarword"));
        // Check if text is still ok and if id property too
        QCOMPARE(t1->id(), QString::fromLatin1(""));
        // Check cursor position
        QCOMPARE(m_editor->textCursor().position(), 3);

        // Add id to t1
        m_editorData->changeProperty(t1, "id", QString("idbarid"));
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo{idbarid}word"));
        // Change t1 text
        m_editorData->changeProperty(t1, "text", QString(""));
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo{idbarid}word"));
        m_editor->changeCursorPosition(4);
        m_editor->changeCursorPosition(11, QTextCursor::KeepAnchor);
        QTest::keyClick(m_editor, Qt::Key_Backspace);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("fooword"));
        QString structure("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->textCursor().position(), 3);
    }

    void testLineReturnInTextElement()
    {
        newReport();
        QTest::keyClicks(m_editor, "foobar");
        m_editor->changeCursorPosition(3);
        QTest::keyClick(m_editor, Qt::Key_Return);
        QString structure("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->document()->toPlainText(), QString::fromLatin1("foo\nbar"));
        QCOMPARE(m_editor->textCursor().position(), 4);
    }

    void testVSpace()
    {
        newReport();
        m_editorData->insertSubElement(m_editorData->reportData(), new VSpaceData(10), 0);
        QString structure("ReportData[VSpaceData]");
        compareReportStructure(m_editorData->reportData(), structure);

        QTest::keyClicks(m_editor, "foo");
        structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editor->textCursor().position(), 4);
    }
};

QTEST_MAIN(Test)

#include "TestReportTextEditor.moc"
