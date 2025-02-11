/*
 * Copyright (C) 2021 Rafał Dzięgiel <rafostar.github@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GTUBER_ADAPTIVE_ADAPTIVE_STREAM_DEVEL_H__
#define __GTUBER_ADAPTIVE_ADAPTIVE_STREAM_DEVEL_H__

#if !defined(__GTUBER_PLUGIN_DEVEL_INSIDE__) && !defined(GTUBER_COMPILATION)
#error "Only "gtuber/gtuber-plugin-devel.h" can be included directly."
#endif

#include <gtuber/gtuber-adaptive-stream.h>

G_BEGIN_DECLS

GtuberAdaptiveStream * gtuber_adaptive_stream_new                 (void);

void                   gtuber_adaptive_stream_set_manifest_type   (GtuberAdaptiveStream *stream, GtuberAdaptiveStreamManifest type);

void                   gtuber_adaptive_stream_set_init_range      (GtuberAdaptiveStream *stream, guint64 start, guint64 end);

void                   gtuber_adaptive_stream_set_index_range     (GtuberAdaptiveStream *stream, guint64 start, guint64 end);

G_END_DECLS

#endif /* __GTUBER_ADAPTIVE_ADAPTIVE_STREAM_DEVEL_H__ */
