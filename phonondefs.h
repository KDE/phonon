/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONONDEFS_H
#define PHONONDEFS_H

#define PHONON_PRIVATECLASS( classname, baseclass ) \
protected: \
	virtual bool aboutToDeleteIface(); \
	virtual void createIface(); \
	virtual void setIface( void* p ) \
	{ \
		iface_ptr = reinterpret_cast<Ifaces::classname*>( p ); \
		baseclass##Private::setIface( static_cast<Ifaces::baseclass*>( iface_ptr ) ); \
	} \
private: \
	Ifaces::classname* iface_ptr; \
	inline Ifaces::classname* iface() { return iface_ptr; } \
	inline const Ifaces::classname* iface() const { return iface_ptr; } \

#define PHONON_PRIVATEABSTRACTCLASS( classname, baseclass ) \
protected: \
	virtual bool aboutToDeleteIface(); \
	virtual void setIface( void* p ) \
	{ \
		iface_ptr = reinterpret_cast<Ifaces::classname*>( p ); \
		baseclass##Private::setIface( static_cast<Ifaces::baseclass*>( iface_ptr ) ); \
	} \
private: \
	Ifaces::classname* iface_ptr; \
	inline Ifaces::classname* iface() { return iface_ptr; } \
	inline const Ifaces::classname* iface() const { return iface_ptr; } \

/**
 * \internal
 * Used in class declarations to provide the needed functions. This is used for
 * abstract base classes.
 *
 * \param classname The Name of the class this macro is used for.
 *
 * Example:
 * \code
 * class AbstractEffect : public QObject
 * {
 *   Q_OBJECT
 *   Q_PROPERTY( int propertyA READ propertyA WRITE setPropertyA )
 *   PHONON_ABSTRACTBASE( AbstractEffect )
 *   public:
 *     int propertyA() const;
 *     void setPropertyA( int );
 * };
 * \endcode
 *
 * \see PHONON_OBJECT
 * \see PHONON_HEIR
 */
