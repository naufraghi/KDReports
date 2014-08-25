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
#ifdef HAVE_KDTOOLS
#include "PropertyEditorProperty.h"
#include <QStringList>

TabProperty::TabProperty( const TabProperty & other )
    : KDAbstractCompositeProperty( other )
{
    init( true );
}

TabProperty * TabProperty::clone() const {
    return new TabProperty( *this );
}

/*! Constructor. Creates a QSize property with name \a name
  and value \a value.
*/
TabProperty::TabProperty( const QTextOption::Tab& value, const QString& name )
    : KDAbstractCompositeProperty( name )
{
    init( false );
    QVariant v = QVariant::fromValue(value);
    setValue( v );
}

void TabProperty::init( bool copy ) {
    if ( copy )
        return;
    KDPropertyInterface* position = new KDDoubleProperty( 0,  QObject::tr("Position") );
    QStringList list;
    list << "Left Tab";
    list << "Right Tab";
    list << "Center Tab";
    list << "Delimiter Tab";
    KDPropertyInterface* type = new KDChoiceProperty(list, 0, QObject::tr("Type"));
    KDPropertyInterface* delimiter = new KDIntProperty( 0,  QObject::tr("Position") );
    position->setStandalone( false );
    type->setStandalone( false );
    delimiter->setStandalone( false );
    addProperty( position );
    addProperty( type );
    addProperty( delimiter );
}

TabProperty::~TabProperty() {}

QVariant TabProperty::value() const
{
    QTextOption::TabType type = static_cast<QTextOption::TabType>(propertyAt(1)->value().toInt());
#if QT_VERSION < 0x040700
    QTextOption::Tab tab;
    tab.position = propertyAt(0)->value().toDouble();
    tab.type = type;
    tab.delimiter = propertyAt(2)->value().toChar();
    return qVariantFromValue(tab);
#else
    return qVariantFromValue( QTextOption::Tab(propertyAt(0)->value().toDouble(), type, propertyAt(2)->value().toChar()) );
#endif
}
void TabProperty::setValue( const QVariant& v )
{
  QTextOption::Tab t = qVariantValue<QTextOption::Tab>(v);
  propertyAt(0)->setValue(t.position);
  propertyAt(1)->setValue(t.type);
  propertyAt(2)->setValue(t.delimiter);
  update();
}

bool TabProperty::isEditable() const { return false; }

QString TabProperty::toString() const
{
    return QString::fromLatin1("Tab(%1,%2,%3)").arg(propertyAt(0)->value().toDouble()).arg( propertyAt(1)->value().toInt()).arg(propertyAt(2)->value().toChar());
}
#endif
