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
#include <KDReports>
#include <KDReportsTextDocument_p.h>
#include <ReportObjectXmlElementHandler.h>
#include <structure/ReportData.h>
#include <EditorData.h>
#include <ReportStructureModel.h>
#include <structure/VSpaceData.h>
#include <structure/ObjectData.h>
#include <structure/TextElementData.h>
#include <structure/HLineElementData.h>
#include <structure/ParagraphData.h>
#include <structure/ImageElementData.h>
#include <structure/TableElementData.h>
#include <structure/CellData.h>
#include <QUndoStack>
#include <QTextBlock>
#include <QTextTable>
#include <QTextTableCell>

// Help to debug to disable some test easely
#define TEST_HLINE 1
#define TEST_IMAGE 1
#define TEST_TABLE 1
#define TEST_VSPACE 1
#define TEST_MOVE 1

using namespace KDReports;
namespace KDReports { class Test; }

class KDReports::Test : public QObject {
    Q_OBJECT

private:
    EditorData* m_editorData;
    void initReport(const QString& f)
    {
        QFile file( f );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        m_editorData = new EditorData();
        QVERIFY(m_editorData->loadReport(&file));
    }

    void compareTextDocument(const QTextDocument& doc2, const QTextDocument& doc1)
    {
        if (doc1.blockCount() !=  doc2.blockCount()) {
            qDebug() << "actual:" << doc1.toPlainText();
            qDebug() << "excpected:" << doc2.toPlainText();
        }
        QCOMPARE(doc1.blockCount(), doc2.blockCount());
        // Verify bottom margin
        for (int i = 0; i < doc1.blockCount(); i++) {
            QTextBlock b1 = doc1.findBlockByNumber(i);
            QTextBlock b2 = doc2.findBlockByNumber(i);
            QCOMPARE(b1.blockFormat().bottomMargin(), b2.blockFormat().bottomMargin());
        }

        QCOMPARE( doc1.toPlainText(), doc2.toPlainText() );
    }

    void compareObject(ObjectData* o, const QString& className)
    {
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), className);
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

    void testUndoRedo()
    {
        QString structure1;
        createActualStructure(m_editorData->reportData(), structure1);
        while (m_editorData->undoStack()->canUndo()) {
            m_editorData->undoStack()->undo();
        }
        QTextDocument empty;
        compareTextDocument(empty, m_editorData->report()->doc().contentDocument());
        while (m_editorData->undoStack()->canRedo()) {
            m_editorData->undoStack()->redo();
        }
        QString structure2;
        createActualStructure(m_editorData->reportData(), structure2);
        QCOMPARE(structure1, structure2);
    }

    void compareDocument(const QString& structure, const QTextDocument& document
                         , bool cmpUndoRedo = true, bool cmpUpdate = true)
    {
        qDebug() << "compare document";
        compareReportStructure(m_editorData->reportData(), structure);
        compareTextDocument(document, m_editorData->report()->doc().contentDocument());
        if (cmpUndoRedo) {
            qDebug() << "do undo-redo and compare";
            testUndoRedo();
            // Test if document is the same after undo / redo
            compareTextDocument(document, m_editorData->report()->doc().contentDocument());
        }
        // Test update
        if (cmpUpdate) {
            qDebug() << "do update and compare";
            m_editorData->reportData()->update();
            compareReportStructure(m_editorData->reportData(), structure);
            compareTextDocument(document, m_editorData->report()->doc().contentDocument());
        }
    }

