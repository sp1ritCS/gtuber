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

#include <json-glib/json-glib.h>

#include "gtuber-lbry.h"
#include "utils/common/gtuber-utils-common.h"
#include "utils/json/gtuber-utils-json.h"

struct _GtuberLbry
{
  GtuberWebsite parent;

  gchar *video_id;
  gchar *hls_uri;
  gboolean parsed_hls;
};

struct _GtuberLbryClass
{
  GtuberWebsiteClass parent_class;
};

#define parent_class gtuber_lbry_parent_class
G_DEFINE_TYPE (GtuberLbry, gtuber_lbry, GTUBER_TYPE_WEBSITE)

static void gtuber_lbry_finalize (GObject *object);

static GtuberFlow gtuber_lbry_create_request (GtuberWebsite *website,
    GtuberMediaInfo *info, SoupMessage **msg, GError **error);
static GtuberFlow gtuber_lbry_parse_input_stream (GtuberWebsite *website,
    GInputStream *stream, GtuberMediaInfo *info, GError **error);

static void
gtuber_lbry_init (GtuberLbry *self)
{
  self->hls_uri = NULL;
}

static void
gtuber_lbry_class_init (GtuberLbryClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GtuberWebsiteClass *website_class = (GtuberWebsiteClass *) klass;

  gobject_class->finalize = gtuber_lbry_finalize;

  website_class->handles_input_stream = TRUE;
  website_class->create_request = gtuber_lbry_create_request;
  website_class->parse_input_stream = gtuber_lbry_parse_input_stream;
}

static void
gtuber_lbry_finalize (GObject *object)
{
  GtuberLbry *self = GTUBER_LBRY (object);

  g_debug ("Lbry finalize");

  g_free (self->video_id);
  g_free (self->hls_uri);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static GtuberFlow
acquire_hls_uri (GtuberLbry *self, JsonReader *reader, GError **error)
{
  const gchar *hls_uri;

  g_debug ("Searching for HLS URI...");

  hls_uri = gtuber_utils_json_get_string (reader, "result", "streaming_url", NULL);
  if (!hls_uri) {
    g_set_error (error, GTUBER_WEBSITE_ERROR,
        GTUBER_WEBSITE_ERROR_PARSE_FAILED,
        "Streaming URL is missing");
    return GTUBER_FLOW_ERROR;
  }

  self->hls_uri = g_strdup (hls_uri);
  g_debug ("Got HLS URI: %s", self->hls_uri);

  return GTUBER_FLOW_RESTART;
}

static GtuberFlow
parse_hls (GtuberLbry *self, GInputStream *stream,
    GtuberMediaInfo *info, GError **error)
{
  gboolean success;

  g_debug ("Parsing LBRY HLS...");

  success = gtuber_utils_common_parse_hls_input_stream (stream, info, error);
  self->parsed_hls = TRUE;

  g_debug ("HLS parsed, success: %s", success ? "YES" : "NO");

  return (success)
      ? GTUBER_FLOW_RESTART
      : GTUBER_FLOW_ERROR;
}

static GtuberFlow
fill_media_info (GtuberLbry *self, JsonReader *reader,
    GtuberMediaInfo *info, GError **error)
{
  g_debug ("Filling media info...");

  if (!gtuber_utils_json_go_to (reader, "result", self->video_id, "value", NULL)) {
    g_set_error (error, GTUBER_WEBSITE_ERROR,
        GTUBER_WEBSITE_ERROR_PARSE_FAILED,
        "Invalid website API response");
    return GTUBER_FLOW_ERROR;
  }

  gtuber_media_info_set_title (info,
      gtuber_utils_json_get_string (reader, "title", NULL));
  gtuber_media_info_set_description (info,
      gtuber_utils_json_get_string (reader, "description", NULL));

  if (gtuber_utils_json_go_to (reader, "video", NULL)) {
    gtuber_media_info_set_duration (info,
        gtuber_utils_json_get_int (reader, "duration", NULL));

    gtuber_utils_json_go_back (reader, 1);
  }

  /* Return from "result.video_id.value" */
  gtuber_utils_json_go_back (reader, 3);
  g_debug ("Media info filled");

  return GTUBER_FLOW_OK;
}

static GtuberFlow
gtuber_lbry_create_request (GtuberWebsite *website,
    GtuberMediaInfo *info, SoupMessage **msg, GError **error)
{
  GtuberLbry *self = GTUBER_LBRY (website);
  gchar *req_body;

  if (self->hls_uri && !self->parsed_hls) {
    *msg = soup_message_new ("GET", self->hls_uri);
    return GTUBER_FLOW_OK;
  }

  req_body = g_strdup_printf (
      "{"
      "  \"method\": \"%s\","
      "  \"params\": {"
      "    \"%s\": \"%s\""
      "  }"
      "}",
      (self->hls_uri == NULL) ? "get" : "resolve",
      (self->hls_uri == NULL) ? "uri" : "urls",
      self->video_id);

  *msg = soup_message_new ("POST", "https://api.na-backend.odysee.com/api/v1/proxy");
  gtuber_utils_common_msg_take_request (*msg, "application/json-rpc", req_body);

  return GTUBER_FLOW_OK;
}

static GtuberFlow
gtuber_lbry_parse_input_stream (GtuberWebsite *website,
    GInputStream *stream, GtuberMediaInfo *info, GError **error)
{
  GtuberLbry *self = GTUBER_LBRY (website);
  JsonParser *parser;
  JsonReader *reader;
  GtuberFlow flow = GTUBER_FLOW_ERROR;

  if (self->hls_uri && !self->parsed_hls)
    return parse_hls (self, stream, info, error);

  parser = json_parser_new ();
  json_parser_load_from_stream (parser, stream, NULL, error);
  if (*error)
    goto finish;

  gtuber_utils_json_parser_debug (parser);
  reader = json_reader_new (json_parser_get_root (parser));

  flow = (!self->hls_uri)
      ? acquire_hls_uri (self, reader, error)
      : fill_media_info (self, reader, info, error);

  g_object_unref (reader);

finish:
  g_object_unref (parser);

  if (*error)
    return GTUBER_FLOW_ERROR;

  return flow;
}

GtuberWebsite *
query_plugin (GUri *uri)
{
  guint uri_match;
  const gchar *path;

/*
1. API resolve - check if video or audio stream (store width/height)
2. API get - obtain initial URI
3. HEAD of URI - check redirect and if is m3u8
 HLS:
  4.1. GET URI
  4.2. parse m3u8
 Direct:
  4.1. add as stream with resolved URI (and set stored width/height)
*/

  if (!gtuber_utils_common_uri_matches_hosts (uri, &uri_match,
      "odysee.com",
      NULL))
    return NULL;

  path = g_uri_get_path (uri);

  /* Safety check if path starts from "@" char */
  if (path && g_str_has_prefix (path, "/@")) {
    GtuberLbry *lbry;
    gchar *video_path;

    video_path = g_uri_join (G_URI_FLAGS_ENCODED, NULL,
        NULL, NULL, -1, path, NULL, g_uri_get_fragment (uri));

    lbry = g_object_new (GTUBER_TYPE_LBRY, NULL);
    lbry->video_id = g_strdup (video_path + 1);

    g_debug ("Requested video: %s", lbry->video_id);
    g_free (video_path);

    return GTUBER_WEBSITE (lbry);
  }

  return NULL;
}
