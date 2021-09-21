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

#include <glib-object.h>

#include "gtuber-stream.h"
#include "gtuber-stream-private.h"
#include "gtuber-adaptive-stream.h"
#include "gtuber-adaptive-stream-devel.h"
#include "gtuber-adaptive-stream-private.h"

#define parent_class gtuber_adaptive_stream_parent_class
G_DEFINE_TYPE (GtuberAdaptiveStream, gtuber_adaptive_stream, GTUBER_TYPE_STREAM);

static void
gtuber_adaptive_stream_init (GtuberAdaptiveStream *self)
{
  self = gtuber_adaptive_stream_get_instance_private (self);

  self->init_start = 0;
  self->init_end = 0;

  self->index_start = 0;
  self->index_end = 0;
}

static void
gtuber_adaptive_stream_class_init (GtuberAdaptiveStreamClass *klass)
{
}

/**
 * gtuber_adaptive_stream_new:
 *
 * Creates a new #GtuberAdaptiveStream instance.
 *
 * This is mainly useful for plugin development.
 *
 * Returns: (transfer full): a new #GtuberAdaptiveStream instance.
 */
GtuberAdaptiveStream *
gtuber_adaptive_stream_new (void)
{
  return g_object_new (GTUBER_TYPE_ADAPTIVE_STREAM, NULL);
}

/**
 * gtuber_adaptive_stream_get_init_range:
 * @stream: a #GtuberAdaptiveStream
 * @start: (out) (optional): the start of stream init range
 * @end: (out) (optional): the end of stream init range
 *
 * Gets the byte range of stream initialization segment.
 *
 * Returns: %TRUE if successful, with the out parameters set, %FALSE otherwise.
 **/
gboolean
gtuber_adaptive_stream_get_init_range (GtuberAdaptiveStream *self,
    guint64 *start, guint64 *end)
{
  g_return_val_if_fail (GTUBER_IS_ADAPTIVE_STREAM (self), FALSE);

  if (self->init_end <= self->init_start)
    return FALSE;

  if (start)
    *start = self->init_start;
  if (end)
    *end = self->init_end;

  return TRUE;
}

/**
 * gtuber_adaptive_stream_set_init_range:
 * @stream: a #GtuberAdaptiveStream
 * @start: the start of stream init range
 * @end: the end of stream init range
 *
 * Sets the byte range of stream initialization segment.
 **/
void
gtuber_adaptive_stream_set_init_range (GtuberAdaptiveStream *self,
    guint64 start, guint64 end)
{
  g_return_if_fail (GTUBER_IS_ADAPTIVE_STREAM (self));

  self->init_start = start;
  self->init_end = end;
}

/**
 * gtuber_adaptive_stream_get_index_range:
 * @stream: a #GtuberAdaptiveStream
 * @start: (out) (optional): the start of stream index range
 * @end: (out) (optional): the end of stream index range
 *
 * Gets the byte range of stream media segment.
 *
 * Returns: %TRUE if successful, with the out parameters set, %FALSE otherwise.
 **/
gboolean
gtuber_adaptive_stream_get_index_range (GtuberAdaptiveStream *self,
    guint64 *start, guint64 *end)
{
  g_return_val_if_fail (GTUBER_IS_ADAPTIVE_STREAM (self), FALSE);

  if (self->index_end <= self->index_start)
    return FALSE;

  if (start)
    *start = self->index_start;
  if (end)
    *end = self->index_end;

  return TRUE;
}

/**
 * gtuber_adaptive_stream_set_index_range:
 * @stream: a #GtuberAdaptiveStream
 * @start: the start of stream index range
 * @end: the end of stream index range
 *
 * Sets the byte range of stream media segment.
 **/
void
gtuber_adaptive_stream_set_index_range (GtuberAdaptiveStream *self,
    guint64 start, guint64 end)
{
  g_return_if_fail (GTUBER_IS_ADAPTIVE_STREAM (self));

  self->index_start = start;
  self->index_end = end;
}
