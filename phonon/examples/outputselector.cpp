/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without fee is hereby
    granted, provided that the above copyright notice appear in all
    copies and that both that the copyright notice and this
    permission notice and warranty disclaimer appear in supporting
    documentation, and that the name of the author not be used in
    advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.

    The author disclaim all warranties with regard to this
    software, including all implied warranties of merchantability
    and fitness.  In no event shall the author be liable for any
    special, indirect or consequential damages or any damages
    whatsoever resulting from loss of use, data or profits, whether
    in an action of contract, negligence or other tortious action,
    arising out of or in connection with the use or performance of
    this software.

*/

#include <QtCore/QtDebug>
#include <QtXml/QDomDocument>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnectionInterface>
#include <QtGui/QApplication>
#include <QtGui/QToolButton>
#include <QtGui/QGridLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <Phonon/AudioOutputDevice>
#include <Phonon/BackendCapabilities>
#include <Phonon/AudioOutputDeviceModel>

class OrgKdePhononAudioOutputInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.kde.Phonon.AudioOutput"; }

public:
    OrgKdePhononAudioOutputInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgKdePhononAudioOutputInterface();

    Q_PROPERTY(bool muted READ muted WRITE setMuted)
    inline bool muted() const
    { return qvariant_cast< bool >(internalPropGet("muted")); }
    inline void setMuted(bool value)
    { internalPropSet("muted", qVariantFromValue(value)); }

    Q_PROPERTY(int outputDeviceIndex READ outputDeviceIndex WRITE setOutputDeviceIndex)
    inline int outputDeviceIndex() const
    { return qvariant_cast< int >(internalPropGet("outputDeviceIndex")); }
    inline void setOutputDeviceIndex(int value)
    { internalPropSet("outputDeviceIndex", qVariantFromValue(value)); }

    Q_PROPERTY(double volume READ volume WRITE setVolume)
    inline double volume() const
    { return qvariant_cast< double >(internalPropGet("volume")); }
    inline void setVolume(double value)
    { internalPropSet("volume", qVariantFromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusReply<QString> category()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("category"), argumentList);
    }

    inline QDBusReply<QString> name()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("name"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void mutedChanged(bool in0);
    void nameChanged(const QString &newName);
    void newOutputAvailable(const QString &service, const QString &path);
    void outputDestroyed();
    void outputDeviceIndexChanged(int in0);
    void volumeChanged(double in0);
};

namespace org {
  namespace kde {
    namespace Phonon {
      typedef ::OrgKdePhononAudioOutputInterface AudioOutput;
    }
  }
}

OrgKdePhononAudioOutputInterface::OrgKdePhononAudioOutputInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

OrgKdePhononAudioOutputInterface::~OrgKdePhononAudioOutputInterface()
{
}

using Phonon::AudioOutputDevice;
using Phonon::AudioOutputDeviceModel;

class RemoteAudioOutput : public QWidget
{
    Q_OBJECT
    public:
        RemoteAudioOutput(const QList<AudioOutputDevice> &m,
                const QString &service, const QString &path, QDBusConnection &connection, QWidget *parent);
        ~RemoteAudioOutput()
        {
            delete nameLabel;
            delete muteButton;
            delete volumeSlider;
            delete deviceCombo;
        }

        inline QString name() const { return m_name; }
        inline QString category() const { return m_category; }
        inline Phonon::AudioOutputDevice device() const { return m_device; }
        inline double volume() const { return m_volume; }
        inline bool muted() const { return m_muted; }

        inline void setDevice(const Phonon::AudioOutputDevice &d) { iface->setOutputDeviceIndex(d.index()); }
        inline void setVolume(double v) { iface->setVolume(v); }
        inline void setMuted(bool m) { iface->setMuted(m); }

    signals:
        void changed();

    private slots:
        void nameChanged(const QString &newName);
        void volumeChanged(double v);
        void mutedChanged(bool m);
        void outputDeviceIndexChanged(int i);
        void toggleMute(bool m);
        void sliderValue(int v);
        void deviceSelected(int i);

    private:
        const QList<AudioOutputDevice> &m_modelData;
        QString m_name;
        QString m_category;
        Phonon::AudioOutputDevice m_device;
        double m_volume;
        bool m_muted;
        org::kde::Phonon::AudioOutput *iface;

