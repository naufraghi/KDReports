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
#ifndef PROPERTYEDITORPROPERTY_H
#define PROPERTYEDITORPROPERTY_H

#include <qtextoption.h>
#include <KDToolsGui/kdproperty.h>
#include <KDToolsCore/kdtoolsglobal.h>
#include <KDToolsGui/kdpropertymodel.h>

class TabProperty : public KDAbstractCompositeProperty {
    TabProperty & operator=( const TabProperty & other );
protected:
    TabProperty( const TabProperty & other );
public:
    TabProperty( const QTextOption::Tab& value, const QString& name );
    ~TabProperty();

    /* reimp */ QVariant value() const;
    /* reimp */ void setValue( const QVariant& );

    /* reimp */ QString toString() const;

    /* reimp */ bool isEditable() const;

    /* reimp */ TabProperty * clone() const;
private:
    KDTOOLS_DECLARE_PRIVATE_DERIVED( TabProperty, KDAbstractCompositeProperty );
};

#endif // PROPERTYEDITORPROPERTY_H
#endif
