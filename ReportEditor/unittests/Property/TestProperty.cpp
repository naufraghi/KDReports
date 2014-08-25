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
#include <structure/VSpaceData.h>
#include "KDReportsVariableType.h"
#include "structure/ParagraphData.h"

using namespace KDReports;

class TestProperty : public QObject {
    Q_OBJECT
private:
    ReportData* m_reportData;
    Report* m_report;
    void initReport(const QString& f)
    {
        QFile file( f );
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        m_report = new Report();
        ReportObjectXmlElementHandler handler;
        m_report->setXmlElementHandler(&handler);
        QVERIFY( m_report->loadFromXML( &file ) );
        m_reportData = handler.reportData();
    }

    void testProperty(ObjectData* o, const QString& propertyName, const QVariant& defaultValue, const QVariant& testValue)
    {
        qDebug() << "property:" << propertyName;

        QCOMPARE(o->property(propertyName.toLatin1()), QVariant(defaultValue));
        // change value
        o->setProperty(propertyName.toLatin1(), QVariant(testValue));
        // test new property value
        QCOMPARE(o->property(propertyName.toLatin1()), QVariant(testValue));
    }

private slots:
    void init()
    {
    }

    void initTestCase()
    {
        m_reportData = 0;
        m_report = 0;
    }

    void cleanup()
    {
        delete m_reportData;
        delete m_report;
    }

