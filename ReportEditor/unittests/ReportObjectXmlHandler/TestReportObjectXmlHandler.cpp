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
#include <ReportObjectXmlElementHandler.h>
#include <structure/ReportData.h>
#include <structure/HeaderData.h>

using namespace KDReports;

class ReportObjectXmlHandlerTest : public QObject {
    Q_OBJECT
private slots:
    void testStructureSimpleXml()
    {
        QFile file( ":/simple.xml" );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        ReportData* reportData = handler.reportData();
        QList<ObjectData*> childrenList =reportData->childrenElements();
        // The document contains 3 TextElementData, the second one is inline. We expect 2 paragraphs
        QCOMPARE(childrenList.size(), 2);

        // First paragraph
        ObjectData* child = childrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(child->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));
        QCOMPARE(QString::fromLatin1(child->childrenElements().at(1)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // Second paragraph
        child = childrenList.at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(child->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        delete reportData;
    }

    void testStructureHeaderFooter()
    {
        QFile file( ":/headerFooter.xml" );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        ReportData* reportData = handler.reportData();
        QList<ObjectData*> childrenList =reportData->childrenElements();
        // 1 first level children (Text)
        QCOMPARE(childrenList.size(), 1);
        ObjectData* currentChild;
        ObjectData* subChild;

        // Header
        // One header
        QCOMPARE(reportData->headerMap().size(), 1);
        currentChild = reportData->headerMap().values().at(0);
        QCOMPARE(QString::fromLatin1(currentChild->metaObject()->className()), QString::fromLatin1("HeaderData"));
        // Header contains text, text
        QCOMPARE(currentChild->childrenElements().size(), 2);

        //1st paragraph
        subChild = currentChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        subChild = subChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //2nd paragraph
        subChild = currentChild->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        subChild = subChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // Footer
        // One footer
        QCOMPARE(reportData->footerMap().size(), 1);
        currentChild = reportData->footerMap().values().at(0);
        QCOMPARE(QString::fromLatin1(currentChild->metaObject()->className()), QString::fromLatin1("FooterData"));
        // Footer contains text
        QCOMPARE(currentChild->childrenElements().size(), 1);
        subChild = currentChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        subChild = subChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // TextElementData
        currentChild = childrenList.at(0);
        QCOMPARE(QString::fromLatin1(currentChild->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        subChild = currentChild->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(subChild->metaObject()->className()), QString::fromLatin1("TextElementData"));

        delete reportData;
    }

    void testHLine()
    {
        QFile file( ":/hline.xml" );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        ReportData* reportData = handler.reportData();
        QList<ObjectData*> childrenList =reportData->childrenElements();
        // The document contains one hline
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(QString::fromLatin1(childrenList.at(0)->metaObject()->className()), QString::fromLatin1("HLineElementData"));
        delete reportData;
    }

    void testVariable()
    {
        QFile file( ":/variable.xml" );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        ReportData* reportData = handler.reportData();
        // The document contains one header
        // One header
        QCOMPARE(reportData->headerMap().size(), 1);
        QCOMPARE(QString::fromLatin1(reportData->headerMap().values().at(0)->metaObject()->className()), QString::fromLatin1("HeaderData"));
        // The header contains one variable,  the variable is inline but is the first one so we need a paragraph
        QList<ObjectData*> childrenList = reportData->headerMap().values().at(0)->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(QString::fromLatin1(childrenList.at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(childrenList.at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("VariableData"));
        delete reportData;
    }

    void testPriceList()
    {
        QFile file( ":/PriceList.xml" );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        ReportData* reportData = handler.reportData();
        QList<ObjectData*> childrenList =reportData->childrenElements();
        QStringList expectedChildren;

        expectedChildren << "ParagraphData"; // title_element
        expectedChildren << "VSpaceData";
        expectedChildren << "ParagraphData"; // table1_title
        expectedChildren << "ParagraphData"; // table1
        expectedChildren << "VSpaceData";
        expectedChildren << "ParagraphData"; // table2_title
        expectedChildren << "ParagraphData"; // table2
        expectedChildren << "PageBreakData";

        expectedChildren << "ParagraphData"; // table1_title
        expectedChildren << "ParagraphData"; // table1
        expectedChildren << "VSpaceData";
        expectedChildren << "ParagraphData"; // table2_title
        expectedChildren << "ParagraphData"; // table2
        expectedChildren << "VSpaceData";

        expectedChildren << "ParagraphData"; // hr

        expectedChildren << "ParagraphData"; // last table

        QCOMPARE(childrenList.size(), expectedChildren.size());
        for(int i = 0; i < childrenList.size(); i++){
            QCOMPARE(QString::fromLatin1(childrenList.at(i)->metaObject()->className()), expectedChildren.at(i));
        }

        QCOMPARE(reportData->headerMap().size(), 2);
        QCOMPARE(reportData->footerMap().size(), 1);

        QList<ObjectData*> subChildrenList;
        ObjectData* child;
        // Header 1
        subChildrenList = reportData->headerMap().value(HeaderData::OddPages)->childrenElements();
        QCOMPARE(subChildrenList.size(), 5);

        // image and &#x9;
        QCOMPARE(QString::fromLatin1(subChildrenList.at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("ImageElementData"));
        child = subChildrenList.at(0)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // page number and /
        QCOMPARE(QString::fromLatin1(subChildrenList.at(1)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(1)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));
        child = subChildrenList.at(1)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // pagecount and &#x9;Date:
        QCOMPARE(QString::fromLatin1(subChildrenList.at(2)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(2)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));
        child = subChildrenList.at(2)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // textdate and , Time:
        QCOMPARE(QString::fromLatin1(subChildrenList.at(3)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(3)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));
        child = subChildrenList.at(3)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // texttime
        QCOMPARE(QString::fromLatin1(subChildrenList.at(4)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(4)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));

        // Header 2
        subChildrenList = reportData->headerMap().value(HeaderData::EvenPages)->childrenElements();
        QCOMPARE(subChildrenList.size(), 3);

        // image and &#x9 and Even pages header:;
        QCOMPARE(QString::fromLatin1(subChildrenList.at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("ImageElementData"));
        child = subChildrenList.at(0)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));
        child = subChildrenList.at(0)->childrenElements().at(2);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // page number and /
        QCOMPARE(QString::fromLatin1(subChildrenList.at(1)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(1)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));
        child = subChildrenList.at(1)->childrenElements().at(1);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // pagecount
        QCOMPARE(QString::fromLatin1(subChildrenList.at(2)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(2)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("VariableData"));





        //Footer
        subChildrenList = reportData->footerMap().values().at(0)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);

        // company_address
        QCOMPARE(QString::fromLatin1(subChildrenList.at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        child = subChildrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));


        // Document
        // title_element
        subChildrenList = childrenList.at(0)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //vspace
        QCOMPARE(QString::fromLatin1(childrenList.at(1)->metaObject()->className()), QString::fromLatin1("VSpaceData"));

        // table1_title
        subChildrenList = childrenList.at(2)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // table1
        subChildrenList = childrenList.at(3)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("AutoTableElementData"));

        //vspace
        QCOMPARE(QString::fromLatin1(childrenList.at(4)->metaObject()->className()), QString::fromLatin1("VSpaceData"));

        // table2_title
        subChildrenList = childrenList.at(5)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // table2
        subChildrenList = childrenList.at(6)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("AutoTableElementData"));

        //pagebreak
        QCOMPARE(QString::fromLatin1(childrenList.at(7)->metaObject()->className()), QString::fromLatin1("PageBreakData"));

        // table1_title
        subChildrenList = childrenList.at(8)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // table1
        subChildrenList = childrenList.at(9)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("AutoTableElementData"));

        //vspace
        QCOMPARE(QString::fromLatin1(childrenList.at(10)->metaObject()->className()), QString::fromLatin1("VSpaceData"));

        // table2_title
        subChildrenList = childrenList.at(11)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TextElementData"));

        // table2
        subChildrenList = childrenList.at(12)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("AutoTableElementData"));

        //vspace
        QCOMPARE(QString::fromLatin1(childrenList.at(13)->metaObject()->className()), QString::fromLatin1("VSpaceData"));

        // hr
        subChildrenList = childrenList.at(14)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("HtmlElementData"));

        // last table
        subChildrenList = childrenList.at(15)->childrenElements();
        QCOMPARE(subChildrenList.size(), 1);
        child = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(child->metaObject()->className()), QString::fromLatin1("TableElementData"));

        subChildrenList = subChildrenList.at(0)->childrenElements();
        QCOMPARE(subChildrenList.size(), 7);

        ObjectData *cell;
        // 1st cell
        cell = subChildrenList.at(0);
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //2nd
        cell = subChildrenList.at(1);
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 2);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ImageElementData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(1)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //3rd
        cell = subChildrenList.at(2);
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 2);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(1)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //4th
        cell = subChildrenList.at(3);
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //5th
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //6th
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        //7th
        cell = subChildrenList.at(6);
        QCOMPARE(QString::fromLatin1(cell->metaObject()->className()), QString::fromLatin1("CellData"));
        QCOMPARE(cell->childrenElements().size(), 1);
        QCOMPARE(cell->childrenElements().at(0)->childrenElements().size(), 1);
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        QCOMPARE(QString::fromLatin1(cell->childrenElements().at(0)->childrenElements().at(0)->metaObject()->className()), QString::fromLatin1("TextElementData"));

        delete reportData;
    }
};

QTEST_MAIN(ReportObjectXmlHandlerTest)

#include "TestReportObjectXmlHandler.moc"
