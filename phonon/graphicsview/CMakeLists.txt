set(phonon_VGO_SRCS
    graphicsview/glpainter.cpp
    graphicsview/glslpainter.cpp
    graphicsview/qpainterpainter.cpp
    graphicsview/videographicsobject.cpp

    # Work around QtCreator not displaying random headers.
    graphicsview/abstractvideographicspainter.h
    graphicsview/glpainter.h
    graphicsview/videoframe.h
    graphicsview/videographicsobjectinterface.h

    PARENT_SCOPE
)

set(phonon_VGO_HDRS
    graphicsview/videoframe.h
    # VGO does not need to be public, as there is only pull but no push behavior.
    #graphicsview/videographicsobject.h
    graphicsview/videographicsobjectinterface.h

    PARENT_SCOPE
)
