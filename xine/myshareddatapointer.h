/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2008 Matthias Kretz <kretz@kde.org>
**
** This file is adapted from a part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MYSHAREDDATAPOINTER_H
#define MYSHAREDDATAPOINTER_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>

template <class T> class MySharedDataPointer
{
public:
    typedef T Type;

    inline T &operator*() { return *d; }
    inline const T &operator*() const { return *d; }
    inline T *operator->() { return d; }
    inline T *operator->() const { return d; }
    inline T *data() const { return d; }
    inline const T *constData() const { return d; }

    inline void detach() { if (d && d->ref != 1) detach_helper(); }

    inline void reset()
    {
        if(d && !d->ref.deref())
            d->deleteLater();

        d = 0;
    }

    inline operator bool () const { return d != 0; }

    inline bool operator==(const MySharedDataPointer<T> &other) const { return d == other.d; }
    inline bool operator!=(const MySharedDataPointer<T> &other) const { return d != other.d; }
    inline bool operator==(const T *ptr) const { return d == ptr; }
    inline bool operator!=(const T *ptr) const { return d != ptr; }

    inline MySharedDataPointer() { d = 0; }
    inline ~MySharedDataPointer() { if (d && !d->ref.deref()) d->deleteLater(); }

    explicit MySharedDataPointer(T *data);
    inline MySharedDataPointer(const MySharedDataPointer<T> &o) : d(o.d) { if (d) d->ref.ref(); }

#ifndef QT_NO_MEMBER_TEMPLATES
    template<class X>
    inline MySharedDataPointer(const MySharedDataPointer<X> &o) : d(static_cast<T *>(o.data()))
    {
        if(d)
            d->ref.ref();
    }
#endif

    inline MySharedDataPointer<T> & operator=(const MySharedDataPointer<T> &o) {
        if (o.d != d) {
            if (o.d)
                o.d->ref.ref();
            if (d && !d->ref.deref())
                d->deleteLater();
            d = o.d;
        }
        return *this;
    }
    inline MySharedDataPointer &operator=(T *o) {
        if (o != d) {
            if (o)
                o->ref.ref();
            if (d && !d->ref.deref())
                d->deleteLater();
            d = o;
        }
        return *this;
    }

    inline bool operator!() const { return !d; }

private:
    void detach_helper();

    T *d;
};

template <class T>
Q_OUTOFLINE_TEMPLATE void MySharedDataPointer<T>::detach_helper()
{
    T *x = new T(*d);
    x->ref.ref();
    if (!d->ref.deref())
        d->deleteLater();
    d = x;
}

template <class T>
Q_INLINE_TEMPLATE MySharedDataPointer<T>::MySharedDataPointer(T *adata) : d(adata)
{ if (d) d->ref.ref(); }

#endif // MYSHAREDDATAPOINTER_H
