install( FILES
    phonon/AbstractAudioOutput
    phonon/AbstractMediaStream
    phonon/AbstractVideoOutput
    phonon/AddonInterface
    phonon/AudioCaptureDevice
    phonon/AudioCaptureDeviceModel
    phonon/AudioChannelDescription
    phonon/AudioChannelDescriptionModel
    phonon/AudioDataOutput
    phonon/AudioOutput
    phonon/AudioOutputDevice
    phonon/AudioOutputDeviceModel
    phonon/AudioOutputInterface
    phonon/AudioOutputInterface40
    phonon/AudioOutputInterface42
    phonon/AvCapture
    phonon/BackendCapabilities
    phonon/BackendInterface
    phonon/Effect
    phonon/EffectDescription
    phonon/EffectDescriptionModel
    phonon/EffectInterface
    phonon/EffectParameter
    phonon/EffectWidget
    phonon/Global
    phonon/GlobalDescriptionContainer
    phonon/MediaController
    phonon/MediaNode
    phonon/MediaObject
    phonon/MediaObjectInterface
    phonon/MediaSource
    phonon/Mrl
    phonon/ObjectDescription
    phonon/ObjectDescriptionData
    phonon/ObjectDescriptionModel
    phonon/ObjectDescriptionModelData
    phonon/Path
    phonon/PlatformPlugin
    phonon/SeekSlider
    phonon/StreamInterface
    phonon/SubtitleDescription
    phonon/SubtitleDescriptionModel
    phonon/VideoCaptureDevice
    phonon/VideoCaptureDeviceModel
    phonon/VideoPlayer
    phonon/VideoWidget
    phonon/VideoWidgetInterface
    phonon/VideoWidgetInterface44
    phonon/VideoWidgetInterfaceLatest
    phonon/VolumeFaderEffect
    phonon/VolumeFaderInterface
    phonon/VolumeSlider
DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

set(COMPAT_HEADERS
    old/Phonon/AbstractAudioOutput
    old/Phonon/AbstractMediaStream
    old/Phonon/AbstractVideoOutput
    old/Phonon/AddonInterface
    old/Phonon/AudioDevice
    old/Phonon/AudioDeviceEnumerator
    old/Phonon/AudioOutput
    old/Phonon/AudioOutputDevice
    old/Phonon/AudioOutputDeviceModel
    old/Phonon/AudioOutputInterface
    old/Phonon/BackendCapabilities
    old/Phonon/BackendInterface
    old/Phonon/Effect
    old/Phonon/EffectDescription
    old/Phonon/EffectDescriptionModel
    old/Phonon/EffectInterface
    old/Phonon/EffectParameter
    old/Phonon/EffectWidget
    old/Phonon/Global
    old/Phonon/MediaController
    old/Phonon/MediaNode
    old/Phonon/MediaObject
    old/Phonon/MediaObjectInterface
    old/Phonon/MediaSource
    old/Phonon/ObjectDescription
    old/Phonon/ObjectDescriptionModel
    old/Phonon/Path
    old/Phonon/PlatformPlugin
    old/Phonon/SeekSlider
    old/Phonon/StreamInterface
    old/Phonon/VideoPlayer
    old/Phonon/VideoWidget
    old/Phonon/VideoWidgetInterface
    old/Phonon/VolumeFaderEffect
    old/Phonon/VolumeFaderInterface
    old/Phonon/VolumeSlider
)

install(FILES ${COMPAT_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KDE/Phonon
    COMPONENT Devel)
# FIXME: phonon/Phonon (dir) and phonon/phonon (file) conflict on case-insensitive
#        systems if one enables this
if (PHONON_INSTALL_QT_COMPAT_HEADERS)
    # <Phonon/Foo>
    install(FILES ${COMPAT_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/phonon/Phonon
        COMPONENT Devel)
    # <phonon/phonon> meta-include
    install(FILES old/phonon_meta_include
        DESTINATION ${INCLUDE_INSTALL_DIR}/phonon
        COMPONENT Devel
        RENAME phonon)
endif()

if (PHONON_BUILD_EXPERIMENTAL)
    install(FILES
        old/Phonon/Experimental/AbstractVideoDataOutput
        old/Phonon/Experimental/AudioDataOutput
        old/Phonon/Experimental/SnapshotInterface
        old/Phonon/Experimental/VideoDataOutput
        old/Phonon/Experimental/VideoDataOutputInterface
        old/Phonon/Experimental/VideoFrame
        old/Phonon/Experimental/VideoFrame2
        old/Phonon/Experimental/Visualization
    DESTINATION ${INCLUDE_INSTALL_DIR}/KDE/Phonon/Experimental COMPONENT Devel)
    # Qt4.x never included the experimental library, so no need for compat headers.
endif (PHONON_BUILD_EXPERIMENTAL)
