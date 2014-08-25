/****************************************************************************
** Copyright (C) 2007-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#include <QObject>
#include <QtTest/QtTest>
#include <ModelInfo.h>

class Test : public QObject {
    Q_OBJECT
private slots:
    void title()
    {
        ModelInfo info("A Title" );
        QCOMPARE( info.title(), QString("A Title"));
    }

private:
    void testEditable( QAbstractItemModel* model, int rows, int columns, bool bothTitles )
    {
        QCOMPARE( (model->flags( model->index(0,0) ) &  Qt::ItemIsSelectable) ? true : false, !bothTitles );
        for ( int column = (bothTitles ? 1 : 0 ); column < columns; ++column )
            QVERIFY( model->flags( model->index(0,column) ) &  Qt::ItemIsSelectable);
        for ( int row = 1; row < rows; ++row )
            for ( int col = 0; col < columns; ++col )
                QVERIFY( model->flags( model->index(row,col) ) &  Qt::ItemIsSelectable);
    }

    void checkProxySize( ModelInfo& info )
    {
        QAbstractItemModel* proxy = info.viewModel();
        QCOMPARE( proxy->columnCount(), info.dataColumnCount() );
        QCOMPARE( proxy->rowCount(), info.dataRowCount() );
    }

private slots:
    void emptyModelNoHeaders()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "false" );
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QCOMPARE( info.dataRowCount(), 1 );
        QCOMPARE( info.dataColumnCount(), 1 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 1 );
        QCOMPARE( model->columnCount(), 1 );

        QCOMPARE( model->headerData( 0, Qt::Horizontal ).value<QString>(), QString("1") );
        QCOMPARE( model->headerData( 0, Qt::Vertical ).value<QString>(), QString("1") );
        testEditable( model, 1, 1, false );
        checkProxySize( info );
    }


    void emptyModelHorizontalHeader()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "true" );
        info.setHeaderStateInfo( Qt::Vertical, "false" );

        QCOMPARE( info.dataRowCount(), 1 );
        QCOMPARE( info.dataColumnCount(), 1 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 2 );
        QCOMPARE( model->columnCount(), 1 );

        QCOMPARE( model->headerData( 0, Qt::Horizontal ).value<QString>(), QString("1") );
        QCOMPARE( model->headerData( 0, Qt::Vertical ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Vertical ).value<QString>(), QString("1") );
        testEditable( model, 1, 1, false );
        checkProxySize( info );
    }

    void emptyModelVericalHeader()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "false" );
        info.setHeaderStateInfo( Qt::Vertical, "true" );

        QCOMPARE( info.dataRowCount(), 1 );
        QCOMPARE( info.dataColumnCount(), 1 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 1 );
        QCOMPARE( model->columnCount(), 2 );

        QCOMPARE( model->headerData( 0, Qt::Horizontal ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Horizontal ).value<QString>(), QString("1") );
        QCOMPARE( model->headerData( 0, Qt::Vertical ).value<QString>(), QString("1") );
        testEditable( model, 1, 1, false );
        checkProxySize( info );
    }

    void emptyModelBothHeaders()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "true" );
        info.setHeaderStateInfo( Qt::Vertical, "true" );

        QCOMPARE( info.dataRowCount(), 1 );
        QCOMPARE( info.dataColumnCount(), 1 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 2 );
        QCOMPARE( model->columnCount(), 2 );

        QCOMPARE( model->headerData( 0, Qt::Horizontal ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Horizontal ).value<QString>(), QString("1") );
        QCOMPARE( model->headerData( 0, Qt::Vertical ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Vertical ).value<QString>(), QString("1") );

        testEditable( model, 1, 1, true );
        checkProxySize( info );
    }

    void emptyModelBothHeadersByDefault()
    {
        ModelInfo info( "test" );

        QCOMPARE( info.dataRowCount(), 1 );
        QCOMPARE( info.dataColumnCount(), 1 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 2 );
        QCOMPARE( model->columnCount(), 2 );

        QCOMPARE( model->headerData( 0, Qt::Horizontal ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Horizontal ).value<QString>(), QString("1") );
        QCOMPARE( model->headerData( 0, Qt::Vertical ).value<QString>(), QString("Title") );
        QCOMPARE( model->headerData( 1, Qt::Vertical ).value<QString>(), QString("1") );

        testEditable( model, 1, 1, true );
        checkProxySize( info );
    }

    void nonEmptyModelHorizontalHeader()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "true" );
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        info.setDataRowCount( 3 );
        info.setDataColumnCount( 5 );
        QCOMPARE( info.dataRowCount(), 3 );
        QCOMPARE( info.dataColumnCount(), 5 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 4 );
        QCOMPARE( model->columnCount(), 5 );

        testEditable( model, 3, 5, false );
        checkProxySize( info );
    }

    void nonEmptyModelBothHeader()
    {
        ModelInfo info( "test" );
        info.setHeaderStateInfo( Qt::Horizontal, "true" );
        info.setHeaderStateInfo( Qt::Vertical, "true" );
        info.setDataRowCount( 3 );
        info.setDataColumnCount( 5 );
        QCOMPARE( info.dataRowCount(), 3 );
        QCOMPARE( info.dataColumnCount(), 5 );

        QAbstractItemModel* model = info.editModel();
        QCOMPARE( model->rowCount(), 4 );
        QCOMPARE( model->columnCount(), 6 );

        testEditable( model, 3, 5, true );
        checkProxySize( info );
    }

    void proxyModelHeaderAdjusting()
    {
        for ( int rowOffset = 0; rowOffset < 2; ++rowOffset ) {
            for ( int colOffset = 0; colOffset < 2; ++colOffset ) {
                // qDebug("==================> %d,%d", rowOffset, colOffset );

                ModelInfo info( "test" );
                info.setHeaderStateInfo( Qt::Horizontal, rowOffset == 1 ? "true" : "false");
                info.setHeaderStateInfo( Qt::Vertical, colOffset == 1 ? "true" : "false");
                info.setDataRowCount( 6 );
                info.setDataColumnCount( 3 );

                QAbstractItemModel* model = info.editModel();
                QAbstractItemModel* proxy = info.viewModel();

                for ( int row = 0; row < 6+rowOffset; ++row ) {
                    for ( int col = 0; col < 3+colOffset; ++col ) {
                        model->setData( model->index( row, col ), QString("(%1,%2)").arg(col).arg(row) );
                    }
                }

                // Checking headers
                if ( rowOffset == 1 ) {
                    // Horizontal Header
                    for ( int col = 0; col < 3; ++col )
                        QCOMPARE( proxy->headerData( col, Qt::Horizontal ).value<QString>(), QString("(%1,0)").arg(col+colOffset) );
                }

                if ( colOffset == 1 ) {
                    // Vertical Header
                    for ( int row = 0; row < 6; ++ row )
                        QCOMPARE( proxy->headerData( row, Qt::Vertical ).value<QString>(), QString("(0,%1)").arg(row+rowOffset ) );
                }

                QCOMPARE( proxy->rowCount(), 6 );
                QCOMPARE( proxy->columnCount(), 3 );

                for ( int row = 0; row < 6; ++row ) {
                    for ( int col = 0; col < 3; ++col ) {
                        QCOMPARE( proxy->data( proxy->index( row, col ) ).value<QString>(), QString("(%1,%2)").arg(col+colOffset).arg(row + rowOffset) );
                    }
                }
            }
        }
    }

    void DefaultHeaderInfo2()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );
    }

    void ForcedOff()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( !info.headerVisible( Qt::Vertical ) );
    }

    void forcedOn()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "true" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );
    }

    void forcedOn2()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        info.setHeaderStateInfo( Qt::Vertical, "true" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );
    }

    void forcedOn3()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "true" );
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );
    }

    void forcedOn4()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "" );
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );
    }

    void reset()
    {
        ModelInfo info("test");
        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( !info.headerVisible( Qt::Vertical ) );

        info.setHeaderStateInfo( Qt::Vertical, "true" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );

        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( info.headerVisible( Qt::Vertical ) );

        info.resetHeaderInfo();
        QVERIFY( !info.headerVisible( Qt::Vertical ) );

        info.setHeaderStateInfo( Qt::Vertical, "false" );
        QVERIFY( !info.headerVisible( Qt::Vertical ) );
    }
};

QTEST_MAIN(Test)

#include "ModelInfoTest.moc"