    void testVSpaceProperty()
    {
        initReport(":/vspace.xml" );

        QList<ObjectData*> childrenList = m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        ObjectData* o = childrenList.at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("VSpaceData"));
        testProperty(o, QString("size"), QVariant(10), QVariant(20));
    }


    void testVariableProperty()
    {
        initReport(":/variable.xml" );

        QCOMPARE(m_reportData->headerMap().size(), 1);
        QCOMPARE(m_reportData->headerMap().values().at(0)->childrenElements().size(), 1);
        QCOMPARE(m_reportData->headerMap().values().at(0)->childrenElements().at(0)->childrenElements().size(), 1);
        ObjectData* o = m_reportData->headerMap().values().at(0)->childrenElements().at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("VariableData"));

        testProperty(o, QString("type"), QVariant(KDReports::PageNumber), QVariant(KDReports::PageCount));
    }

    void testReportProperty()
    {
        initReport(":/reportproperty.xml" );
        testProperty(m_reportData, QString("headerBodySpacing"), QVariant(10), QVariant(8));
        testProperty(m_reportData, QString("footerBodySpacing"), QVariant(10), QVariant(15));
        QFont defaultFont;
        defaultFont.setPointSizeF(12);
        defaultFont.setFamily("Arial");
        QFont testFont;
        testFont.setPointSizeF(20);
        testFont.setFamily("Times");
        testProperty(m_reportData, QString("defaultFont"), QVariant(defaultFont), QVariant(testFont));
        testProperty(m_reportData, QString("marginTop"), QVariant(15), QVariant(30));
        testProperty(m_reportData, QString("marginRight"), QVariant(15), QVariant(30));
        testProperty(m_reportData, QString("marginLeft"), QVariant(15), QVariant(30));
        testProperty(m_reportData, QString("marginBottom"), QVariant(15), QVariant(30));
    }

    void testTextElementProperty()
    {
        initReport(":/textelement.xml" );
        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("TextElementData"));
        QFont defaultFont;
        defaultFont.setPointSizeF(12);
        defaultFont.setFamily("Arial");
        defaultFont.setWeight(QFont::Bold);
        defaultFont.setItalic(true);
        defaultFont.setStrikeOut(true);
        defaultFont.setUnderline(true);
        QFont testFont;
        testFont.setPointSizeF(20);
        testFont.setFamily("Times");
        testFont.setWeight(QFont::Normal);
        testProperty(o, QString("id"), QVariant("id_text_xml"), QVariant("id_test"));
        testProperty(o, QString("background"), QColor(QLatin1String("#CCCCFF")), QColor(Qt::blue));
        testProperty(o, QString("font"), QVariant(defaultFont), QVariant(testFont));
        testProperty(o, QString("textColor"), QColor(QLatin1String("red")), QColor(Qt::blue));
        testProperty(o, QString("text"), QString("Page "), QString("Only for test"));
        testProperty(o, QString("modelKey"), QString("myModel"), QString("newModel"));
        testProperty(o, QString("modelRow"), QVariant("2"), QVariant("9"));
        testProperty(o, QString("modelColumn"), QVariant("1"), QVariant("7"));
    }

    void testHeaderProperty()
    {
        initReport(":/variable.xml" );

        QCOMPARE(m_reportData->headerMap().size(), 1);
        ObjectData* o = m_reportData->headerMap().values().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("HeaderData"));

        QFont defaultFont;
        defaultFont.setPointSizeF(12);
        defaultFont.setFamily("Arial");
        QFont testFont;
        testFont.setPointSizeF(20);
        testFont.setFamily("Times");
        testProperty(o, QString("font"), QVariant(defaultFont), QVariant(testFont));
        testProperty(o, QString("location"), QVariant(KDReports::AllPages), QVariant(KDReports::EvenPages));
    }

    void testFooterProperty()
    {
        initReport(":/footer.xml" );

        QCOMPARE(m_reportData->footerMap().size(), 1);
        ObjectData* o = m_reportData->footerMap().values().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("FooterData"));

        QFont defaultFont;
        defaultFont.setPointSizeF(12);
        defaultFont.setFamily("Arial");
        QFont testFont;
        testFont.setPointSizeF(20);
        testFont.setFamily("Times");
        testProperty(o, QString("font"), QVariant(defaultFont), QVariant(testFont));
        testProperty(o, QString("location"), QVariant(KDReports::OddPages), QVariant(KDReports::EvenPages));
    }

    void testHtmlElementProperty()
    {
        initReport(":/html.xml" );
        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("HtmlElementData"));
        testProperty(o, QString("id"), QVariant("id_text_xml"), QVariant("id_test"));
        testProperty(o, QString("background"), QColor(QLatin1String("#CCCCFF")), QColor(Qt::blue));
        testProperty(o, QString("html"), QString("Test html properties"), QString("Only for test"));
    }

    void testImageElementProperty()
    {
        initReport(":/image.xml" );
        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ImageElementData"));
        testProperty(o, QString("id"), QVariant("id_image_xml"), QVariant("id_test"));
        testProperty(o, QString("width"), QVariant(200), QVariant(300));
        testProperty(o, QString("height"), QVariant(0), QVariant(150));
        testProperty(o, QString("fitToPage"), false, true);
        testProperty(o, QString("width"), QVariant(0), QVariant(600));
    }

    void testTableProperty()
    {
        initReport(":/table.xml" );
        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        // table property
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("TableElementData"));
        testProperty(o, QString("width"), QVariant(100), QVariant(300));
        testProperty(o, QString("background"), QColor(QLatin1String("red")), QColor(Qt::blue));
        testProperty(o, QString("headerRowCount"), QVariant(2), QVariant(1));
        testProperty(o, QString("cellpadding"), QVariant(3), QVariant(4));
        testProperty(o, QString("border"), QVariant(2), QVariant(0.5));

        // cell property
        QCOMPARE(childrenList.at(0)->childrenElements().at(0)->childrenElements().size(), 23);
        ObjectData* c = childrenList.at(0)->childrenElements().at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(c->metaObject()->className()), QString::fromLatin1("CellData"));
        testProperty(c, QString("background"), QColor(QLatin1String("yellow")), QColor(Qt::blue));
        testProperty(c, QString("columnSpan"), QVariant(3), QVariant(4));
        testProperty(c, QString("columnSpan"), QVariant(4), QVariant(2));
        testProperty(c, QString("columnSpan"), QVariant(2), QVariant(3));
        testProperty(c, QString("rowSpan"), QVariant(1), QVariant(3));
        testProperty(c, QString("rowSpan"), QVariant(3), QVariant(2));
        testProperty(c, QString("rowSpan"), QVariant(2), QVariant(4));

    }

    void testAutoTableProperty()
    {
        initReport(":/autotable.xml" );
        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("AutoTableElementData"));
        testProperty(o, QString("width"), QVariant(100), QVariant(300));
        testProperty(o, QString("background"), QColor(QLatin1String("red")), QColor(Qt::blue));
        testProperty(o, QString("border"), QVariant(2), QVariant(0.5));
        testProperty(o, QString("modelKey"), QVariant("table_model"), QVariant("new_table_model"));
        testProperty(o, QString("headerBackground"), QColor(QLatin1String("green")), QColor(Qt::gray));
        testProperty(o, QString("verticalHeaderVisible"), true, false);
        testProperty(o, QString("horizontalHeaderVisible"), true, false);
    }

    void testHLineProperty()
    {
        initReport(":/hline.xml" );

        QList<ObjectData*> childrenList = m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        ObjectData* o = childrenList.at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("HLineElementData"));
        testProperty(o, QString("thickness"), QVariant(3), QVariant(5));
        testProperty(o, QString("margin"), QVariant(10), QVariant(20));
        testProperty(o, QString("color"), QColor(QLatin1String("blue")), QColor(Qt::green));
    }

    void testChartProperty()
    {
        initReport(":/chart.xml" );

        QList<ObjectData*> childrenList =  m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 1);
        QCOMPARE(childrenList.at(0)->childrenElements().size(), 1);
        ObjectData* o = childrenList.at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ChartElementData"));
        testProperty(o, QString("modelKey"), QVariant("myChart"), QVariant("anotherChart"));
        testProperty(o, QString("size"), QSizeF(150, 200), QSizeF(200, 300));
        testProperty(o, QString("background"), QColor(QLatin1String("green")), QColor(Qt::red));
    }

    void testParagraphProperty()
    {
        initReport(":/paragraph.xml" );
        QList<ObjectData*> childrenList = m_reportData->childrenElements();
        QCOMPARE(childrenList.size(), 7);
        // first paragraph
        ObjectData* o = childrenList.at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        Tabs t;
        QTextOption::Tab tab;
        tab.type = QTextOption::CenterTab;
        tab.delimiter = QChar::fromLatin1( 'P' );
        tab.position = -1;
        QTextOption::Tab tab2;
        tab2.type = QTextOption::RightTab;
        tab2.delimiter = QChar::fromLatin1( 'P' );
        tab2.position = -1;
        t.append(tab);
        t.append(tab2);
        QVariant v;
        v.setValue(t);
        QCOMPARE(o->property(QString("tabs").toLatin1()).value<Tabs>(), v.value<Tabs>());
        t.clear();
        t.append(tab2);
        v.setValue(t);
        o->setProperty(QString("tabs").toLatin1(), v);
        QCOMPARE(o->property(QString("tabs").toLatin1()).value<Tabs>(), v.value<Tabs>());
        testProperty(o, QString("marginLeft"), QVariant(4), QVariant(1));
        testProperty(o, QString("marginTop"), QVariant(8), QVariant(2));
        testProperty(o, QString("marginRight"), QVariant(12), QVariant(3));
        testProperty(o, QString("marginBottom"), QVariant(16), QVariant(4));
        testProperty(o, QString("background"), QColor(QLatin1String("blue")), QColor(Qt::red));
        // text
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
        // text without alignment
        o = childrenList.at(1);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignLeft), QVariant(Qt::AlignHCenter));
        // html
        o = childrenList.at(2);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
        // Verify is margin property are still correct
        o->setProperty(QString("marginLeft").toLatin1(), QVariant(1));
        o->setProperty(QString("marginTop").toLatin1(), QVariant(2));
        o->setProperty(QString("marginRight").toLatin1(), QVariant(3));
        o->setProperty(QString("marginBottom").toLatin1(), QVariant(4));
        // image
        o = childrenList.at(3);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
        // chart
        o = childrenList.at(4);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
        // table
        o = childrenList.at(5);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
        // text element inside cell
        o = o->childrenElements().at(0)->childrenElements().at(0)->childrenElements().at(0);
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignHCenter), QVariant(Qt::AlignRight));
        // autotable
        o = childrenList.at(6);
        testProperty(o, QString("marginLeft"), QVariant(10), QVariant(10));
        testProperty(o, QString("marginTop"), QVariant(10), QVariant(10));
        testProperty(o, QString("marginRight"), QVariant(10), QVariant(10));
        testProperty(o, QString("marginBottom"), QVariant(10), QVariant(10));
        QCOMPARE(QString::fromLatin1(o->metaObject()->className()), QString::fromLatin1("ParagraphData"));
        testProperty(o, QString("alignment"), QVariant(Qt::AlignRight), QVariant(Qt::AlignHCenter));
    }
};

QTEST_MAIN(TestProperty)
#include "TestProperty.moc"