private slots:
    void initTestCase()
    {
        m_editorData = 0;
    }

    void cleanup()
    {
        delete m_editorData;
        m_editorData = 0;
    }

    void testAddInlineText()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        ParagraphData *paragraph = new ParagraphData(0);
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        m_editorData->reportData();
        m_editorData->insertSubElement(m_editorData->reportData(), paragraph, 0);
        QString structure = QString::fromLatin1("ReportData[ParagraphData]");
        compareReportStructure(m_editorData->reportData(), structure);
        m_editorData->insertSubElement(paragraph, text, 0);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addInlineElement( elem2 );
        m_editorData->insertSubElement(paragraph, text2, 1);
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
    }

    void testAddText()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        ParagraphData *paragraph = new ParagraphData(0);
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), paragraph, 0));
        QString structure = QString::fromLatin1("ReportData[ParagraphData]");
        QVERIFY(m_editorData->insertSubElement(paragraph, text, 0));
        ParagraphData *paragraph2 = new ParagraphData(0);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), paragraph2, 1));
        QVERIFY(m_editorData->insertSubElement(paragraph2, text2, 1));
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
    }

    void testAddTextInReportWithoutParagraph()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        TextElementData *text = new TextElementData(TextElement("foo"));

        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]]");
        // Report create automatically a paragraph
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        compareDocument(structure, report.doc().contentDocument());
    }

    void testUpdateParagraph()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );
        TextElement elem3( QString::fromLatin1( "word" ) );
        report.addElement( elem3 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        TextElementData *text = new TextElementData("foo");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        TextElementData *text2 = new TextElementData("bar");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text2, 1));
        TextElementData *text3 = new TextElementData("word");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text3, 2));

        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());

        QString expected(report.doc().contentDocument().toPlainText());

        // Update first paragraph
        m_editorData->reportData()->childrenElements().at(0)->update();
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), expected);

        // Update second paragraph
        m_editorData->reportData()->childrenElements().at(1)->update();
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), expected);

        // Update third paragraph
        m_editorData->reportData()->childrenElements().at(2)->update();
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), expected);
    }

#if TEST_HLINE
    void testAddHLine()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        HLineElement elem1;
        report.addElement( elem1 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        HLineElementData *hline = new HLineElementData(HLineElement());

        QString structure = QString::fromLatin1("ReportData[HLineElementData]");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline, 0));
        compareDocument(structure, report.doc().contentDocument());
    }

    void testAddTextBeforeHLine()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        HLineElement elem2;
        report.addElement( elem1 );
        report.addElement( elem2 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        HLineElementData *hline = new HLineElementData(HLineElement());
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline, 0));

        // Create a text before hline
        TextElementData *text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(hline->parentElement(), text, 0));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData]");
        compareDocument(structure, report.doc().contentDocument());
    }

    void testAddTextAfterHLine()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        HLineElement elem1;
        report.addElement( elem1 );
        TextElement elem2( QString::fromLatin1( "foo" ) );
        report.addElement( elem2 );

        // load an empty report
        initReport(":/reportproperty.xml" );
        HLineElementData *hline = new HLineElementData(HLineElement());
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline, 0));
        QString structure = QString::fromLatin1("ReportData[HLineElementData-ParagraphData[TextElementData]]");

        // Create a text after hline
        TextElementData *text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(hline, text, 0));
        compareDocument(structure, report.doc().contentDocument());

        // Test a second hline
        report.addElement(elem1);
        HLineElementData *hline2 = new HLineElementData(HLineElement());
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline2, 2));
        structure = QString::fromLatin1("ReportData[HLineElementData-ParagraphData[TextElementData]-HLineElementData]");
        compareDocument(structure, report.doc().contentDocument());

        // And another text in the first hline
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        QVERIFY(m_editorData->insertSubElement(hline, text2, 0));
        structure = QString::fromLatin1("ReportData[HLineElementData-ParagraphData[TextElementData]-ParagraphData[TextElementData]-HLineElementData]");
        compareReportStructure(m_editorData->reportData(), structure);

        QTextCursor cursor( &report.doc().contentDocument() );
        cursor.setPosition(1);
        cursor.insertBlock();
        cursor.insertText("bar");
        compareDocument(structure, report.doc().contentDocument());
    }

    void testHLineBetweenParagraph()
    {
        initReport(":/hlineText.xml" );
        QString initial(m_editorData->reportData()->textDocumentData().document().toPlainText());
        m_editorData->reportData()->update();
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), initial);
    }

    void testUpdateHLine()
    {
        initReport(":/reportproperty.xml" );
        QString initial(m_editorData->reportData()->textDocumentData().document().toPlainText());
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), initial);
        TextElementData *text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        HLineElementData *hline = new HLineElementData(HLineElement());
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline, 1));

        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        HLineElement elem2;
        report.addElement( elem2 );

        m_editorData->reportData()->childrenElements().at(1)->doUpdate();
        QString structure("ReportData[ParagraphData[TextElementData]-HLineElementData]");
        compareDocument(structure, report.doc().contentDocument());

        HLineElement elem3;
        report.addElement( elem3 );
        HLineElementData *hline2 = new HLineElementData(HLineElement());
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), hline2, 2));

        m_editorData->reportData()->childrenElements().at(1)->doUpdate();
        structure= QString::fromLatin1("ReportData[ParagraphData[TextElementData]-HLineElementData-HLineElementData]");
        compareDocument(structure, report.doc().contentDocument());

    }

    void testUpdateParagraphAfterHline()
    {
        initReport(":/hlineText.xml" );
        QString initial(m_editorData->reportData()->textDocumentData().document().toPlainText());
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), initial);
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "1234" ) );
        report.addElement( elem1 );
        HLineElement elem2;
        report.addElement( elem2 );
        TextElement elem3( QString::fromLatin1( "1234" ) );
        report.addElement( elem3 );

        m_editorData->reportData()->childrenElements().at(2)->doUpdate();
        QString structure("ReportData[ParagraphData[TextElementData]-HLineElementData-ParagraphData[TextElementData]]");
        // Don't do undo redo compare because we don't load an empty document
        compareDocument(structure, report.doc().contentDocument(), false, true);
    }
