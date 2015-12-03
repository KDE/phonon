/*  This file is part of the KDE project
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mrl.h"

#include <QtCore/QFile>

namespace Phonon
{

Mrl::Mrl()
    : QUrl()
{
}

Mrl::Mrl(const Mrl &copy)
    : QUrl(copy)
{
}

Mrl::Mrl(const QUrl &url)
    : QUrl(url)
{
}

Mrl::Mrl(const QString &url)
    : QUrl(url)
{
}

Mrl &Mrl::operator =(const Mrl &copy)
{
    QUrl::operator =(copy);
    return *this;
}

#ifndef QT_NO_URL_CAST_FROM_STRING
Mrl &Mrl::operator =(const QString &url)
{
    QUrl::operator =(url);
    return *this;
}
#endif // QT_NO_URL_CAST_FROM_STRING

QByteArray Mrl::toEncoded(FormattingOptions options) const
{
    QByteArray encodedMrl;

    static QByteArray encodingExclude(":/\\?=&,@");
    if (scheme() == QLatin1String("")) {
        encodedMrl = QFile::encodeName("file://" + toLocalFile()).toPercentEncoding(encodingExclude);
    } else if (scheme() == QLatin1String("file")) {
        encodedMrl = QFile::encodeName("file://" + toLocalFile()).toPercentEncoding(encodingExclude);
    } else {
        encodedMrl = QUrl::toEncoded(options);
    }

    return encodedMrl;
}

} // namespace Phonon
