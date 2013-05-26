PHONON_BUILD_PHONON4QT5 {
    !greaterThan(QT_MAJOR_VERSION, 4) {
        error(Qt 5 is required to build Phonon4Qt5)
    }
    TARGET = phonon4qt5
} else {
    greaterThan(QT_MAJOR_VERSION, 4) {
        error("Qt 4 is required to build Phonon, to build Phonon4Qt5 you need to pass CONFIG+=PHONON_BUILD_PHONON4QT5")
    }
    TARGET = phonon
}

TEMPLATE = lib

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

PHONON_LIB_MAJOR_VERSION = 4
PHONON_LIB_MINOR_VERSION = 6
PHONON_LIB_PATCH_VERSION = 60

VERSION = $${PHONON_LIB_MAJOR_VERSION}.$${PHONON_LIB_MINOR_VERSION}.$${PHONON_LIB_PATCH_VERSION}

DEFINES += MAKE_PHONON_LIB

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libpulse libpulse-mainloop-glib
    DEFINES += HAVE_PULSEAUDIO
}

PHONON_NO_DBUS {
    message("Not building DBus support")
    PHONON_NO_DBUS_DEFINE = $${LITERAL_HASH}define PHONONO_NO_DBUS
} else {
    message("Trying to build DBus support")
    PHONON_NO_DBUS_DEFINE = /* $${LITERAL_HASH}undef PHONON_NO_DBUS */
}
PHONON_NO_CAPTURE {
    message("Not building capture support")
    PHONON_NO_CAPTURE_DEFINE = $${LITERAL_HASH}define PHONON_NO_CAPTURE
} else {
    message("Trying to build capture support")
    PHONON_NO_CAPTURE_DEFINE = /* $${LITERAL_HASH}undef PHONON_NO_CAPTURE */
}

phononconfig_p_h.input = phononconfig_p.h.in
phononconfig_p_h.output = phononconfig_p.h

phononnamespace_h.input = phononnamespace.h.in
phononnamespace_h.output = phononnamespace.h

QMAKE_SUBSTITUTES += \
    phononconfig_p_h \
    phononnamespace_h

# Breaks stuff, unkown why.
# QMAKE_CLEAN += \
#     $${phononconfig_p_h.output} \
#     $${phononnamespace_h.output}

SOURCES += \
    abstractaudiooutput.cpp \
    abstractaudiooutput_p.cpp \
    abstractmediastream.cpp \
    abstractvideooutput.cpp \
    abstractvideooutput_p.cpp \
    audiodataoutput.cpp \
    audiooutput.cpp \
    audiooutputinterface.cpp \
    backendcapabilities.cpp \
    effect.cpp \
    effectparameter.cpp \
    effectwidget.cpp \
    factory.cpp \
    globalconfig.cpp \
    iodevicestream.cpp \
    mediacontroller.cpp \
    mediaobject.cpp \
    medianode.cpp \
    mediasource.cpp \
    mrl.cpp \
    objectdescription.cpp \
    objectdescriptionmodel.cpp \
    path.cpp \
    phononnamespace.cpp \
    platform.cpp \
    pulsesupport.cpp \
    seekslider.cpp \
    statesvalidator.cpp \
    streaminterface.cpp \
    swiftslider.cpp \
    volumefadereffect.cpp \
    volumeslider.cpp \
    videoplayer.cpp \
    videowidget.cpp

headers.files = \
    abstractaudiooutput.h \
    abstractmediastream.h \
    abstractvideooutput.h \
    addoninterface.h \
    audiodataoutput.h \
    audiodataoutputinterface.h \
    audiooutput.h \
    audiooutputinterface.h \
    backendcapabilities.h \
    backendinterface.h \
    effect.h \
    effectinterface.h \
    effectparameter.h \
    effectwidget.h \
    globalconfig.h \
    globaldescriptioncontainer.h \
    mediacontroller.h \
    medianode.h \
    mediaobject.h \
    mediaobjectinterface.h \
    mediasource.h \
    mrl.h \
    objectdescription.h \
    objectdescriptionmodel.h \
    path.h \
    phonondefs.h \
    phononnamespace.h \
    phonon_export.h \
    platformplugin.h \
    pulsesupport.h \
    seekslider.h \
    streaminterface.h \
    videoplayer.h \
    videowidget.h \
    videowidgetinterface.h \
    volumefadereffect.h \
    volumefaderinterface.h \
    volumeslider.h