#define PHONON_ABSTRACTBASE( classname ) \
protected: \
	/**
	 * \internal
	 * Constructor that is called from derived classes.
	 *
	 * \param parent Standard QObject parent.
	 */ \
	classname( classname ## Private& d, QObject* parent ); \
	/**
	 * \internal
	 * After construction of the Iface object this method is called
	 * throughout the complete class hierarchy in order to set up the
	 * properties that were already set on the public interface.
	 *
	 * An example implementation could look like this:
	 * \code
	 * ParentClass::setupIface();
	 * m_iface->setPropertyA( d->propertyA );
	 * m_iface->setPropertyB( d->propertyB );
	 * \endcode
	 */ \
	void setupIface(); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If the object does not exist it tries to
	 * create it before returning.
	 *
	 * \return the Iface object, might return \c 0
	 */ \
	Ifaces::classname* iface(); \

/**
 * \internal
 * Used in class declarations to provide the needed functions. This is used for
 * classes that inherit QObject directly.
 *
 * \param classname The Name of the class this macro is used for.
 *
 * Example:
 * \code
 * class EffectSettings : public QObject
 * {
 *   Q_OBJECT
 *   Q_PROPERTY( int propertyA READ propertyA WRITE setPropertyA )
 *   PHONON_OBJECT( EffectSettings )
 *   public:
 *     int propertyA() const;
 *     void setPropertyA( int );
 * };
 * \endcode
 *
 * \see PHONON_ABSTRACTBASE
 * \see PHONON_HEIR
 */
#define PHONON_OBJECT( classname ) \
public: \
	/**
	 * Standard QObject constructor.
	 *
	 * @param parent QObject parent
	 */ \
	classname( QObject* parent = 0 ); \
protected: \
	classname( classname ## Private& d, QObject* parent ); \
	/**
	 * \internal
	 * After construction of the Iface object this method is called
	 * throughout the complete class hierarchy in order to set up the
	 * properties that were already set on the public interface.
	 *
	 * An example implementation could look like this:
	 * \code
	 * ParentClass::setupIface();
	 * m_iface->setPropertyA( d->propertyA );
	 * m_iface->setPropertyB( d->propertyB );
	 * \endcode
	 */ \
	void setupIface(); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If the object does not exist it tries to
	 * create it before returning.
	 *
	 * \return the Iface object, might return \c 0
	 */ \
	Ifaces::classname* iface(); \

/**
 * \internal
 * Used in class declarations to provide the needed functions. This is used for
 * classes that inherit another Phonon object.
 *
 * \param classname The Name of the class this macro is used for.
 *
 * Example:
 * \code
 * class ConcreteEffect : public AbstractEffect
 * {
 *   Q_OBJECT
 *   Q_PROPERTY( int propertyB READ propertyB WRITE setPropertyB )
 *   PHONON_HEIR( ConcreteEffect )
 *   public:
 *     int propertyB() const;
 *     void setPropertyB( int );
 * };
 * \endcode
 *
 * \see PHONON_ABSTRACTBASE
 * \see PHONON_OBJECT
 */
#define PHONON_HEIR( classname ) \
public: \
	/**
	 * Standard QObject constructor.
	 *
	 * @param parent QObject parent
	 */ \
	classname( QObject* parent = 0 ); \
protected: \
	classname( classname ## Private& d, QObject* parent ); \
	/**
	 * \internal
	 * After construction of the Iface object this method is called
	 * throughout the complete class hierarchy in order to set up the
	 * properties that were already set on the public interface.
	 *
	 * An example implementation could look like this:
	 * \code
	 * ParentClass::setupIface();
	 * m_iface->setPropertyA( d->propertyA );
	 * m_iface->setPropertyB( d->propertyB );
	 * \endcode
	 */ \
	void setupIface(); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If the object does not exist it tries to
	 * create it before returning.
	 *
	 * \return the Iface object, might return \c 0
	 */ \
	Ifaces::classname* iface(); \

#define PHONON_ABSTRACTBASE_IMPL( classname ) \
classname::classname( classname ## Private& d, QObject* parent ) \
	: Base( d, parent ) \
{ \
} \
Ifaces::classname* classname::iface() \
{ \
	Q_D( classname ); \
	if( !d->iface() ) \
		d->createIface(); \
	return d->iface(); \
} \

#define PHONON_OBJECT_IMPL( classname ) \
classname::classname( QObject* parent ) \
	: Base( *new classname ## Private(), parent ) \
{ \
	Q_D( classname ); \
	d->createIface(); \
} \
classname::classname( classname ## Private& d, QObject* parent ) \
	: Base( d, parent ) \
{ \
} \
Ifaces::classname* classname::iface() \
{ \
	Q_D( classname ); \
	if( !d->iface() ) \
		d->createIface(); \
	return d->iface(); \
} \
void classname ## Private::createIface() \
{ \
	Q_Q( classname ); \
	Q_ASSERT( iface_ptr == 0 ); \
	setIface( Factory::self()->create ## classname( q ) ); \
	q->setupIface(); \
} \

#define PHONON_HEIR_IMPL( classname, parentclass ) \
classname::classname( QObject* parent ) \
	: parentclass( *new classname ## Private, parent ) \
{ \
} \
classname::classname( classname ## Private& d, QObject* parent ) \
	: parentclass( d, parent ) \
{ \
} \
Ifaces::classname* classname::iface() \
{ \
	Q_D( classname ); \
	if( !d->iface() ) \
		d->createIface(); \
	return d->iface(); \
} \
void classname ## Private::createIface() \
{ \
	Q_Q( classname ); \
	Q_ASSERT( iface_ptr == 0 ); \
	setIface( Factory::self()->create ## classname( q ) ); \
	q->setupIface(); \
}

#endif // PHONONDEFS_H
