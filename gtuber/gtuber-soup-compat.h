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

#ifndef __GTUBER_SOUP_COMPAT_H__
#define __GTUBER_SOUP_COMPAT_H__

#include <gio/gio.h>
#include <libsoup/soup.h>

#if !SOUP_CHECK_VERSION (2, 99, 1)

G_BEGIN_DECLS

SoupMessageHeaders * soup_message_get_request_headers (SoupMessage *msg);

void soup_message_set_request_body (SoupMessage *msg, const char *content_type, GInputStream *stream, gssize content_length);

G_END_DECLS

#endif /* SOUP_CHECK_VERSION */
#endif /* __GTUBER_SOUP_COMPAT_H__ */
