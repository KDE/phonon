/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_VIDEOWIDGET_H
#define Phonon_VIDEOWIDGET_H

#include <QWidget>
#include <kdelibs_export.h>

class QString;

namespace Phonon
{
class AbstractVideoOutput;

namespace Ui
{
namespace Ifaces
{
	class VideoWidget;
}
	/**
	 * @short Widget to display video.
	 *
	 * This widget shows the video signal and provides an object that can be
	 * plugged into the VideoPath. VideoWidget itself cannot derive from
	 * AbstractVideoOutput because of a design limitation and therefore you have to
	 * use the class like this:
	 *
	 * @code
	 * VideoWidget* vwidget = new VideoWidget( this );
	 * videoPath->addOutput( vwidget->videoOutput() );
	 * @endcode
	 *
	 * @author Matthias Kretz <kretz@kde.org>
	 * @since 4.0
	 */
	class PHONON_EXPORT VideoWidget : public QWidget
	{
		friend class VideoWidgetHelper;
		Q_OBJECT
		Q_PROPERTY( bool fullscreen READ isFullscreen WRITE setFullscreen );
		public:
			/**
			 * Standard QWidget constructor.
			 *
			 * @param parent The parent widget.
			 */
			VideoWidget( QWidget* parent = 0 );
			~VideoWidget();

			/**
			 * Returns an object implementing the AbstractVideoOutput interface that
			 * is used for adding an output the the VideoPath. The returned
			 * object has no other uses.
			 *
			 * @return Returns an object for use with VideoPath.
			 */
			AbstractVideoOutput* videoOutput();

			/**
			 * Tells whether the VideoWidget should show the video using the
			 * full screen.
			 *
			 * @return @c true if the video is shown fullscreen
			 * @return @c false if the video is shown in a window
			 *
			 * @see setFullscreen
			 */
			bool isFullscreen() const;

		public slots:
			/**
			 * Sets whether the video should be shown using the full screen.
			 *
			 * @param fullscreen If @c true show video fullscreen, if @false
			 * show in a window.
			 *
			 * @see isFullscreen
			 */
			void setFullscreen( bool fullscreen );

		protected:
			VideoWidget( Ui::Ifaces::VideoWidget* iface, QWidget* parent );
			virtual bool aboutToDeleteIface();
			virtual Ui::Ifaces::VideoWidget* createIface();
			virtual void setupIface();

		protected slots:
			// when the Factory wants to change the Backend the following slots are used
			void slotDeleteIface();
			void slotCreateIface();

		private:
			Ui::Ifaces::VideoWidget* iface();
			Ui::Ifaces::VideoWidget* m_iface;
			class Private;
			Private* d;
	};
}} //namespace Phonon::Ui

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_VIDEOWIDGET_H
