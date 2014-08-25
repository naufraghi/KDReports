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

#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include "IdModel.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMap>
#include <QString>
#include <QList>
#include <KDReportsErrorDetails.h>
class ModelEditor;
class ValueAssociationWidget;
class QAbstractItemModel;
class QIODevice;
class ModelInfo;

/**
 * This class is the loader/saver of sample data.
 */
class XMLHandler
{
public:
    void saveSampleData( QIODevice*, ValueAssociationWidget*, ModelEditor* );
    void saveModelData( QXmlStreamWriter& writer, const QList<ModelInfo*>& );
    void saveAssociationData( QXmlStreamWriter& writer, IdModel *data );

    KDReports::ErrorDetails loadSampleData( QIODevice*, ValueAssociationWidget*, ModelEditor*);
    void loadModelData( QXmlStreamReader&  reader, QMap<QString,ModelInfo*>* dest );
    void loadAssociationData( QXmlStreamReader& reader, IdModel* dest );

private:
    void readModel(QXmlStreamReader& reader);
    void readTable(QXmlStreamReader& reader);
    void readData(QXmlStreamReader& reader);

    QMap<QString,ModelInfo*>* m_modelDest;
    ModelInfo* m_currentInfo;
};

#endif /* XMLHANDLER_H */
