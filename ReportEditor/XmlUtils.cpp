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

#include "XmlUtils.h"
#include <QFile>
#include <QXmlStreamReader>

void readUnknownElement( QXmlStreamReader& reader )
{
    Q_ASSERT(reader.isStartElement());

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isEndElement())
            break;

        if (reader.isStartElement())
            readUnknownElement( reader );
    }
}

QList<QXmlStreamAttributes> extractAttributesList( const QString& xmlFile, const QSet<QString>& elementNames )
{
    QList<QXmlStreamAttributes> result;
    QFile file( xmlFile );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QXmlStreamReader reader( &file );
        while ( !reader.atEnd() ) {
            reader.readNext();
            if ( reader.isStartElement() ) {
                if ( elementNames.contains( reader.name().toString() ) ) {
                    result.append( reader.attributes() );
                }
            }
        }
    }
    return result;
}


QSet<QString> extractAttributes( const QString& xmlFile, const QSet<QString>& elementNames, const QString& attributeToExtract )
{
    QSet<QString> result;
    QList<QXmlStreamAttributes> attributesList = extractAttributesList( xmlFile, elementNames );
    Q_FOREACH( const QXmlStreamAttributes& attributes, attributesList ) {
        const QString attrib = attributes.value( attributeToExtract ).toString();
        if ( !attrib.isEmpty() ) {
            result.insert( attrib );
        }
    }
    return result;
}
