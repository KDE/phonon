/*
 * Copyright (C) 2000-2003 the xine project
 * Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
 *
 * This file is copied from xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 * network buffering control
 */

#ifndef HAVE_NET_BUF_CTRL_H
#define HAVE_NET_BUF_CTRL_H

typedef struct nbc_s nbc_t;
nbc_t *nbc_init (xine_stream_t *xine);
void nbc_close (nbc_t *this);
void nbc_set_pause_cb (nbc_t *this, void (*cb)(void *), void *data);
void nbc_set_normal_cb (nbc_t *this, void (*cb)(void *), void *data);
int report_bufferstatus (nbc_t *this);

#endif
