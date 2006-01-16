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

#ifndef KDEM2MDEFS_H
#define KDEM2MDEFS_H

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
 *   KDEM2M_ABSTRACTBASE( AbstractEffect )
 *   public:
 *     int propertyA() const;
 *     void setPropertyA( int );
 * };
 * \endcode
 *
 * \see KDEM2M_OBJECT
 * \see KDEM2M_HEIR
 */
#define KDEM2M_ABSTRACTBASE( classname ) \
public: \
	~classname(); \
protected: \
	/**
	 * \internal
	 * Constructor that is called from derived classes.
	 *
	 * \param iface Most of the time you can put
	 * <tt>createIface()</tt> there. But pay attention, sometimes
	 * you can't do that because the createIface method accesses an
	 * uninitialized member variable like the d-pointer.
	 *
	 * \param parent Standard QObject parent.
	 */ \
	classname( Ifaces::classname* iface, QObject* parent ); \
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
	virtual Ifaces::classname* createIface() = 0; \
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
	virtual void setupIface(); \
protected slots: \
	/**
	 * \internal
	 * This method cleanly deletes the Iface object. It is called on
	 * destruction and before a backend change.
	 */ \
	void slotDeleteIface(); \
	/**
	 * \internal
	 * This method takes care of creating and setting up the Iface object.
	 * It is called on construction and after a backend change.
	 */ \
	void slotCreateIface(); \
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
 *   KDEM2M_OBJECT( EffectSettings )
 *   public:
 *     int propertyA() const;
 *     void setPropertyA( int );
 * };
 * \endcode
 *
 * \see KDEM2M_ABSTRACTBASE
 * \see KDEM2M_HEIR
 */
#define KDEM2M_OBJECT( classname ) \
public: \
	/**
	 * Standard QObject constructor.
	 *
	 * @param parent QObject parent
	 */ \
	classname( QObject* parent = 0 ); \
	~classname(); \
protected: \
	/**
	 * \internal
	 * Constructor that is called from derived classes.
	 *
	 * \param iface Most of the time you can put
	 * <tt>createIface()</tt> there. But pay attention, sometimes
	 * you can't do that because the createIface method accesses an
	 * uninitialized member variable like the d-pointer.
	 *
	 * \param parent Standard QObject parent.
	 */ \
	classname( Ifaces::classname* iface, QObject* parent ); \
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
	virtual Ifaces::classname* createIface(); \
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
	virtual void setupIface(); \
protected slots: \
	/**
	 * \internal
	 * This method cleanly deletes the Iface object. It is called on
	 * destruction and before a backend change.
	 */ \
	void slotDeleteIface(); \
	/**
	 * \internal
	 * This method takes care of creating and setting up the Iface object.
	 * It is called on construction and after a backend change.
	 */ \
	void slotCreateIface(); \
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
 * classes that inherit another Kdem2m object.
 *
 * \param classname The Name of the class this macro is used for.
 * \param baseclass The Name of the baseclass that inherits QObject directly.
 *
 * Example:
 * \code
 * class ConcreteEffect : public AbstractEffect
 * {
 *   Q_OBJECT
 *   Q_PROPERTY( int propertyB READ propertyB WRITE setPropertyB )
 *   KDEM2M_HEIR( ConcreteEffect )
 *   public:
 *     int propertyB() const;
 *     void setPropertyB( int );
 * };
 * \endcode
 *
 * \see KDEM2M_ABSTRACTBASE
 * \see KDEM2M_OBJECT
 */
#define KDEM2M_HEIR( classname, baseclass ) \
public: \
	/**
	 * Standard QObject constructor.
	 *
	 * @param parent QObject parent
	 */ \
	classname( QObject* parent = 0 ); \
	~classname(); \
protected: \
	/**
	 * \internal
	 * Constructor that is called from derived classes.
	 *
	 * \param iface Most of the time you can put
	 * <tt>createIface()</tt> there. But pay attention, sometimes
	 * you can't do that because the createIface method accesses an
	 * uninitialized member variable like the d-pointer.
	 *
	 * \param parent Standard QObject parent.
	 */ \
	classname( Ifaces::classname* iface, QObject* parent ); \
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
	virtual Ifaces::baseclass* createIface(); \
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
	virtual void setupIface(); \
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

#define KDEM2M_ABSTRACTBASE_IMPL( classname ) \
classname::classname( Ifaces::classname* iface, QObject* parent ) \
	: QObject( parent ) \
	, m_iface( iface ) \
	, d( new Private() ) \
{ \
	connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( slotDeleteIface() ) ); \
	connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( slotCreateIface() ) ); \
} \
classname::~classname() \
{ \
	slotDeleteIface(); \
	delete d; \
	d = 0; \
} \
void classname::slotDeleteIface() \
{ \
	if( aboutToDeleteIface() ) \
	{ \
		delete m_iface; \
		ifaceDeleted(); \
	} \
} \
void classname::ifaceDeleted() \
{ \
	m_iface = 0; \
} \
void classname::slotCreateIface() \
{ \
	Q_ASSERT( m_iface == 0 ); \
	m_iface = createIface(); \
	setupIface(); \
} \
Ifaces::classname* classname::iface() \
{ \
	if( !m_iface ) \
		slotCreateIface(); \
	return m_iface; \
}

#define KDEM2M_OBJECT_IMPL( classname ) \
classname::classname( QObject* parent ) \
	: QObject( parent ) \
	, m_iface( 0 ) \
	, d( new Private() ) \
{ \
	slotCreateIface(); \
	connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( slotDeleteIface() ) ); \
	connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( slotCreateIface() ) ); \
} \
classname::classname( Ifaces::classname* iface, QObject* parent ) \
	: QObject( parent ) \
	, m_iface( iface ) \
	, d( new Private() ) \
{ \
	connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( slotDeleteIface() ) ); \
	connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( slotCreateIface() ) ); \
} \
classname::~classname() \
{ \
	slotDeleteIface(); \
	delete d; \
	d = 0; \
} \
void classname::slotDeleteIface() \
{ \
	if( aboutToDeleteIface() ) \
	{ \
		delete m_iface; \
		ifaceDeleted(); \
	} \
} \
void classname::ifaceDeleted() \
{ \
	m_iface = 0; \
} \
void classname::slotCreateIface() \
{ \
	Q_ASSERT( m_iface == 0 ); \
	m_iface = createIface(); \
	setupIface(); \
} \
Ifaces::classname* classname::iface() \
{ \
	if( !m_iface ) \
		slotCreateIface(); \
	return m_iface; \
} \
Ifaces::classname* classname::createIface() \
{ \
	m_iface = Factory::self()->create ## classname( this ); \
	return m_iface; \
}

#define KDEM2M_HEIR_IMPL( classname, parentclass, baseclass ) \
classname::classname( QObject* parent ) \
	: parentclass( m_iface = Factory::self()->create ## classname( this ), parent ) \
	, d( new Private() ) \
{ \
} \
classname::classname( Ifaces::classname* iface, QObject* parent ) \
	: parentclass( iface, parent ) \
	, m_iface( iface ) \
	, d( new Private() ) \
{ \
} \
classname::~classname() \
{ \
	slotDeleteIface(); \
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
		slotCreateIface(); \
	return m_iface; \
} \
Ifaces::baseclass* classname::createIface() \
{ \
	m_iface = Factory::self()->create ## classname( this ); \
	return m_iface; \
}

#endif // KDEM2MDEFS_H
