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
#include <structure/ParagraphData.h>
#include <structure/CellData.h>
#include <structure/TableElementData.h>
#include <QBuffer>
#include <QMetaProperty>

using namespace KDReports;

class TestXmlSaving : public QObject {
    Q_OBJECT

private:
    void loadSaveCompareReport(QString filename)
    {
        QFile file(filename);
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        Report report;
        ReportObjectXmlElementHandler handler;
        report.setXmlElementHandler(&handler);

        QVERIFY( report.loadFromXML( &file ) );
        file.close();
        ReportData *reportData = handler.reportData();
        QBuffer b;
        b.open(QIODevice::WriteOnly);
        reportData->saveToXml(&b);
        b.close();

        report.setXmlElementHandler(&handler);
        QVERIFY(report.loadFromXML(&b));
        ReportData *reportData2 = handler.reportData();

        compareReport(reportData, reportData2);

        delete reportData;
        delete reportData2;
    }

    void compareReport(ReportData* r1, ReportData* r2)
    {
        // Header
        compareElement(r2->headerMap().value(HeaderData::EvenPages, 0), r1->headerMap().value(HeaderData::EvenPages, 0));
        compareElement(r2->headerMap().value(HeaderData::OddPages, 0), r1->headerMap().value(HeaderData::OddPages, 0));
        compareElement(r2->headerMap().value(HeaderData::FirstPage, 0), r1->headerMap().value(HeaderData::FirstPage, 0));
        compareElement(r2->headerMap().value(HeaderData::LastPage, 0), r1->headerMap().value(HeaderData::LastPage, 0));

        // Footer
        compareElement(r2->footerMap().value(HeaderData::EvenPages, 0), r1->footerMap().value(HeaderData::EvenPages, 0));
        compareElement(r2->footerMap().value(HeaderData::OddPages, 0), r1->footerMap().value(HeaderData::OddPages, 0));
        compareElement(r2->footerMap().value(HeaderData::FirstPage, 0), r1->footerMap().value(HeaderData::FirstPage, 0));
        compareElement(r2->footerMap().value(HeaderData::LastPage, 0), r1->footerMap().value(HeaderData::LastPage, 0));

        compareElement(r1, r2);
    }

    void compareElement(ObjectData* o1, ObjectData* o2)
    {
        // test for header and footer
        if (!o1) {
            QCOMPARE(o1, o2);
            return;
        }
        // comapre object name
        QCOMPARE(o2->objectName(), o1->objectName());


        // compare properties
        for (int i = 0; i < o2->metaObject()->propertyCount() ;i++) {
            QMetaProperty metaProperty = o2->metaObject()->property(i);
            // special case for tabs
            if (qVariantCanConvert<Tabs>(metaProperty.read(o2))) {
                Tabs tabs2 = qVariantValue<Tabs >(metaProperty.read(o2));
                Tabs tabs1 = qVariantValue<Tabs >(metaProperty.read(o1));
                QCOMPARE(tabs2.size(), tabs1.size());
                for (int i = 0; i < tabs2.size(); i++) {
                    QCOMPARE(tabs2.at(i).delimiter, tabs1.at(i).delimiter);
                    QCOMPARE(tabs2.at(i).position, tabs1.at(i).position);
                    QCOMPARE(tabs2.at(i).type, tabs1.at(i).type);

                }
            } else if (qVariantCanConvert<QBrush>(metaProperty.read(o2))) {
                QBrush b2 = qVariantValue<QBrush>(metaProperty.read(o2));
                QBrush b1 = qVariantValue<QBrush>(metaProperty.read(o1));
                // qDebug() << b2 << b1;
                QCOMPARE(b2, b1);
            } else {
               // qDebug() << o2->objectName() << metaProperty.name();
                QCOMPARE(metaProperty.read(o2), metaProperty.read(o1));
            }
        }

        // compare childs
        QCOMPARE(o2->childrenElements().size(), o1->childrenElements().size());
        if (o2->objectName() == "table") {
            for (int i = 0; i < o1->childrenElements().size(); i++) {
                CellData *cell2 = qobject_cast<CellData *>(o2->childrenElements().at(i));
                TableElementData *table1 = qobject_cast<TableElementData*>(o1);

                CellData *cell1 = table1->cellAt(cell2->row(), cell2->column());
                // qDebug() << cell2->row() << cell2->column();
                compareElement(cell2, cell1);
            }
        } else {
            for (int i = 0; i < o1->childrenElements().size(); i++) {
                compareElement(o2->childrenElements().at(i), o1->childrenElements().at(i));
            }
        }
    }

private slots:
    void initTestCase()
    {
        qRegisterMetaType<Tabs>("Tabs");
    }

    void testSaveEmptyReport()
    {
        loadSaveCompareReport(":/reportproperty.xml" );
    }

    void testSaveAutotable()
    {
        loadSaveCompareReport(":/autotable.xml" );
    }

    void testSaveChart()
    {
        loadSaveCompareReport(":/chart.xml" );
    }

    void testSaveFooter()
    {
        loadSaveCompareReport(":/footer.xml" );
    }

    void testSaveHeaderFooter()
    {
        loadSaveCompareReport(":/headerFooter.xml" );
    }

    void testSaveHLine()
    {
        loadSaveCompareReport(":/hline.xml" );
    }

    void testSaveHLineText()
    {
        loadSaveCompareReport(":/hlineText.xml" );
    }

    void testSaveHtml()
    {
        loadSaveCompareReport(":/html.xml" );
    }

    void testSaveImage()
    {
        loadSaveCompareReport(":/image.xml" );
    }

    void testSaveParagraph()
    {
        loadSaveCompareReport(":/paragraph2.xml" );
    }

    void testSaveSimple()
    {
        loadSaveCompareReport(":/simple.xml" );
    }

    void testSaveSimpleTextElement()
    {
        loadSaveCompareReport(":/simpletextelement.xml" );
    }

    void testSaveTable()
    {
        loadSaveCompareReport(":/table.xml" );
    }

    void testSaveTextElement()
    {
        loadSaveCompareReport(":/textelement.xml" );
    }

    void testSaveVariable()
    {
        loadSaveCompareReport(":/variable.xml" );
    }

    void testSaveVSpace()
    {
        loadSaveCompareReport(":/vspace.xml" );
    }

    void testParagraphAndSimpleTable()
    {
        loadSaveCompareReport(":/paragraph.xml" );
    }
};

QTEST_MAIN(TestXmlSaving)

#include "TestXmlSaving.moc"
