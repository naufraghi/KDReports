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
#include "ModelEditor.h"
#include "ModelInfo.h"
#include "structure/ReportData.h"
#include <KDReports>
#include <QTabBar>
#include "structure/AutoTableElementData.h"
#include <KDReportsAutoTableElement>

using namespace KDReports;

Q_DECLARE_METATYPE( ModelInfo* )

class TestModelAssociation : public QObject {
    Q_OBJECT
private slots:

    void testModelEditorAddModel()
    {
        ModelEditor model;
        Report r;
        ReportData rd(r);

        model.setReportData(&rd);

        QCOMPARE(model.m_tabBar->count(), 0);

        model.slotAddModel("model1");
        QCOMPARE(model.m_tabBar->count(), 1);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("model1"));

        model.slotAddModel("model2");
        QCOMPARE(model.m_tabBar->count(), 2);
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("model2"));

        // Add existing model
        model.slotAddModel("model2");
        QCOMPARE(model.m_tabBar->count(), 2);
    }

    void testModelEditorRemoveModel()
    {
        ModelEditor model;
        Report r;
        ReportData rd(r);

        model.setReportData(&rd);

        model.slotAddModel("first");
        model.slotAddModel("second");
        model.slotAddModel("third");

        model.slotRemoveModel("second");
        QCOMPARE(model.m_tabBar->count(), 2);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("first"));
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("third"));
    }

    void testModelEditorModifyModel()
    {
        ModelEditor model;
        Report r;
        ReportData rd(r);

        model.setReportData(&rd);

        model.slotAddModel("first");
        model.slotAddModel("second");
        model.slotAddModel("third");

        model.slotModifyModel("newValue", "second");
        QCOMPARE(model.m_tabBar->count(), 3);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("first"));
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("newValue"));
        QCOMPARE(model.m_tabBar->tabData(2).value<ModelInfo*>()->title(), QString::fromLatin1("third"));

        // modify with existing model
        model.slotModifyModel("third", "first");
        QCOMPARE(model.m_tabBar->count(), 2);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("newValue"));
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("third"));
    }

    void testReportDataModelChanged()
    {
        ModelEditor model;
        Report r;
        ReportData rd(r);

        // Create document
        AutoTableElement auto1("model1");
        AutoTableElementData *t1 = new AutoTableElementData(auto1, QString::fromLatin1("model1"));
        AutoTableElement auto2("model2");
        AutoTableElementData *t2 = new AutoTableElementData(auto2, QString::fromLatin1("model2"));
        AutoTableElement auto3("model3");
        AutoTableElementData *t3 = new AutoTableElementData(auto3, QString::fromLatin1("model3"));
        rd.addObjectElementData(t1);
        rd.addObjectElementData(t2);
        rd.addObjectElementData(t3);

        model.setReportData(&rd);

        // test initial value
        QCOMPARE(model.m_tabBar->count(), 3);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("model1"));
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("model2"));
        QCOMPARE(model.m_tabBar->tabData(2).value<ModelInfo*>()->title(), QString::fromLatin1("model3"));

        // test modify model
        t1->setModelKey("newModel1");
        QCOMPARE(model.m_tabBar->count(), 3);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("newModel1"));

        // test moidy id with existing id
        t1->setModelKey("model2");
        QCOMPARE(model.m_tabBar->count(), 2);
        QCOMPARE(model.m_tabBar->tabData(0).value<ModelInfo*>()->title(), QString::fromLatin1("model2"));
        QCOMPARE(model.m_tabBar->tabData(1).value<ModelInfo*>()->title(), QString::fromLatin1("model3"));
    }
};

QTEST_MAIN(TestModelAssociation)
#include "TestModelAssociation.moc"