#endif

#if TEST_IMAGE
    void testAddImage()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        QPixmap pix( 200, 200 );
        ImageElement imageElement(pix);
        report.addElement( imageElement );

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[ParagraphData[ImageElementData]]");
        ImageElementData *image = new ImageElementData(imageElement, "filename.png");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), image, 0));
        compareDocument(structure, report.doc().contentDocument());

        // Create a text after image
        TextElement elem2( QString::fromLatin1( "foo" ) );
        report.addElement( elem2 );
        TextElementData *text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 1));
        structure = QString::fromLatin1("ReportData[ParagraphData[ImageElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());

        // insert inline image
        QPixmap pix2( 100, 100 );
        ImageElement imageElement2(pix2);
        report.addInlineElement( imageElement2 );

        ImageElementData *image2 = new ImageElementData(imageElement2, "filename2.png");
        ParagraphData* p = qobject_cast<ParagraphData*>(text->parentElement());

        QVERIFY(m_editorData->insertSubElement(p, image2, 1));
        structure = QString::fromLatin1("ReportData[ParagraphData[ImageElementData]-ParagraphData[TextElementData-ImageElementData]]");
        compareDocument(structure, report.doc().contentDocument());

        // insert second inline image
        QPixmap pix3( 100, 100 );
        ImageElement imageElement3(pix3);
        report.addInlineElement( imageElement3 );

        ImageElementData *image3 = new ImageElementData(imageElement3, "filename2.png");

        QVERIFY(m_editorData->insertSubElement(p, image3, 2));
        structure = QString::fromLatin1("ReportData[ParagraphData[ImageElementData]-ParagraphData[TextElementData-ImageElementData-ImageElementData]]");
        compareDocument(structure, report.doc().contentDocument());

        // text inline after image
        // Create a text after image
        TextElement elem3( QString::fromLatin1( "bar" ) );
        report.addInlineElement( elem3 );
        TextElementData *text2 = new TextElementData(elem3);
        QVERIFY(m_editorData->insertSubElement(p, text2, 3));
        structure = QString::fromLatin1("ReportData[ParagraphData[ImageElementData]-ParagraphData[TextElementData-ImageElementData-ImageElementData-TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
    }
#endif

#if TEST_VSPACE
    void testAddVSpace()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        report.addVerticalSpacing(10);

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[VSpaceData]");
        VSpaceData* vspace = new VSpaceData(10);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), vspace, 0));
        compareDocument(structure, report.doc().contentDocument());
    }

    void testAdvancedAddVSpace()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        report.addVerticalSpacing(10);

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[VSpaceData]");
        VSpaceData* vspace = new VSpaceData(10);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), vspace, 0));
        compareDocument(structure, report.doc().contentDocument());

        // Create a text after vspace
        TextElement elem( QString::fromLatin1( "foo" ) );
        report.addElement( elem );
        TextElementData *text = new TextElementData(elem);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 1));
        structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());

        // Add vspace after paragraph
        report.addVerticalSpacing(20);
        VSpaceData* vspace2 = new VSpaceData(20);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), vspace2, 2));
        structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]-VSpaceData]");
        qDebug() << report.doc().contentDocument().blockCount();
        compareDocument(structure, report.doc().contentDocument());

        // Add second vspace
        report.addVerticalSpacing(5);
        VSpaceData* vspace3 = new VSpaceData(5);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), vspace3, 3));
        structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]-VSpaceData-VSpaceData]");
        compareDocument(structure, report.doc().contentDocument());

        // Create a text after vspace
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );
        TextElementData *text2 = new TextElementData(elem2);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text2, 4));
        structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]-VSpaceData-VSpaceData-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
    }

    void testVSpaceBeforeFirstParagraph()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        report.addVerticalSpacing(10);
        report.addElement(TextElement("Foo"));

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[VSpaceData-ParagraphData[TextElementData]]");

        // Create a text
        TextElement elem( QString::fromLatin1( "Foo" ) );
        TextElementData *text = new TextElementData(elem);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));

        VSpaceData* vspace = new VSpaceData(10);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), vspace, 0));

        compareDocument(structure, report.doc().contentDocument());
    }