    public:
        QLabel *nameLabel;
        QToolButton *muteButton;
        QSlider *volumeSlider;
        QComboBox *deviceCombo;
};

void RemoteAudioOutput::nameChanged(const QString &newName)
{
    m_name = newName;
    nameLabel->setText(m_name);
    emit changed();
}

void RemoteAudioOutput::volumeChanged(double v)
{
    m_volume = v;
    volumeSlider->setValue(static_cast<int>(m_volume * 1000));
    emit changed();
}

void RemoteAudioOutput::mutedChanged(bool m)
{
    m_muted = m;
    muteButton->setChecked(m_muted);
    emit changed();
}

void RemoteAudioOutput::outputDeviceIndexChanged(int i)
{
    m_device = Phonon::AudioOutputDevice::fromIndex(i);
    QAbstractItemModel *model = deviceCombo->model();
    deviceCombo->setCurrentIndex(model->index(m_modelData.indexOf(m_device), 0).row());
    emit changed();
}

RemoteAudioOutput::RemoteAudioOutput(const QList<AudioOutputDevice> &m,
        const QString &service, const QString &path,
        QDBusConnection &connection, QWidget *parent)
    : QWidget(parent),
    m_modelData(m),
    iface(new org::kde::Phonon::AudioOutput(service, path, connection, this))
{
    connect(iface, SIGNAL(nameChanged(QString)), SLOT(nameChanged(QString)));
    connect(iface, SIGNAL(volumeChanged(double)), SLOT(volumeChanged(double)));
    connect(iface, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
    connect(iface, SIGNAL(outputDeviceIndexChanged(int)), SLOT(outputDeviceIndexChanged(int)));
    connect(iface, SIGNAL(outputDestroyed()), SLOT(deleteLater()));
    m_name = iface->name();
    m_category = iface->category();
    m_device = Phonon::AudioOutputDevice::fromIndex(iface->outputDeviceIndex());
    m_volume = iface->volume();
    m_muted = iface->muted();

    nameLabel = new QLabel(m_name, parent);

    muteButton = new QToolButton(parent);
    muteButton->setCheckable(true);
    muteButton->setChecked(m_muted);
    connect(muteButton, SIGNAL(toggled(bool)), SLOT(toggleMute(bool)));

    volumeSlider = new QSlider(Qt::Horizontal, parent);
    volumeSlider->setMinimumSize(100, 10);
    volumeSlider->setRange(0, 1000);
    volumeSlider->setValue(static_cast<int>(m_volume * 1000));
    connect(volumeSlider, SIGNAL(valueChanged(int)), SLOT(sliderValue(int)));

    deviceCombo = new QComboBox(parent);
    AudioOutputDeviceModel *model = new AudioOutputDeviceModel(deviceCombo);
    model->setModelData(m_modelData);
    deviceCombo->setModel(model);
    deviceCombo->setCurrentIndex(model->index(m_modelData.indexOf(m_device), 0).row());
    connect(deviceCombo, SIGNAL(activated(int)), SLOT(deviceSelected(int)));
}

void RemoteAudioOutput::toggleMute(bool m)
{
    iface->setMuted(m);
}

void RemoteAudioOutput::sliderValue(int v)
{
    iface->setVolume(v * 0.001);
}

void RemoteAudioOutput::deviceSelected(int i)
{
    iface->setOutputDeviceIndex(m_modelData.at(i).index());
}

class OutputSelector : public QWidget
{
    Q_OBJECT
    public:
        OutputSelector();

    private slots:
        void newOutputAvailable(const QString &service, const QString &path);
        void introspectDone(const QDBusMessage &msg);
        void cleanupOutput(QObject *o);
        void updateModel();

    private:
        void addOutput(RemoteAudioOutput *);
        QDBusConnection connection;
        QList<RemoteAudioOutput *> m_outputs;
        QList<AudioOutputDevice> m_modelData;
        QGridLayout *layout;
};

void OutputSelector::updateModel()
{
    m_modelData = Phonon::BackendCapabilities::availableAudioOutputDevices();
}

OutputSelector::OutputSelector()
    : connection(QDBusConnection::sessionBus()),
    layout(new QGridLayout(this))
{
    if (!connection.isConnected()) {
        QMessageBox::information(0, "D-Bus Connection Failed",
                QString("Could not connect to D-Bus server %1: %1")
                .arg(connection.lastError().name())
                .arg(connection.lastError().message()));
        exit(1);
    }

    layout->addWidget(new QLabel("Audio Output", this), 0, 0, Qt::AlignCenter);
    layout->addWidget(new QLabel("Mute", this), 0, 1, Qt::AlignCenter);
    layout->addWidget(new QLabel("Loudness", this), 0, 2, Qt::AlignCenter);
    layout->addWidget(new QLabel("Device", this), 0, 3, Qt::AlignCenter);

    m_modelData = Phonon::BackendCapabilities::availableAudioOutputDevices();
    connect(Phonon::BackendCapabilities::notifier(), SIGNAL(availableAudioOutputDevicesChanged()),
            SLOT(updateModel()));

    connection.connect(QString(), QString(), QLatin1String(org::kde::Phonon::AudioOutput::staticInterfaceName()),
            QLatin1String("newOutputAvailable"), this, SLOT(newOutputAvailable(QString, QString)));

    QDBusConnectionInterface *bus = connection.interface();
    QMap<uint, QString> serviceNameForPid;
    {
        const QStringList &services = bus->registeredServiceNames();
        foreach (const QString &serviceName, services) {
            QDBusReply<uint> reply = bus->servicePid(serviceName);
            if (!reply.isValid()) {
                continue;
            }
            const uint pid = reply;
            if (!serviceNameForPid.value(pid).isEmpty()) {
                continue;
            }
            const QString &s = bus->serviceOwner(serviceName);
            if (s != connection.baseService()) {
                serviceNameForPid[pid] = s;
            }
        }
    }

    for (QMapIterator<uint, QString> it(serviceNameForPid); it.hasNext();) {
        it.next();
        connection.callWithCallback(QDBusMessage::createMethodCall(it.value(),
                    QLatin1String("/AudioOutputs"),
                    QLatin1String("org.freedesktop.DBus.Introspectable"),
                    QLatin1String("Introspect")),
                this, SLOT(introspectDone(QDBusMessage)), 0, -1);
    }
}

void OutputSelector::introspectDone(const QDBusMessage &msg)
{
    if (msg.arguments().size() != 1 || msg.arguments().first().type() != QVariant::String) {
        return;
    }
    const QString &xml = msg.arguments().first().toString();

    QDomDocument doc;
    doc.setContent(xml);
    QDomElement node = doc.documentElement();
    for (QDomElement child = node.firstChildElement(); !child.isNull();
            child = child.nextSiblingElement()) {
        if (child.tagName() == QLatin1String("node")) {
            const QString &path = QLatin1String("/AudioOutputs/") + child.attribute(QLatin1String("name"));
            addOutput(new RemoteAudioOutput(m_modelData, msg.service(), path, connection, this));
        }
    }
}

void OutputSelector::newOutputAvailable(const QString &service, const QString &path)
{
    addOutput(new RemoteAudioOutput(m_modelData, service, path, connection, this));
}

void OutputSelector::addOutput(RemoteAudioOutput *r)
{
    layout->addWidget(r->nameLabel, 1 + m_outputs.size(), 0);
    layout->addWidget(r->muteButton, 1 + m_outputs.size(), 1);
    layout->addWidget(r->volumeSlider, 1 + m_outputs.size(), 2);
    layout->addWidget(r->deviceCombo, 1 + m_outputs.size(), 3);
    connect(r, SIGNAL(destroyed(QObject*)), SLOT(cleanupOutput(QObject*)));
    m_outputs << r;
    qDebug() << m_outputs.size();
}

void OutputSelector::cleanupOutput(QObject *o)
{
    RemoteAudioOutput *r = static_cast<RemoteAudioOutput *>(o);
    const int row = m_outputs.indexOf(r);
    for (int i = row; i < layout->rowCount() - 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            QLayoutItem *item = layout->itemAtPosition(i + 2, j);
            if (item) {
                layout->addItem(item, i + 1, j);
            }
        }
        for (int j = 0; j < 4; ++j) {
            QLayoutItem *item = layout->itemAtPosition(i + 2, j);
            if (item) {
                layout->removeItem(item);
            }
        }
    }
    m_outputs.removeAt(row);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    OutputSelector s;
    s.show();

    return app.exec();
}

#include "outputselector.moc"
