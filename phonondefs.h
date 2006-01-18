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
public: \
	~classname(); \
protected: \
	/**
	 * \internal
	 * Constructor that is called from derived classes.
	 *
	 * \param parent Standard QObject parent.
	 */ \
	classname( bool callCreateIface, QObject* parent ); \
	/**
	 * \internal
	 * Called right after the <tt>delete m_iface</tt> statement to reset @c
	 * m_iface to @c 0. If this is not done there can be dangling pointers
	 * in the hierarchy of classes.
	 *
	 * The reimplementation has to call its parent class:
	 * \code
	 * void SubClass::ifaceDeleted()
	 * {
	 *   m_iface = 0;
	 *   ParentClass::ifaceDeleted();
	 * }
	 * \endcode
	 *
	 * \see aboutToDeleteIface
	 */ \
	virtual void ifaceDeleted(); \
	/**
	 * \internal
	 * Used to save the state of the object before the Iface object is
	 * deleted. This can be used to recreate the state in setupIface().
	 *
	 * \return \c true if the Iface object may be deleted.
	 * \return \c false if the Iface object should not be deleted.
	 * The reimplementation has to call its parent class:
	 * \code
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * A complete implementation could look like this:
	 * \code
	 * d->propertyA = m_iface->propertyA();
	 * d->propertyB = m_iface->propertyB();
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * \see ifaceDeleted
	 * \see slotDeleteIface
	 */ \
	virtual bool aboutToDeleteIface(); \
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
	void setupIface( Ifaces::classname* newIface ); \
	virtual void deleteIface(); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If m_iface is @c 0 it calls slotCreateIface
	 * before returning m_iface.
	 *
	 * \return the Iface object
	 *
	 * \see m_iface
	 */ \
	Ifaces::classname* iface(); \
	/**
	 * \internal
	 * The pointer to the Iface object. This can be @c.
	 */ \
	Ifaces::classname* m_iface; \
	class Private; \
	Private* d;

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
	~classname(); \
protected: \
	classname( bool callCreateIface, QObject* parent ); \
	/**
	 * \internal
	 * Called right after the <tt>delete m_iface</tt> statement to reset @c
	 * m_iface to @c 0. If this is not done there can be dangling pointers
	 * in the hierarchy of classes.
	 *
	 * The reimplementation has to call its parent class:
	 * \code
	 * void SubClass::ifaceDeleted()
	 * {
	 *   m_iface = 0;
	 *   ParentClass::ifaceDeleted();
	 * }
	 * \endcode
	 *
	 * \see aboutToDeleteIface
	 */ \
	virtual void ifaceDeleted(); \
	/**
	 * \internal
	 * Creates the Iface object belonging to this class. For most cases the
	 * implementation is
	 * \code
	 * m_iface = Factory::self()->createClassName( this );
	 * return m_iface;
	 * \endcode
	 *
	 * This function should not be called except from slotCreateIface.
	 *
	 * \see slotCreateIface
	 */ \
	virtual void createIface(); \
	/**
	 * \internal
	 * Used to save the state of the object before the Iface object is
	 * deleted. This can be used to recreate the state in setupIface().
	 *
	 * \return \c true if the Iface object may be deleted.
	 * \return \c false if the Iface object should not be deleted.
	 * The reimplementation has to call its parent class:
	 * \code
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * A complete implementation could look like this:
	 * \code
	 * d->propertyA = m_iface->propertyA();
	 * d->propertyB = m_iface->propertyB();
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * \see ifaceDeleted
	 * \see slotDeleteIface
	 */ \
	virtual bool aboutToDeleteIface(); \
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
	void setupIface( Ifaces::classname* newIface ); \
	virtual void deleteIface(); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If m_iface is @c 0 it calls slotCreateIface
	 * before returning m_iface.
	 *
	 * \return the Iface object
	 *
	 * \see m_iface
	 */ \
	Ifaces::classname* iface(); \
	/**
	 * \internal
	 * The pointer to the Iface object. This can be @c.
	 */ \
	Ifaces::classname* m_iface; \
	class Private; \
	Private* d;

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
	~classname(); \
