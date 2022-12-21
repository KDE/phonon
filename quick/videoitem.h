// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <QQuickItem>
#include <QSGImageNode>
#include <QQuickWindow>
#include <QMutex>

#include <experimental/videodataoutput2.h>

class VideoItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QObject *output READ output CONSTANT)
public:
    explicit VideoItem(QQuickItem *parent = nullptr)
    {
        setFlag(QQuickItem::ItemHasContents);
        m_output.setAllowedFormats({Phonon::Experimental::VideoFrame2::Format_RGB32});
        m_output.setRunning(true);
        // connect(this, &QQuickItem::windowChanged, this, &VideoItem::handleWindowChanged);
        connect(&m_output,
                &Phonon::Experimental::VideoDataOutput2::frameReadySignal,
                this,
                [this](const Phonon::Experimental::VideoFrame2 &frame) {
                    QMutexLocker lock(&m_mutex);
                    m_dirty = true;
                    m_frame = frame;
                    window()->update();
                });
    }

    QObject *output()
    {
        return &m_output;
    }

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override
    {
        auto node = dynamic_cast<QSGImageNode *>(oldNode);

        if (!node) {
            node = window()->createImageNode();
            node->setTexture(window()->createTextureFromImage(QImage()));
            node->setOwnsTexture(true);
            connect(window(), &QQuickWindow::beforeRendering, this, [node, this] {
                if (!m_dirty) {
                    return;
                }
                QMutexLocker lock(&m_mutex);
                m_dirty = false;
                node->setTexture(window()->createTextureFromImage(m_frame.qImage()));
            }, Qt::DirectConnection);
        }
        node->setRect(boundingRect());

        return node;
    }

    QAtomicInt m_dirty = false;
    QMutex m_mutex;
    Phonon::Experimental::VideoFrame2 m_frame;
    Phonon::Experimental::VideoDataOutput2 m_output;
};
