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

#include <XMLHandler.h>
#include <QStandardItemModel>
#include "ModelEditor.h"
#include "ValueAssociationWidget.h"
#include <QDebug>
#include <QXmlStreamReader>
#include <QAbstractItemModel>
#include <QXmlStreamWriter>
#include "ModelInfo.h"
#include "XmlUtils.h"

void XMLHandler::saveSampleData(QIODevice* device, ValueAssociationWidget* associationWidget, ModelEditor* modelEditor)
{
    QXmlStreamWriter writer( device );
    writer.setAutoFormatting(true);

    writer.writeStartElement( "sample-data" );

    writer.writeStartElement( "named-values" );
    saveAssociationData( writer, associationWidget->m_model );
    writer.writeEndElement(); // named-values

    writer.writeStartElement( "models" );
    saveModelData( writer, modelEditor->m_modelInfos.values() );
    writer.writeEndElement(); // models

    writer.writeEndElement(); // sample-data
}

KDReports::ErrorDetails XMLHandler::loadSampleData( QIODevice* device, ValueAssociationWidget* associationWidget, ModelEditor* modelEditor )
{
    QXmlStreamReader reader( device );

    reader.readNext(); // Start Document
    reader.readNext(); // sample-data
    if ( reader.name() != "sample-data" )
        reader.raiseError( QObject::tr("Expected <sample-data> as topmost node in XML file") );

    while ( !reader.atEnd() ) {
        reader.readNext();
        if ( reader.isStartElement() ) {

            if ( reader.name() == "named-values" )
                loadAssociationData( reader, associationWidget->m_model );
            else if ( reader.name() == "models" )
                loadModelData( reader, &modelEditor->m_modelInfos );
            else
                readUnknownElement( reader );
        }
    }
    if ( reader.hasError() ) {
        KDReports::ErrorDetails details( reader.errorString() );
        details.setLine( reader.lineNumber() );
        details.setColumn( reader.columnNumber() );
        return details;
    }
    else
        return KDReports::ErrorDetails();
}

void XMLHandler::saveModelData( QXmlStreamWriter& writer, const QList<ModelInfo*>& list)
{
    Q_FOREACH( ModelInfo* info, list ) {
        QAbstractItemModel* model = info->internalModel();

        writer.writeStartElement( QLatin1String("Model") );
        writer.writeAttribute( QLatin1String("title"), info->title() );
        writer.writeAttribute( QLatin1String("rows"), QString::number(info->dataRowCount()) );
        writer.writeAttribute( QLatin1String("columns"), QString::number(info->dataColumnCount()) );

        writer.writeStartElement( QLatin1String("Table") );
        for ( int row = 0; row < model->rowCount(); ++row ) {
            for ( int column = 0; column < model->columnCount(); ++column ) {
                writer.writeStartElement( "Data" );
                writer.writeAttribute( QLatin1String("row"), QString::number(row) );
                writer.writeAttribute( QLatin1String("column"), QString::number(column) );
                writer.writeCharacters( model->data( model->index( row, column ) ).value<QString>() );
                writer.writeEndElement(); // Data
            }
        }
        writer.writeEndElement(); // Table

        writer.writeEndElement(); // Model
    }
}


void XMLHandler::loadModelData( QXmlStreamReader& reader, QMap<QString,ModelInfo*>* dest )
{
    m_modelDest = dest;
    m_currentInfo = 0;

    while ( !reader.atEnd() ) {
        reader.readNext();

        if ( reader.isEndElement() )
            break;

        if ( reader.isStartElement() ) {
            if ( reader.name() == "Model" )
                readModel(reader);
            else
                readUnknownElement( reader );
        }
    }
}

void XMLHandler::readModel(QXmlStreamReader& reader)
{
    const QString title = reader.attributes().value("title").toString();
    bool ok;
    int rows = reader.attributes().value("rows").toString().toInt( &ok );
    if ( !ok )
        reader.raiseError( QObject::tr("expected a \"rows\" attribute with an integer value") );

    int columns = reader.attributes().value("columns").toString().toInt(&ok);
    if ( !ok )
        reader.raiseError( QObject::tr("expected a \"columns\" attribute with an integer value") );

    m_currentInfo = m_modelDest->value(title);
    if ( m_currentInfo ) {
        m_currentInfo->setDataRowCount( rows );
        m_currentInfo->setDataColumnCount( columns );
    }

    while ( !reader.atEnd() ) {
        reader.readNext();
        if ( reader.isEndElement() )
            break;
        else if ( reader.isStartElement() ) {
            if ( reader.name() == "Table" )
                readTable(reader);
            else
                readUnknownElement( reader);
        }
    }
}

void XMLHandler::readTable(QXmlStreamReader& reader)
{
    while ( ! reader.atEnd() ) {
        reader.readNext();
        if ( reader.isEndElement() )
            break;

        if ( reader.isStartElement() ) {
            if ( reader.name() == "Data" )
                readData(reader);
            else
                readUnknownElement( reader);
        }
    }
}

void XMLHandler::readData(QXmlStreamReader& reader)
{
    bool ok;
    int row = reader.attributes().value("row").toString().toInt(&ok);
    if ( !ok )
        reader.raiseError( QObject::tr("expected a \"row\" attribute with an integer value") );

    int column = reader.attributes().value("column").toString().toInt(&ok);
    if ( !ok )
        reader.raiseError( QObject::tr("expected a \"column\" attribute with an integer value") );

    const QString text = reader.readElementText();

    if ( m_currentInfo )
        m_currentInfo->internalModel()->setData( m_currentInfo->internalModel()->index(row, column), text );

    reader.readNext();
}

void XMLHandler::saveAssociationData( QXmlStreamWriter& writer, IdModel* data )
{
    for ( int row = 0; row < data->rowCount(); ++row ) {
        const QString id = data->index( row, 0 ).data().toString();
        const QString value = data->index( row, 1 ).data().toString();
        writer.writeStartElement( "entry" );
        writer.writeAttribute( "id", id );
        writer.writeAttribute( "value", value );
        writer.writeEndElement();
    }
}

void XMLHandler::loadAssociationData( QXmlStreamReader& reader, IdModel *dest )
{
    QMap<QString,int> indexMap;
    for ( int row=0; row < dest->rowCount(); ++ row )
        indexMap.insert( dest->data( dest->index( row, 0 ) ).value<QString>(), row );

    while ( !reader.atEnd() ) {
        reader.readNext();
        if ( reader.isEndElement() ) {
            break;
        }
        if ( reader.isStartElement() ) {
            if ( reader.name() == "entry" ) {
                // Found the start of an id/value pair
                const QString id = reader.attributes().value("id").toString();
                const QString value = reader.attributes().value("value").toString();
                if ( !id.isEmpty() && indexMap.contains( id )) {
                    dest->setData( dest->index( indexMap[id], 1 ), value );
                }
                while ( !reader.atEnd() ) {
                    reader.readNext();
                    if ( reader.isEndElement() ) {
                        break;
                    } else if ( reader.isStartElement() ) {
                        readUnknownElement( reader );
                    }
                }
            } else {
                readUnknownElement( reader );
            }
        }
    }
}