protected: \
	classname( bool callCreateIface, QObject* parent ); \
	/**
	 * \internal
	 * Called right after the <tt>delete m_iface</tt> statement to reset @c
	 * m_iface to @c 0. If this is not done there can be dangling pointers
	 * in the hierarchy of classes.
	 *
	 * The reimplementation has to call its parent class:
	 * \code
	 * void SubClass::ifaceDeleted()
	 * {
	 *   m_iface = 0;
	 *   ParentClass::ifaceDeleted();
	 * }
	 * \endcode
	 *
	 * \see aboutToDeleteIface
	 */ \
	virtual void ifaceDeleted(); \
	/**
	 * \internal
	 * Creates the Iface object belonging to this class. For most cases the
	 * implementation is
	 * \code
	 * m_iface = Factory::self()->createClassName( this );
	 * return m_iface;
	 * \endcode
	 *
	 * This function should not be called except from slotCreateIface.
	 *
	 * \see slotCreateIface
	 */ \
	virtual void createIface(); \
	/**
	 * \internal
	 * Used to save the state of the object before the Iface object is
	 * deleted. This can be used to recreate the state in setupIface().
	 *
	 * \return \c true if the Iface object may be deleted.
	 * \return \c false if the Iface object should not be deleted.
	 * The reimplementation has to call its parent class:
	 * \code
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * A complete implementation could look like this:
	 * \code
	 * d->propertyA = m_iface->propertyA();
	 * d->propertyB = m_iface->propertyB();
	 * return ParentClass::aboutToDeleteIface();
	 * \endcode
	 *
	 * \see ifaceDeleted
	 * \see slotDeleteIface
	 */ \
	virtual bool aboutToDeleteIface(); \
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
	void setupIface( Ifaces::classname* newIface ); \
private: \
	/**
	 * \internal
	 * Returns the Iface object. If m_iface is @c 0 it calls slotCreateIface
	 * before returning m_iface.
	 *
	 * \return the Iface object
	 *
	 * \see m_iface
	 */ \
	Ifaces::classname* iface(); \
	/**
	 * \internal
	 * The pointer to the Iface object. This can be @c.
	 */ \
	Ifaces::classname* m_iface; \
	class Private; \
	Private* d;

#define PHONON_ABSTRACTBASE_IMPL( classname ) \
classname::classname( bool callCreateIface, QObject* parent ) \
	: Object( parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
	Q_ASSERT( callCreateIface == false ); \
} \
classname::~classname() \
{ \
	delete d; \
	d = 0; \
} \
void classname::ifaceDeleted() \
{ \
	m_iface = 0; \
} \
Ifaces::classname* classname::iface() \
{ \
	if( !m_iface ) \
		createIface(); \
	return m_iface; \
} \
void classname::deleteIface() \
{ \
	if( aboutToDeleteIface() ) \
	{ \
		delete m_iface; \
		ifaceDeleted(); \
	} \
}

#define PHONON_OBJECT_IMPL( classname ) \
classname::classname( QObject* parent ) \
	: Object( parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
} \
classname::classname( bool callCreateIface, QObject* parent ) \
	: Object( parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
	if( callCreateIface ) \
		createIface(); \
} \
classname::~classname() \
{ \
	delete d; \
	d = 0; \
} \
void classname::ifaceDeleted() \
{ \
	m_iface = 0; \
} \
Ifaces::classname* classname::iface() \
{ \
	if( !m_iface ) \
		createIface(); \
	return m_iface; \
} \
void classname::createIface() \
{ \
	Q_ASSERT( m_iface == 0 ); \
	m_iface = Factory::self()->create ## classname( this ); \
	setupIface( m_iface ); \
} \
void classname::deleteIface() \
{ \
	if( aboutToDeleteIface() ) \
	{ \
		delete m_iface; \
		ifaceDeleted(); \
	} \
}

#define PHONON_HEIR_IMPL( classname, parentclass ) \
classname::classname( QObject* parent ) \
	: parentclass( false, parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
} \
classname::classname( bool callCreateIface, QObject* parent ) \
	: parentclass( false, parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
	if( callCreateIface ) \
		createIface(); \
} \
classname::~classname() \
{ \
	delete d; \
	d = 0; \
} \
void classname::ifaceDeleted() \
{ \
	m_iface = 0; \
	parentclass::ifaceDeleted(); \
} \
Ifaces::classname* classname::iface() \
{ \
	if( !m_iface ) \
		createIface(); \
	return m_iface; \
} \
void classname::createIface() \
{ \
	Q_ASSERT( m_iface == 0 ); \
	m_iface = Factory::self()->create ## classname( this ); \
	setupIface( m_iface ); \
}

#endif // PHONONDEFS_H