#endif

    void testTextProperty()
    {
        // load an empty report
        initReport(":/reportproperty.xml" );

        // Create a text
        TextElement elem( QString::fromLatin1( "foo" ) );
        TextElementData *text = new TextElementData(elem);
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 1));

        QTextDocument doc;
        QTextCursor cursor(&doc);
        cursor.insertText("foo");

        compareTextDocument(doc, m_editorData->report()->doc().contentDocument());

        // Modify text
        m_editorData->changeProperty(text, "text", QString::fromLatin1("new text"));
        doc.clear();
        cursor.insertText(QString::fromLatin1("new text"));
        compareTextDocument(doc, m_editorData->report()->doc().contentDocument());
    }

#if TEST_TABLE
    void testAddTable()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TableElement table;
        table.cell(0, 0).addElement(TextElement("foo"));
        report.addElement(table);
        TableElementData *tableData = new TableElementData();

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TableElementData[CellData[ParagraphData[TextElementData]]]]]");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), tableData, 0));
        CellData *cell = tableData->cellAt(0, 0);
        TextElementData* text = new TextElementData(TextElement("foo"));

        QVERIFY(m_editorData->insertSubElement(cell,text, 0));
        compareDocument(structure, report.doc().contentDocument());
    }

    void testTableAddTwoText()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TableElement table;
        table.cell(0, 0).addElement(TextElement("foo"));
        table.cell(0, 0).addInlineElement(TextElement(" bar"));
        report.addElement(table);
        TableElementData *tableData = new TableElementData();
        TableElement table2;

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TableElementData[CellData[ParagraphData[TextElementData-TextElementData]]]]]");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), tableData, 0));
        CellData *cell = tableData->cellAt(0, 0);
        TextElementData* text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(cell,text, 0));
        TextElementData* text2 = new TextElementData(TextElement(" bar"));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text2, EditorData::LastChild));

        compareDocument(structure, report.doc().contentDocument());
    }

    void testMoreComplexTable()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TableElement table;
        table.cell(0, 0).addElement(TextElement("row 0"));
        table.cell(0, 0).addInlineElement(TextElement(" col 0"));
        table.cell(0, 1).addElement(TextElement("row 0 col 1"));
        table.cell(1, 0).addElement(TextElement("row 1 col 0"));
        table.cell(1, 1).addElement(TextElement("row 1 col 1"));
        report.addElement(table);
        TableElementData *tableData = new TableElementData();
        TableElement table2;

        // load an empty report
        initReport(":/reportproperty.xml" );
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TableElementData[CellData[ParagraphData[TextElementData-TextElementData]]-CellData[ParagraphData[TextElementData]]-CellData[ParagraphData[TextElementData]]-CellData[ParagraphData[TextElementData]]]]]"); //-CellData[ParagraphData[TextElementData]]-CellData[ParagraphData[TextElementData]]]]]");
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), tableData, 0));
        CellData *cell = tableData->cellAt(0, 0);
        TextElementData* text = new TextElementData(TextElement("row 0"));
        QVERIFY(m_editorData->insertSubElement(cell,text, 0));
        TextElementData* text2 = new TextElementData(TextElement(" col 0"));
        QVERIFY(m_editorData->insertSubElement(text->parentElement(), text2, EditorData::LastChild));
        m_editorData->insertColumn(tableData, 1);
        m_editorData->insertRow(tableData, 1);
        CellData *cell2 = tableData->cellAt(0, 1);
        TextElementData* text3 = new TextElementData(TextElement("row 0 col 1"));
        QVERIFY(m_editorData->insertSubElement(cell2,text3, 0));

        CellData *cell3 = tableData->cellAt(1, 0); //new CellData(tableData, 1, 0);
        TextElementData* text4 = new TextElementData(TextElement("row 1 col 0"));
        QVERIFY(m_editorData->insertSubElement(cell3,text4, 0));

        CellData *cell4 = tableData->cellAt(1, 1);//new CellData(tableData, 1, 1);
        TextElementData* text5 = new TextElementData(TextElement("row 1 col 1"));
        QVERIFY(m_editorData->insertSubElement(cell4,text5, 0));

        compareDocument(structure, report.doc().contentDocument());
    }

    void testRemoveTable()
    {
        // load a table report
        initReport(":/testTable.xml" );
        ReportData* reportData = m_editorData->reportData();
        ObjectData* o = reportData->childrenElements().at(0)->childrenElements().at(0);
        TableElementData* table;
        QVERIFY(table = qobject_cast<TableElementData*>(o));
        QModelIndex reportIdx(m_editorData->model()->index(0, 0, QModelIndex()));
        QModelIndex paragraphIdx(m_editorData->model()->index(0, 0, reportIdx));
        QModelIndex tableIdx(m_editorData->model()->index(0, 0, paragraphIdx));
        m_editorData->changeIndex(tableIdx);
        QTextDocument save(m_editorData->reportData()->textDocumentData().document().toPlainText());
        m_editorData->removeCurrentObject();
        QTextDocument emptyText;
        compareTextDocument(emptyText, m_editorData->report()->doc().contentDocument());
        m_editorData->undoStack()->undo();
        compareTextDocument(save, m_editorData->report()->doc().contentDocument());
        m_editorData->reportData()->update();
        compareTextDocument(save, m_editorData->report()->doc().contentDocument());
        m_editorData->undoStack()->redo();
        compareTextDocument(emptyText, m_editorData->report()->doc().contentDocument());
        m_editorData->reportData()->update();
        compareTextDocument(emptyText, m_editorData->report()->doc().contentDocument());
    }

    void testChangeCellProperty()
    {
        // load a table report
        initReport(":/testTable.xml" );
        ReportData* reportData = m_editorData->reportData();
        const QString defaultData = m_editorData->report()->doc().contentDocument().toPlainText();

        ObjectData* o = reportData->childrenElements().at(0)->childrenElements().at(0)->childrenElements().at(0);
        m_editorData->changeProperty(o, "background", QColor(Qt::green));
        QCOMPARE(m_editorData->report()->doc().contentDocument().toPlainText(), defaultData);
    }

    void testChangeTable()
    {
        initReport(":/testTable.xml" );
        TextElementData *text = new TextElementData(TextElement("foo"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 1));
        QString initial(m_editorData->reportData()->textDocumentData().document().toPlainText());
        ObjectData* o = m_editorData->reportData()->childrenElements().at(0)->childrenElements().at(0);
        m_editorData->changeProperty(o, "background", QColor(Qt::green));
        QCOMPARE(m_editorData->reportData()->textDocumentData().document().toPlainText(), initial);
    }

    void testTableUpdate()
    {
        initReport(":/table.xml");
        QTextCursor c(&m_editorData->reportData()->textDocumentData().document());
        c.setPosition(1);
        QTextTable* table = c.currentTable();
        QCOMPARE(table->cellAt(0, 0).columnSpan(), 3);

        m_editorData->reportData()->update();
        c.setPosition(1);
        table = c.currentTable();
        QCOMPARE(table->cellAt(0, 0).columnSpan(), 3);
    }
