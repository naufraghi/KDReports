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

#ifndef TOPELEMENT_H
#define TOPELEMENT_H

#include "ObjectData.h"

class EDITOR_EXPORT TopElement : public ObjectData
{
    Q_OBJECT
public:
    explicit TopElement();
    virtual void validate();
protected:
    void insertLineBefore(QTextCursor cursor);
    void insertLineAfter(QTextCursor cursor, const QTextBlockFormat &f);
    bool m_lineReturn;
    virtual void doRemove();
private:
};

#endif // TOPELEMENT_H