private_headers.files = \
    abstractaudiooutput_p.h \
    abstractmediastream_p.h \
    abstractvideooutput_p.h \
    audiodataoutput_p.h \
    audiooutputadaptor_p.h \
    audiooutput_p.h \
    backendcapabilities_p.h \
    effectparameter_p.h \
    effect_p.h \
    effectwidget_p.h \
    factory_p.h \
    frontendinterface_p.h \
    globalconfig_p.h \
    globalstatic_p.h \
    iodevicestream_p.h \
    medianodedestructionhandler_p.h \
    medianode_p.h \
    mediaobject_p.h \
    mediasource_p.h \
    objectdescriptionmodel_p.h \
    objectdescription_p.h \
    path_p.h \
    phonondefs_p.h \
    phononnamespace_p.h \
    phononpimpl_p.h \
    platform_p.h \
    pulsestream_p.h \
    qsettingsgroup_p.h \
    seekslider_p.h \
    statesvalidator_p.h \
    streaminterface_p.h \
    swiftslider_p.h \
    videowidget_p.h \
    volumefadereffect_p.h \
    volumeslider_p.h

HEADERS += $${headers.files} $${private_headers.files}

camel_headers.files = \
    ../includes/phonon/AbstractAudioOutput \
    ../includes/phonon/AbstractMediaStream \
    ../includes/phonon/AbstractVideoOutput \
    ../includes/phonon/AddonInterface \
    ../includes/phonon/AudioCaptureDevice \
    ../includes/phonon/AudioCaptureDeviceModel \
    ../includes/phonon/AudioChannelDescription \
    ../includes/phonon/AudioChannelDescriptionModel \
    ../includes/phonon/AudioDataOutput \
    ../includes/phonon/AudioOutput \
    ../includes/phonon/AudioOutputDevice \
    ../includes/phonon/AudioOutputDeviceModel \
    ../includes/phonon/AudioOutputInterface \
    ../includes/phonon/AudioOutputInterface40 \
    ../includes/phonon/AudioOutputInterface42 \
    ../includes/phonon/AvCapture \
    ../includes/phonon/BackendCapabilities \
    ../includes/phonon/BackendInterface \
    ../includes/phonon/Effect \
    ../includes/phonon/EffectDescription \
    ../includes/phonon/EffectDescriptionModel \
    ../includes/phonon/EffectInterface \
    ../includes/phonon/EffectParameter \
    ../includes/phonon/EffectWidget \
    ../includes/phonon/Global \
    ../includes/phonon/GlobalDescriptionContainer \
    ../includes/phonon/MediaController \
    ../includes/phonon/MediaNode \
    ../includes/phonon/MediaObject \
    ../includes/phonon/MediaObjectInterface \
    ../includes/phonon/MediaSource \
    ../includes/phonon/Mrl \
    ../includes/phonon/ObjectDescription \
    ../includes/phonon/ObjectDescriptionData \
    ../includes/phonon/ObjectDescriptionModel \
    ../includes/phonon/ObjectDescriptionModelData \
    ../includes/phonon/Path \
    ../includes/phonon/PlatformPlugin \
    ../includes/phonon/SeekSlider \
    ../includes/phonon/StreamInterface \
    ../includes/phonon/SubtitleDescription \
    ../includes/phonon/SubtitleDescriptionModel \
    ../includes/phonon/VideoCaptureDevice \
    ../includes/phonon/VideoCaptureDeviceModel \
    ../includes/phonon/VideoPlayer \
    ../includes/phonon/VideoWidget \
    ../includes/phonon/VideoWidgetInterface \
    ../includes/phonon/VideoWidgetInterface44 \
    ../includes/phonon/VideoWidgetInterfaceLatest \
    ../includes/phonon/VolumeFaderEffect \
    ../includes/phonon/VolumeFaderInterface \
    ../includes/phonon/VolumeSlider

message(prefix support is not implemented)
unix:!symbian {
    maemo5 {
        camel_headers.path = /opt/usr/include/$${TARGET}
        headers.path = /opt/usr/include
        target.path = /opt/usr/lib
    } else {
        camel_headers.path = /usr/include/$${TARGET}
        headers.path = /usr/include/$${TARGET}
        target.path = /usr/lib
    }
    INSTALLS += camel_headers headers target
}