#endif

    void testRemoveLastElement()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );
        TextElement elem3( QString::fromLatin1( "test" ) );
        report.addElement( elem3 );
        TextElement elem4( QString::fromLatin1( "word" ) );
        report.addElement( elem4 );

        Report modifiedReport;
        modifiedReport.addElement(elem1);
        modifiedReport.addElement(elem2);
        modifiedReport.addElement(elem3);

        // load an empty report
        initReport(":/reportproperty.xml" );
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        TextElementData *text3 = new TextElementData(TextElement("test"));
        TextElementData *text4 = new TextElementData(TextElement("word"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text2, 1));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text3, 2));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text4, 3));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
        m_editorData->changeIndex(m_editorData->model()->index(text4->parentElement()));
        m_editorData->removeCurrentObject();
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, modifiedReport.doc().contentDocument());
    }

#if TEST_MOVE
    void testMoveElement()
    {
        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );
        TextElement elem3( QString::fromLatin1( "test" ) );
        report.addElement( elem3 );
        TextElement elem4( QString::fromLatin1( "word" ) );
        report.addElement( elem4 );

        Report modifiedReport;
        modifiedReport.addElement(elem1);
        modifiedReport.addElement(elem4);
        modifiedReport.addElement(elem2);
        modifiedReport.addElement(elem3);

        // load an empty report
        initReport(":/reportproperty.xml" );
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        TextElementData *text3 = new TextElementData(TextElement("test"));
        TextElementData *text4 = new TextElementData(TextElement("word"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text2, 1));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text3, 2));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text4, 3));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareDocument(structure, report.doc().contentDocument());
        m_editorData->moveElement(text4->parentElement(), text4->parentElement()->parentElement(), 1);
        compareDocument(structure, modifiedReport.doc().contentDocument());
    }

    void testMoveText()
    {
        initReport(":/reportproperty.xml" );
        TextElementData *text = new TextElementData(TextElement("foo"));
        TextElementData *text2 = new TextElementData(TextElement("bar"));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text, 0));
        QVERIFY(m_editorData->insertSubElement(m_editorData->reportData(), text2, 1));
        QCOMPARE(m_editorData->document().toPlainText(), QString::fromLatin1("foo\nbar"));
        QString structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);

        // Move text2 just after text
        ObjectData* p = text2->parentElement();
        m_editorData->moveElement(text2, text->parentElement(), 1);
        m_editorData->removeObject(p);
        QCOMPARE(m_editorData->document().toPlainText(), QString::fromLatin1("foobar"));
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        m_editorData->undoStack()->undo();
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData-TextElementData]-ParagraphData]");
        compareReportStructure(m_editorData->reportData(), structure);
        m_editorData->undoStack()->undo();
        structure = QString::fromLatin1("ReportData[ParagraphData[TextElementData]-ParagraphData[TextElementData]]");
        compareReportStructure(m_editorData->reportData(), structure);
        QCOMPARE(m_editorData->document().toPlainText(), QString::fromLatin1("foo\nbar"));
    }

#endif

    void testParagraphMarginPropagation()
    {
        initReport(":/paragraph2.xml");
        QList<ObjectData*> childrenList = m_editorData->reportData()->childrenElements();

        // Check if first paragraph margin are the same in the second paragraph
        ObjectData* p1 = childrenList.at(0);
        ObjectData* p2 = childrenList.at(1);

        QVariant left = p1->property("marginLeft");
        QCOMPARE(p2->property("marginLeft"), left);
        // Change p1 property and check if p2 property was changed too
        p1->setProperty("marginLeft", 6);
        QCOMPARE(p1->property("marginLeft"), QVariant(6));
        QCOMPARE(p2->property("marginLeft"), QVariant(6));
        // Change p2 property and check if p1 property was not changed too
        p2->setProperty("marginLeft", 5);
        QCOMPARE(p2->property("marginLeft"), QVariant(5));
        QCOMPARE(p1->property("marginLeft"), QVariant(6));
    }
};

QTEST_MAIN(Test)

#include "TestDocumentModification.moc"
