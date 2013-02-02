/*
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

#include "videowidgetplugin.h"

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtCore/qplugin.h>

#include <phonon/videowidget.h>

static const char *toolTipC = "Phonon Video Widget";

VideoWidgetPlugin::VideoWidgetPlugin(const QString &group, QObject *parent) :
    QObject(parent),
    m_group(group),
    m_initialized(false)
{
}

QString VideoWidgetPlugin::name() const
{
    return QLatin1String("Phonon::VideoWidget");
}

QString VideoWidgetPlugin::group() const
{
    return m_group;
}

QString VideoWidgetPlugin::toolTip() const
{
    return QString(QLatin1String(toolTipC));
}

QString VideoWidgetPlugin::whatsThis() const
{
    return QString(QLatin1String(toolTipC));
}

QString VideoWidgetPlugin::includeFile() const
{
    return QLatin1String("<phonon/videowidget.h>");
}

QIcon VideoWidgetPlugin::icon() const
{
    // TODO: icon
    return QIcon();
}

bool VideoWidgetPlugin::isContainer() const
{
    return false;
}

QWidget *VideoWidgetPlugin::createWidget(QWidget *parent)
{
    return new Phonon::VideoWidget(parent);
}

bool VideoWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

void VideoWidgetPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
    Q_UNUSED(formEditor);
    if (m_initialized)
        return;
    m_initialized = true;
}

QString VideoWidgetPlugin::domXml() const
{
    return QLatin1String("\
    <ui language=\"c++\">\
        <widget class=\"Phonon::VideoWidget\" name=\"VideoWidget\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>300</width>\
                    <height>200</height>\
                </rect>\
            </property>\
        </widget>\
    </ui>");
}
