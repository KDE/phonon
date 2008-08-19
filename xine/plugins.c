/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include <xine.h>
#include <xine/compat.h>
#include <xine/input_plugin.h>
#include <xine/xine_internal.h>
#include <xine/xineutils.h>

extern void *init_kbytestream_plugin (xine_t *xine, void *data);
extern void *init_kvolumefader_plugin (xine_t *xine, void *data);
/*extern void *init_kmixer_plugin(xine_t *xine, void *data);*/

static const post_info_t kvolumefader_special_info = { XINE_POST_TYPE_AUDIO_FILTER };
/*static const post_info_t kmixer_special_info = { XINE_POST_TYPE_AUDIO_FILTER };*/

/*
 * exported plugin catalog entry
 */
const plugin_info_t phonon_xine_plugin_info[] = {
    /* type, API, "name", version, special_info, init_function */
    { PLUGIN_INPUT, 17, (char *)"KBYTESTREAM" , XINE_VERSION_CODE, NULL                      , &init_kbytestream_plugin  },
    { PLUGIN_POST , 9 , (char *)"KVolumeFader", XINE_VERSION_CODE, &kvolumefader_special_info, &init_kvolumefader_plugin },
    /*{ PLUGIN_POST , 9 , "KMixer"      , XINE_VERSION_CODE, &kmixer_special_info      , &init_kmixer_plugin       },*/
    { PLUGIN_NONE , 0 , (char *)""            , 0                , NULL                      , NULL                      }
};
