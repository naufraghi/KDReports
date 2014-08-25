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

#include "PropertyEditorWidget.h"
#ifdef HAVE_KDTOOLS
#include <KDToolsGui/KDPropertyView>
#include <KDToolsGui/KDPropertyModel>
#include <KDToolsGui/kdproperty.h>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QTextOption>
#include "PropertyEditorProperty.h"
#include "PropertyChangeCommand.h"
#else
#include <QLabel>
#endif
#include "structure/ObjectData.h"
#include "structure/ParagraphData.h"
#include "structure/ReportData.h"
#include <QVBoxLayout>
#include <QList>
#include <QBrush>
#include <QDebug>
#include "EditorData.h"
#include <float.h>

PropertyEditorWidget::PropertyEditorWidget(EditorData *editorData, QWidget *parent)
    : QWidget(parent)
    , m_editorData(editorData)
#ifdef HAVE_KDTOOLS

    , m_propertyview(new KDPropertyView)
    , m_propertymodel(0)
#endif
{
    QVBoxLayout* topLayout = new QVBoxLayout( this );
#ifdef HAVE_KDTOOLS
    qRegisterMetaType<Tabs>("Tabs");
    topLayout->addWidget( m_propertyview );
#else
    topLayout->addWidget(new QLabel(tr("<b>Please rebuild with KDTools support</b>"), this));
#endif
}

void PropertyEditorWidget::changeObjectData()
{
#ifdef HAVE_KDTOOLS
    ObjectData *object = m_editorData->selectedObject();
    m_object = object;
    if(m_propertymodel)
        m_propertymodel->deleteLater();
    m_propertymodel = new KDPropertyModel(this);
    connect( m_propertymodel, SIGNAL(propertyChanged(KDPropertyInterface*)),
             this, SLOT(propertyValueChanged(KDPropertyInterface*)));
    m_propertyview->setModel(m_propertymodel);

    if (!object)
        return;

    for(int i=1; i< object->metaObject()->propertyCount();i++){
        QMetaProperty metaProperty = object->metaObject()->property(i);
        QString name(metaProperty.name());
        QString translatedName(object->translatePropertyName(metaProperty.name()));
        QVariant value(object->property(metaProperty.name()));
        KDPropertyInterface *p;
        if (metaProperty.isEnumType()) {
            QMetaEnum enumerator = metaProperty.enumerator();
            if (!enumerator.isFlag()) {
                QStringList list;
                int currentKey = -1;
                for (int i = 0; i < enumerator.keyCount(); i++) {
                    QString name = object->translatedEnumValue(enumerator, i);
                    if (name.isEmpty())
                        name = QString::fromLatin1(enumerator.key(i));
                    list << name;
                    if (value.toInt() == enumerator.value(i))
                        currentKey = i;
                }
                p = new KDChoiceProperty(list, currentKey, translatedName);
            } else {
                QStringList list;
                QList<QVariant> indexList;
                QList<int> currentValues;
                foreach(QString key, QString::fromLatin1(enumerator.valueToKeys(value.toInt())).split('|')){
                    currentValues.append(enumerator.keyToValue(key.toLatin1()));
                }
                for (int i = 0; i < enumerator.keyCount(); i++) {
                    QString name = object->translatedEnumValue(enumerator, i);
                    if (name.isEmpty())
                        name = QString::fromLatin1(enumerator.key(i));
                    list << name;
                    if (currentValues.contains(enumerator.value(i)))
                        indexList << i;
                }
                p = new KDMultipleChoiceProperty(list, indexList, translatedName);
            }
        }  else if (metaProperty.type() == QVariant::Int) {
            QVariant min(object->propertyValue(name, ObjectData::MIN));
            QVariant max(object->propertyValue(name, ObjectData::MAX));
            int minValue = min.isValid() ? min.toInt() : 0;
            int maxValue = max.isValid() ? max.toInt() : INT_MAX;
            p = new KDIntProperty(minValue, maxValue, value.toInt(), translatedName);
        } else if (metaProperty.type() == QVariant::Double) {
            QVariant min(object->propertyValue(name, ObjectData::MIN));
            QVariant max(object->propertyValue(name, ObjectData::MAX));
            double minValue = min.isValid() ? min.toDouble() : 0.0;
            double maxValue = max.isValid() ? max.toDouble() : DBL_MAX;
            p = new KDDoubleProperty(minValue, maxValue, value.toDouble(), translatedName);
        } else if (metaProperty.type() == QVariant::Bool) {
            p = new KDBooleanProperty(value.toBool(), translatedName);
        } else if (metaProperty.type() == QVariant::Color) {
            p = new KDColorProperty(value.toString(), translatedName);
        } else if (metaProperty.type() == QVariant::Brush) {
            QBrush b = qVariantValue<QBrush>(value);
            p = new KDColorProperty( b.color(), translatedName);
        } else if (metaProperty.type() == QVariant::String ) {
            if (QString::fromLatin1(metaProperty.name()) == QString::fromLatin1("filename"))
                p = new KDFilenameProperty(value.toString(), translatedName);
            else
                p = new KDStringProperty(value.toString(), translatedName);
        } else if (metaProperty.type() == QVariant::Font ) {
            p = new KDFontProperty(qVariantValue<QFont>(value), translatedName);
        } else if (metaProperty.type() == QVariant::SizeF || metaProperty.type() == QVariant::Size) {
            QSizeF size = qVariantValue<QSize>(value);
            p = new KDSizeProperty(size.toSize(), translatedName);
        } else if (metaProperty.type() == QVariant::UserType ) {
            if (metaProperty.typeName() == QString::fromLatin1("Tabs")) {
                Tabs tabs = qVariantValue<Tabs >(metaProperty.read(object));
                KDPropertyCategory *c = new KDPropertyCategory(tr("Tabs"));
                c->setCheckable(false);
                foreach (QTextOption::Tab tab, tabs) {
                    c->addProperty(new TabProperty(tab, tr("Tab")));
                }
                p = c;
            } else {
                qWarning()<<metaProperty.typeName()<<" not yet supported";
                continue;
            }
        } else {
            qWarning()<<metaProperty.type()<<" not yet supported";
            continue;
        }
        m_propertymodel->addProperty( p );
    }
#endif
}

#ifdef HAVE_KDTOOLS

void PropertyEditorWidget::propertyValueChanged( KDPropertyInterface* property )
{
    // Get the metaProperty
    QMetaProperty metaProperty;
    for(int i=1; i< m_object->metaObject()->propertyCount();i++){
        if(m_object->translatePropertyName(m_object->metaObject()->property(i).name()) == property->name().toLatin1()){
            metaProperty = m_object->metaObject()->property(i);
            break;
        }
    }

    if (metaProperty.isValid()) {
        QVariant value;
        if (KDChoiceProperty* ch = dynamic_cast<KDChoiceProperty* >(property)) {
            QMetaEnum enumerator = metaProperty.enumerator();
            value = enumerator.keyToValue(ch->choices().at(ch->value().toInt()).toLatin1());
        } else {
            value = property->value();
        }
        m_editorData->changeProperty(m_object, metaProperty, value);
    } else {
        qWarning() << "Property" << metaProperty.name() << "does not exist";
    }
}

#endif
