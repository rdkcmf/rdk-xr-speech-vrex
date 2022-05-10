/*
##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2019 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <xrsv_ws_private.h>

#define XRSV_WS_JSON_KEY_API_VERSION                   "apiVersion"
#define XRSV_WS_JSON_KEY_APP_ID                        "appId"
#define XRSV_WS_JSON_KEY_REQ_TYPE                      "requestType"
#define XRSV_WS_JSON_KEY_PARTNER_ID                    "partnerId"
#define XRSV_WS_JSON_KEY_EXPERIENCE                    "experienceTag"
#define XRSV_WS_JSON_KEY_ACCOUNT_ID                    "serviceAccountId"
#define XRSV_WS_JSON_KEY_STB_DEVICE_ID                 "voiceDeviceId"
#define XRSV_WS_JSON_KEY_DEVICE_NAME                   "deviceName"
#define XRSV_WS_JSON_KEY_DEVICE_MODEL                  "deviceModel"
#define XRSV_WS_JSON_KEY_DEVICE_MAC                    "deviceMAC"
#define XRSV_WS_JSON_KEY_DEVICE_TYPE                   "deviceType"
#define XRSV_WS_JSON_KEY_DEVICE_SOFTWARE               "deviceSwVersion"
#define XRSV_WS_JSON_KEY_DEVICE_ID                     "deviceId"
#define XRSV_WS_JSON_KEY_CODEC                         "codec"
#define XRSV_WS_JSON_KEY_LANGUAGE                      "language"
#define XRSV_WS_JSON_KEY_CONV_MODE                     "conversationMode"
#define XRSV_WS_JSON_KEY_SOFTWARE_VER                  "stbSwVersion"
#define XRSV_WS_JSON_KEY_IP_ADDR_V4                    "Xr18IpAddr"
#define XRSV_WS_JSON_KEY_IP_ADDR_V6                    "Xr18V6IpAddr"
#define XRSV_WS_JSON_KEY_SSID                          "Xr18Ssid"
#define XRSV_WS_JSON_KEY_PSTN                          "Xr18Pstn"
#define XRSV_WS_JSON_KEY_REASON                        "reason"
#define XRSV_WS_JSON_KEY_FEATURE                       "activateFeature"
#define XRSV_WS_JSON_KEY_PTT                           "Push-to-Talk"
#define XRSV_WS_JSON_KEY_SOWUW                         "SOWUW"
#define XRSV_WS_JSON_KEY_EOWUW                         "EOWUW"
#define XRSV_WS_JSON_KEY_DOA                           "DOA"
#define XRSV_WS_JSON_KEY_DYNAMIC_GAIN                  "gain"
#define XRSV_WS_JSON_KEY_KW_CONFIDENCE_LINEAR          "keywordConfidenceLinear"
#define XRSV_WS_JSON_KEY_KW_CONFIDENCE_NONLINEAR       "keywordConfidenceNonLinear"
#define XRSV_WS_JSON_KEY_KW_SENSITIVITY                "keywordSensitivity"
#define XRSV_WS_JSON_KEY_KW_SENSITIVITY_TRIGGERED      "keywordSensitivityTriggered"
#define XRSV_WS_JSON_KEY_KW_SENSITIVITY_HIGH           "keywordSensitivityHigh"
#define XRSV_WS_JSON_KEY_KW_SENSITIVITY_HIGH_TRIGGERED "keywordSensitivityHighTriggered"
#define XRSV_WS_JSON_KEY_LAST_CMD_TIMES                "lastCommandTimes"
#define XRSV_WS_JSON_KEY_LAST_CMD_TRANSCRIPTION        "lastCommandTranscription"
#define XRSV_WS_JSON_KEY_FOLLOW_UP                     "isFollowUpCommand"
#define XRSV_WS_JSON_KEY_SIGNAL_NOISE_RATIO            "signalNoiseRatio"
#define XRSV_WS_JSON_KEY_KW_GAIN                       "keywordGain"
#define XRSV_WS_JSON_KEY_TRX                           "trx"
#define XRSV_WS_JSON_KEY_FOLLOW_UP                     "isFollowUpCommand"

#define XRSV_WS_IDENTIFIER (0xA32D57CC)

static bool     xrsv_ws_object_is_valid(xrsv_ws_obj_t *obj);
static void     xrsv_ws_msg_init(xrsv_ws_obj_t *obj, uint8_t **buffer, uint32_t *length);
static void     xrsv_ws_msg_stream_end(xrsv_ws_obj_t *obj, int32_t reason, uint8_t **buffer, uint32_t *length);
static bool     xrsv_ws_msg_decode(xrsv_ws_obj_t *obj, json_t *obj_json);
static json_t * xrsv_ws_phone_call_obj_get(json_t *obj);
static uint64_t xrsv_ws_time_get(void);
static bool     xrsv_ws_update_init_json(xrsv_ws_obj_t *obj, const char *key, json_t *value);
static bool     xrsv_ws_update_init_json_str(xrsv_ws_obj_t *obj, const char *key, const char *value);
static void     xrsv_ws_update_init_last_command_times(xrsv_ws_obj_t *obj);

static void xrsv_ws_handler_ws_source_error(void *data, xrsr_src_t src);
static void xrsv_ws_handler_ws_session_begin(void *data, const uuid_t uuid, xrsr_src_t src, uint32_t dst_index, xrsr_keyword_detector_result_t *detector_result, xrsr_session_config_out_t *config_out, xrsr_session_config_in_t *config_in, rdkx_timestamp_t *timestamp, const char *transcription_in);
static void xrsv_ws_handler_ws_session_config(void *data, const uuid_t uuid, xrsr_session_config_in_t *config_in);
static void xrsv_ws_handler_ws_session_end(void *data, const uuid_t uuid, xrsr_session_stats_t *stats, rdkx_timestamp_t *timestamp);
static void xrsv_ws_handler_ws_stream_begin(void *data, const uuid_t uuid, xrsr_src_t src, rdkx_timestamp_t *timestamp);
static void xrsv_ws_handler_ws_stream_kwd(void *data, const uuid_t uuid, rdkx_timestamp_t *timestamp);
static void xrsv_ws_handler_ws_stream_end(void *data, const uuid_t uuid, xrsr_stream_stats_t *stats, rdkx_timestamp_t *timestamp);
static bool xrsv_ws_handler_ws_connected(void *data, const uuid_t uuid, xrsr_handler_send_t send, void *param, rdkx_timestamp_t *timestamp);
static void xrsv_ws_handler_ws_disconnected(void *data, const uuid_t uuid, xrsr_session_end_reason_t reason, bool retry, bool *detect_resume, rdkx_timestamp_t *timestamp);
static bool xrsv_ws_handler_ws_recv_msg(void *data, xrsr_recv_msg_t type, const uint8_t *buffer, uint32_t length);

bool xrsv_ws_object_is_valid(xrsv_ws_obj_t *obj) {
   if(obj != NULL && obj->identifier == XRSV_WS_IDENTIFIER) {
      return(true);
   }
   return(false);
}

xrsv_ws_object_t xrsv_ws_create(const xrsv_ws_params_t *params) {
   if(params == NULL) {
      XLOGD_ERROR("invalid params");
      return(NULL);
   }
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)malloc(sizeof(xrsv_ws_obj_t));

   if(obj == NULL) {
      XLOGD_ERROR("Out of memory.");
      return(NULL);
   }

   memset(obj, 0, sizeof(*obj));

   json_t *obj_init = json_object();
   int rc;
   
   if(obj_init == NULL) {
      XLOGD_ERROR("object create failed");
      free(obj);
      return(NULL);
   }
   const char *codec = "PCM_16_16K";

   rc  = json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_API_VERSION,   json_string("v2"));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_APP_ID,        json_string(params->app_id));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_REQ_TYPE,      json_string("init"));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_PARTNER_ID,    json_string(params->partner_id));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_EXPERIENCE,    json_string(params->experience));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_ACCOUNT_ID,    json_string(params->account_id));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_STB_DEVICE_ID, json_string(params->stb_device_id));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DEVICE_NAME,   json_string(params->device_name));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DEVICE_MODEL,  json_string(params->device_name));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DEVICE_MAC,    json_string(params->device_mac));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DEVICE_TYPE,   json_string(params->device_type));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DEVICE_ID,     json_string(params->device_id));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_CODEC,         json_string(codec));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_LANGUAGE,      json_string(params->language));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_CONV_MODE,     json_string("false"));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_SOFTWARE_VER,  json_string(params->software_version));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_IP_ADDR_V4,    json_string(params->ip_addr_v4));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_IP_ADDR_V6,    json_string(params->ip_addr_v6));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_SSID,          json_string(params->ssid));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_PSTN,          json_string(params->pstn));
   if(params->test_flag) {
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_FEATURE,       json_string("test"));
   }

   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_FOLLOW_UP,     json_false()); // TODO
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_LAST_CMD_TRANSCRIPTION, json_string(""));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_LAST_CMD_TIMES, json_array());
   
   if(rc != 0) {
      XLOGD_ERROR("object set failed");
      json_decref(obj_init);
      free(obj);
      return(NULL);
   }
   
   json_t *obj_stream_end = json_object();

   if(obj_stream_end == NULL) {
      XLOGD_ERROR("object create failed");
      json_decref(obj_init);
      free(obj);
      return(NULL);
   }

   rc  = json_object_set_new_nocheck(obj_stream_end, XRSV_WS_JSON_KEY_REQ_TYPE, json_string("end_of_stream"));
   rc |= json_object_set_new_nocheck(obj_stream_end, XRSV_WS_JSON_KEY_REASON,   json_integer(0));

   if(rc != 0) {
      XLOGD_ERROR("object set failed");
      json_decref(obj_init);
      json_decref(obj_stream_end);
      free(obj);
      return(NULL);
   }

   if(params->device_id != NULL) {
      rc = snprintf(obj->query_element_device_id,     sizeof(obj->query_element_device_id), "deviceId=%s",    params->device_id);
      if(rc >= sizeof(obj->query_element_device_id)) {
         XLOGD_WARN("truncated device id <%d>", rc);
      }
      rc = snprintf(obj->query_element_device_id_xbo, sizeof(obj->query_element_device_id), "xboDeviceId=%s", params->device_id);
      if(rc >= sizeof(obj->query_element_device_id_xbo)) {
         XLOGD_WARN("truncated xbo device id <%d>", rc);
      }
   }
   if(params->receiver_id) {
      rc = snprintf(obj->query_element_receiver_id,   sizeof(obj->query_element_receiver_id), "receiverId=%s", params->receiver_id);
      if(rc >= sizeof(obj->query_element_receiver_id)) {
         XLOGD_WARN("truncated receiver id <%d>", rc);
      }
   }
   if(params->app_id != NULL) {
      rc = snprintf(obj->query_element_app_id, sizeof(obj->query_element_app_id), "appId=%s", params->app_id);
      if(rc >= sizeof(obj->query_element_app_id)) {
         XLOGD_WARN("truncated app id <%d>", rc);
      }
   }
   if(params->partner_id != NULL) {
      rc = snprintf(obj->query_element_partner_id, sizeof(obj->query_element_partner_id), "partnerId=%s", params->partner_id);
      if(rc >= sizeof(obj->query_element_partner_id)) {
         XLOGD_WARN("truncated partner id <%d>", rc);
      }
   }
   if(params->experience != NULL) {
      rc = snprintf(obj->query_element_experience, sizeof(obj->query_element_experience), "experienceTag=%s", params->experience);
      if(rc >= sizeof(obj->query_element_experience)) {
         XLOGD_WARN("truncated experience id <%d>", rc);
      }
   }
   if(params->language != NULL) {
      rc = snprintf(obj->query_element_language, sizeof(obj->query_element_language), "language=%s", params->language);
      if(rc >= sizeof(obj->query_element_language)) {
         XLOGD_WARN("truncated language <%d>", rc);
      }
   }
   rc = snprintf(obj->query_element_codec, sizeof(obj->query_element_codec), "codec=%s", codec);
   if(rc >= sizeof(obj->query_element_codec)) {
      XLOGD_WARN("truncated codec <%d>", rc);
   }
   rc = snprintf(obj->query_element_aspect_ratio, sizeof(obj->query_element_aspect_ratio), "aspectRatio=HD");
   if(rc >= sizeof(obj->query_element_aspect_ratio)) {
      XLOGD_WARN("truncated aspect ratio <%d>", rc);
   }

   obj->obj_init       = obj_init;
   obj->obj_stream_end = obj_stream_end;
   obj->identifier     = XRSV_WS_IDENTIFIER;
   obj->user_data      = params->user_data;

   return(obj);
}
bool xrsv_ws_handlers(xrsv_ws_object_t object, const xrsv_ws_handlers_t *handlers_in, xrsr_handlers_t *handlers_out) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }

   bool ret = true;
   handlers_out->data           = obj;
   handlers_out->source_error   = xrsv_ws_handler_ws_source_error;
   handlers_out->session_begin  = xrsv_ws_handler_ws_session_begin;
   handlers_out->session_config = xrsv_ws_handler_ws_session_config;
   handlers_out->session_end    = xrsv_ws_handler_ws_session_end;
   handlers_out->stream_begin   = xrsv_ws_handler_ws_stream_begin;
   handlers_out->stream_kwd     = xrsv_ws_handler_ws_stream_kwd;
   handlers_out->stream_end     = xrsv_ws_handler_ws_stream_end;
   handlers_out->connected      = xrsv_ws_handler_ws_connected;
   handlers_out->disconnected   = xrsv_ws_handler_ws_disconnected;
   handlers_out->recv_msg       = xrsv_ws_handler_ws_recv_msg;

   obj->handlers = *handlers_in;
   return(ret);
}

bool xrsv_ws_update_user_data(xrsv_ws_object_t object, void *user_data) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   obj->user_data = user_data;
   return(true);
}

bool xrsv_ws_update_init_json(xrsv_ws_obj_t *obj, const char *key, json_t *value) {
   if(key == NULL || value == NULL) {
      XLOGD_ERROR("invalid params");
      return(false);
   }
   json_t *obj_json = obj->obj_init;

   // Update the value
   int rc  = json_object_set_new_nocheck(obj_json, key, value);

   if(rc != 0) {
      XLOGD_ERROR("object set failed");
      return(false);
   }
   return(true);
}

bool xrsv_ws_update_init_json_str(xrsv_ws_obj_t *obj, const char *key, const char *value) {
   if(key == NULL || value == NULL) {
      XLOGD_ERROR("invalid params");
      return(false);
   }
   return(xrsv_ws_update_init_json(obj, key, json_string(value)));
}

bool xrsv_ws_update_device_id(xrsv_ws_object_t object, const char *device_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool rv = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_ID, device_id);

   if(rv) {
      int rc = snprintf(obj->query_element_device_id, sizeof(obj->query_element_device_id), "deviceId=%s", device_id);
      if(rc >= sizeof(obj->query_element_device_id)) {
         XLOGD_WARN("truncated device id <%d>", rc);
      }
      rc = snprintf(obj->query_element_device_id_xbo, sizeof(obj->query_element_device_id_xbo), "xboDeviceId=%s", device_id);
      if(rc >= sizeof(obj->query_element_device_id_xbo)) {
         XLOGD_WARN("truncated xbo device id <%d>", rc);
      }
   }
   return(rv);
}

bool xrsv_ws_update_receiver_id(xrsv_ws_object_t object, const char *receiver_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   int rc = snprintf(obj->query_element_receiver_id, sizeof(obj->query_element_receiver_id), "receiverId=%s", receiver_id);
   if(rc >= sizeof(obj->query_element_receiver_id)) {
      XLOGD_WARN("truncated receiver id <%d>", rc);
   }
   return(true);
}

bool xrsv_ws_update_device_mac(xrsv_ws_object_t object, const char *device_mac) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_MAC, device_mac));
}

bool xrsv_ws_update_account_id(xrsv_ws_object_t object, const char *account_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_ACCOUNT_ID, account_id));
}

bool xrsv_ws_update_partner_id(xrsv_ws_object_t object, const char *partner_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool rv = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_PARTNER_ID, partner_id);
   if(rv) {
      int rc = snprintf(obj->query_element_partner_id, sizeof(obj->query_element_partner_id), "partnerId=%s", partner_id);
      if(rc >= sizeof(obj->query_element_partner_id)) {
         XLOGD_WARN("truncated partner id <%d>", rc);
      }
   }
   return(rv);
}
 
bool xrsv_ws_update_experience(xrsv_ws_object_t object, const char *experience) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool rv = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_EXPERIENCE, experience);
   if(rv) {
      int rc = snprintf(obj->query_element_experience, sizeof(obj->query_element_experience), "experienceTag=%s", experience);
      if(rc >= sizeof(obj->query_element_experience)) {
         XLOGD_WARN("truncated experience id <%d>", rc);
      }
   }
   return(rv);
}

bool xrsv_ws_update_stb_device_id(xrsv_ws_object_t object, const char *stb_device_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_STB_DEVICE_ID, stb_device_id));
}

bool xrsv_ws_update_device_name(xrsv_ws_object_t object, const char *device_name) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool ret_name = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_NAME, device_name);
   bool ret_model = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_MODEL, device_name);
   return(ret_name && ret_model);
}

bool xrsv_ws_update_device_type(xrsv_ws_object_t object, const char *device_type) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_TYPE, device_type));
}

bool xrsv_ws_update_device_software(xrsv_ws_object_t object, const char *device_software) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_DEVICE_SOFTWARE, device_software));
}

bool xrsv_ws_update_app_id(xrsv_ws_object_t object, const char *app_id) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool rv = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_APP_ID, app_id);
   if(rv) {
      int rc = snprintf(obj->query_element_app_id, sizeof(obj->query_element_app_id), "appId=%s", app_id);
      if(rc >= sizeof(obj->query_element_app_id)) {
         XLOGD_WARN("truncated app id <%d>", rc);
      }
   }
   return(rv);
}

bool xrsv_ws_update_software_version(xrsv_ws_object_t object, const char *software_version) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_SOFTWARE_VER, software_version));
}

bool xrsv_ws_update_ip_addr(xrsv_ws_object_t object, const char *ip_addr_v4, const char *ip_addr_v6) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool ret_v4 = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_IP_ADDR_V4, ip_addr_v4);
   bool ret_v6 = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_IP_ADDR_V6, ip_addr_v6);
   return(ret_v4 && ret_v6);
}

bool xrsv_ws_update_ssid(xrsv_ws_object_t object, const char *ssid) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_SSID, ssid));
}

bool xrsv_ws_update_pstn(xrsv_ws_object_t object, const char *pstn) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_PSTN, pstn));
}

bool xrsv_ws_update_language(xrsv_ws_object_t object, const char *language) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   bool rv = xrsv_ws_update_init_json_str(obj, XRSV_WS_JSON_KEY_LANGUAGE, language);
   if(rv) {
      int rc = snprintf(obj->query_element_language, sizeof(obj->query_element_language), "language=%s", language);
      if(rc >= sizeof(obj->query_element_language)) {
         XLOGD_WARN("truncated language <%d>", rc);
      }
   }
   return(rv);
}

bool xrsv_ws_update_session_params(xrsv_ws_object_t object, const char *call_state) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   return(xrsv_ws_update_init_json_str(obj, "callState", call_state));
}

void xrsv_ws_destroy(xrsv_ws_object_t object) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)object;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   XLOGD_INFO("");

   if(obj->obj_init != NULL) {
      XLOGD_INFO("free init object");
      json_decref(obj->obj_init);
      obj->obj_init = NULL;
   }
   if(obj->obj_stream_end != NULL) {
      XLOGD_INFO("free stream end object");
      json_decref(obj->obj_stream_end);
      obj->obj_stream_end = NULL;
   }
   obj->query_element_device_id[0]     = '\0';
   obj->query_element_device_id_xbo[0] = '\0';
   obj->query_element_receiver_id[0]   = '\0';
   obj->query_element_codec[0]         = '\0';
   obj->query_element_trx[0]           = '\0';
   obj->query_element_app_id[0]        = '\0';
   obj->query_element_partner_id[0]    = '\0';
   obj->query_element_experience[0]    = '\0';
   obj->query_element_language[0]      = '\0';
   obj->query_element_aspect_ratio[0]  = '\0';
   obj->identifier                     = 0;
   free(obj);
}

void xrsv_ws_handler_ws_source_error(void *data, xrsr_src_t src) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }

   if(obj->handlers.source_error != NULL) {
      (*obj->handlers.source_error)(src, obj->user_data);
   }
}

void xrsv_ws_handler_ws_session_begin(void *data, const uuid_t uuid, xrsr_src_t src, uint32_t dst_index, xrsr_keyword_detector_result_t *detector_result, xrsr_session_config_out_t *config_out, xrsr_session_config_in_t *config_in, rdkx_timestamp_t *timestamp, const char *transcription_in) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   if (transcription_in != NULL) {
      XLOGD_ERROR("%s: Voice session by text not supported for this handler.", __FUNCTION__);
   }

   char uuid_str[37] = {'\0'};
   json_t *obj_init = obj->obj_init;
   xrsv_ws_stream_params_t stream_params;

   if(detector_result != NULL) {
      stream_params.keyword_sample_begin               = detector_result->offset_kwd_begin - detector_result->offset_buf_begin;
      stream_params.keyword_sample_end                 = detector_result->offset_kwd_end   - detector_result->offset_buf_begin;
      stream_params.keyword_doa                        = detector_result->doa;
      stream_params.keyword_sensitivity                = 0;
      stream_params.keyword_sensitivity_triggered      = false;
      stream_params.keyword_sensitivity_high           = 0;
      stream_params.keyword_sensitivity_high_support   = false;
      stream_params.keyword_sensitivity_high_triggered = false;
      stream_params.keyword_gain                       = detector_result->kwd_gain;
      stream_params.dynamic_gain                       = detector_result->dynamic_gain;
      stream_params.linear_confidence                  = 0.0;
      stream_params.nonlinear_confidence               = 0;
      stream_params.signal_noise_ratio                 = detector_result->snr; // if NULL 255.0 is invalid value;
      stream_params.push_to_talk                       = false;
   }

   obj->user_initiated = config_out->user_initiated;

   if(obj->handlers.session_begin != NULL) {
      (*obj->handlers.session_begin)(uuid, src, dst_index, config_out, (detector_result == NULL) ? NULL : &stream_params, timestamp, obj->user_data);
   }

   uuid_unparse_lower(uuid, uuid_str);
   int rc = json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_TRX,   json_string(uuid_str));

   if(rc != 0) {
      XLOGD_ERROR("object set failed");
   }

   // Add attribute-value pairs to query string
   snprintf(obj->query_element_trx, sizeof(obj->query_element_trx), "trx=%s", uuid_str);

   config_in->ws.query_strs[0] = obj->query_element_device_id;
   config_in->ws.query_strs[1] = obj->query_element_codec;
   config_in->ws.query_strs[2] = obj->query_element_trx;
   config_in->ws.query_strs[3] = NULL;

   obj->current_command_time = time(NULL);
}

void xrsv_ws_handler_ws_session_config(void *data, const uuid_t uuid, xrsr_session_config_in_t *config_in) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   int rc;
   json_t *obj_init = obj->obj_init;

   if(config_in == NULL || config_in->ws.app_config == NULL) {
      XLOGD_ERROR("invalid stream params <%p>", config_in);
      return;
   }

   xrsv_ws_stream_params_t *stream_params = (xrsv_ws_stream_params_t *)config_in->ws.app_config;

   rc  = json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_PTT,   json_string((obj->user_initiated || stream_params->push_to_talk) ? "TRUE" : "FALSE"));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_SOWUW, json_integer(stream_params->keyword_sample_begin));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_EOWUW, json_integer(stream_params->keyword_sample_end));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DOA,   json_integer(stream_params->keyword_doa));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_DYNAMIC_GAIN,  json_real(stream_params->dynamic_gain));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_SIGNAL_NOISE_RATIO, json_real(stream_params->signal_noise_ratio));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_SENSITIVITY, json_integer(stream_params->keyword_sensitivity));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_SENSITIVITY_TRIGGERED, (stream_params->keyword_sensitivity_triggered ? json_true() : json_false()));
   if(stream_params->keyword_sensitivity_high_support) {
      rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_SENSITIVITY_HIGH,           json_integer(stream_params->keyword_sensitivity_high));
      rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_SENSITIVITY_HIGH_TRIGGERED, (stream_params->keyword_sensitivity_high_triggered ? json_true() : json_false()));
   }
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_CONFIDENCE_LINEAR, json_real(stream_params->linear_confidence));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_CONFIDENCE_NONLINEAR, json_integer(stream_params->nonlinear_confidence));
   rc |= json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_KW_GAIN,  json_real(stream_params->keyword_gain));

   if(rc != 0) {
      XLOGD_ERROR("object set failed");
   }

   free(stream_params);
}

void xrsv_ws_handler_ws_session_end(void *data, const uuid_t uuid, xrsr_session_stats_t *stats, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   if(obj->handlers.session_end != NULL) {
      (*obj->handlers.session_end)(uuid, stats, timestamp, obj->user_data);
   }
   xrsv_ws_update_init_last_command_times(obj);
   obj->query_element_trx[0] = '\0';
}

void xrsv_ws_handler_ws_stream_begin(void *data, const uuid_t uuid, xrsr_src_t src, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   if(obj->handlers.stream_begin != NULL) {
      (*obj->handlers.stream_begin)(uuid, src, timestamp, obj->user_data);
   }
}

void xrsv_ws_handler_ws_stream_kwd(void *data, const uuid_t uuid, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   if(obj->handlers.stream_kwd != NULL) {
      (*obj->handlers.stream_kwd)(uuid, timestamp, obj->user_data);
   }
}

void xrsv_ws_handler_ws_stream_end(void *data, const uuid_t uuid, xrsr_stream_stats_t *stats, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   if(obj->send != NULL) { // Create 'end of stream' message.  Note: This may need to be done in the application's context
      uint8_t *buffer = NULL;
      uint32_t length = 0;
      xrsv_ws_msg_stream_end(obj, 0, &buffer, &length);

      if(buffer == NULL || length == 0) {
         XLOGD_ERROR("invalid message");
      } else {
         XLOGD_INFO("msg stream end <%s>", buffer);

         xrsr_result_t result = (*obj->send)(obj->param, buffer, length);
         free(buffer);

         if(result != XRSR_RESULT_SUCCESS) {
            XLOGD_ERROR("result <%s>", xrsr_result_str(result));
         }
      }
   }

   if(obj->handlers.stream_end != NULL) {
      (*obj->handlers.stream_end)(uuid, stats, timestamp, obj->user_data);
   }
}

bool xrsv_ws_handler_ws_connected(void *data, const uuid_t uuid, xrsr_handler_send_t send, void *param, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return false;
   }
   if(obj->handlers.connected != NULL) {
      (*obj->handlers.connected)(uuid, timestamp, obj->user_data);
   }

   // Store connection send function and param
   obj->send  = send;
   obj->param = param;
   
   // Create speech init message
   uint8_t *buffer = NULL;
   uint32_t length = 0;
   xrsv_ws_msg_init(obj, &buffer, &length);
   
   if(buffer == NULL || length == 0) {
      XLOGD_ERROR("invalid message");
      return false;
   }
   XLOGD_INFO("msg init <%s>", buffer);
   
   xrsr_result_t result = (*send)(param, buffer, length);
   free(buffer);

   if(result != XRSR_RESULT_SUCCESS) {
      XLOGD_ERROR("result <%s>", xrsr_result_str(result));
   }
   if(obj->handlers.sent_init != NULL) {
      rdkx_timestamp_t ts_sent_init;
      rdkx_timestamp_get_realtime(&ts_sent_init);
      (*obj->handlers.sent_init)(uuid, &ts_sent_init, obj->user_data);
   }
   return true;
}

void xrsv_ws_handler_ws_disconnected(void *data, const uuid_t uuid, xrsr_session_end_reason_t reason, bool retry, bool *detect_resume, rdkx_timestamp_t *timestamp) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return;
   }
   obj->send  = NULL;
   obj->param = NULL;
   if(detect_resume != NULL) {
      *detect_resume = true;
   }
   if(obj->handlers.disconnected != NULL) {
      (*obj->handlers.disconnected)(uuid, retry, timestamp, obj->user_data);
   }
}

bool xrsv_ws_handler_ws_recv_msg(void *data, xrsr_recv_msg_t type, const uint8_t *buffer, uint32_t length) {
   xrsv_ws_obj_t *obj = (xrsv_ws_obj_t *)data;
   if(!xrsv_ws_object_is_valid(obj)) {
      XLOGD_ERROR("invalid object");
      return(false);
   }
   XLOGD_INFO("type <%s> length <%u>", xrsr_recv_msg_str(type), length);

   json_error_t error;
   json_t *obj_json = json_loads((const char *)buffer, JSON_REJECT_DUPLICATES, &error);
   if(obj_json == NULL) {
      XLOGD_ERROR("invalid json");
      return(false);
   } else if(!json_is_object(obj_json)) {
      XLOGD_ERROR("json object not found");
      return(false);
   }
   bool retval = xrsv_ws_msg_decode(obj, obj_json);
   json_decref(obj_json);
   return(retval);
}


void xrsv_ws_msg_init(xrsv_ws_obj_t *obj, uint8_t **buffer, uint32_t *length) {
   json_t *obj_init = obj->obj_init;
   int rc;
   
   // Update the dynamic values
   rc  = json_object_set_new_nocheck(obj_init, "updated", json_integer(xrsv_ws_time_get()));
   
   if(rc != 0) {
      XLOGD_ERROR("object set failed");
   }

   *buffer = (uint8_t *)json_dumps(obj_init, JSON_COMPACT);
   if(*buffer == NULL) {
      *length = 0;
      XLOGD_ERROR("unable to dump json");
      return;
   }
   *length = strlen((const char *)(*buffer));
}

void xrsv_ws_msg_stream_end(xrsv_ws_obj_t *obj, int32_t reason, uint8_t **buffer, uint32_t *length) {
   json_t *obj_stream_end = obj->obj_stream_end;
   int rc;

   // Update the dynamic values
   rc = json_object_set_new_nocheck(obj_stream_end, XRSV_WS_JSON_KEY_REASON, json_integer(reason));

   if(rc != 0) {
      XLOGD_ERROR("object set failed");
   }

   *buffer = (uint8_t *)json_dumps(obj_stream_end, JSON_COMPACT);
   if(*buffer == NULL) {
      *length = 0;
      XLOGD_ERROR("unable to dump json");
      return;
   }
   *length = strlen((const char *)(*buffer));
}

uint64_t xrsv_ws_time_get(void) {
    struct timespec ts;
    errno = 0;
    if(clock_gettime(CLOCK_REALTIME, &ts)) {
       int errsv = errno;
       XLOGD_ERROR("unable to get clock <%s>", strerror(errsv));
       return(0);
    }
    // Return the time in milliseconds since epoch
    return(((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}

bool xrsv_ws_msg_decode(xrsv_ws_obj_t *obj, json_t *obj_json) {
   if(xlog_level_active(XLOG_MODULE_ID, XLOG_LEVEL_INFO)) {
      char *str = json_dumps(obj_json, JSON_SORT_KEYS | JSON_INDENT(3));
      XLOGD_INFO("obj \n<%s>", str ? str : "NULL");
      if(str != NULL) {
         free(str);
      }
   }

   json_t *obj_req_type = json_object_get(obj_json, "requestType");

   if(obj_req_type == NULL || !json_is_string(obj_req_type)) {
      XLOGD_ERROR("request type invalid");
      return(false);
   }
   const char *str_req_type = json_string_value(obj_req_type);

   // Call handler based on request type
   xrsv_ws_request_handler_t *handler = (str_req_type == NULL) ? NULL : xrsv_ws_request_handler_get(str_req_type, strlen(str_req_type));

   if(handler == NULL) {
      XLOGD_ERROR("no handler for request <%s>", (str_req_type == NULL) ? "NULL" : str_req_type);
      return(false);
   }

   return((*handler->func)(obj, obj_json));
}

bool xrsv_ws_req_mic_status(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   return(false);
}

bool xrsv_ws_req_mic_close(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_reason = json_object_get(obj_json, "reason");

   const char *str_reason = NULL;
   if(obj_reason != NULL && json_is_string(obj_reason)) {
      str_reason = json_string_value(obj_reason);
   }

   XLOGD_INFO("reason <%s>", str_reason ? str_reason : "NULL");
   if(obj->handlers.mic_close != NULL) {
      (*obj->handlers.mic_close)(obj_reason, obj->user_data);
   }
   return(true);
}

bool xrsv_ws_req_processing(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_tran = json_object_get(obj_json, "transcription");

   const char *str_tran = NULL;
   if(obj_tran != NULL && json_is_string(obj_tran)) {
      str_tran = json_string_value(obj_tran);
   }

   XLOGD_INFO("transcription <%s>", str_tran ? str_tran : "NULL");
   if(obj->handlers.processing != NULL) {
      (*obj->handlers.processing)(str_tran, obj->user_data);
   }

   json_t *obj_init = obj->obj_init;
   json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_LAST_CMD_TRANSCRIPTION, json_string(str_tran));
   return(false);
}

bool xrsv_ws_req_listening(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.listening != NULL) {
      (*obj->handlers.listening)(obj->user_data);
   }
   return(false);
}

bool xrsv_ws_req_countdown(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.countdown != NULL) {
      (*obj->handlers.countdown)(obj->user_data);
   }
   return(false);
}

bool xrsv_ws_req_action(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_action = json_object_get(obj_json, "action");

   const char *str_action = NULL;
   if(obj_action != NULL && json_is_string(obj_action)) {
      str_action = json_string_value(obj_action);
   }

   XLOGD_INFO("action <%s>", str_action ? str_action : "NULL");

   // Call handler based on action
   xrsv_ws_action_handler_t *handler = (str_action == NULL) ? NULL : xrsv_ws_action_handler_get(str_action, strlen(str_action));

   if(handler == NULL) {
      XLOGD_ERROR("no handler for action <%s>", str_action ? str_action : "NULL");
      return(false);
   }

   (*handler->func)(obj, obj_json);
   return(false);
}

bool xrsv_ws_req_sip_call(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_call = xrsv_ws_phone_call_obj_get(obj_json);

   if(obj_call == NULL) {
      XLOGD_ERROR("call object not found");
      return(false);
   }

   json_t *obj_req_type = json_object_get(obj_json, "requestType");

   if(obj_req_type == NULL || !json_is_string(obj_req_type)) {
      XLOGD_ERROR("request type invalid");
      return(false);
   }
   const char *str_req_type = json_string_value(obj_req_type);

   // if there are more request types in the future, convert to hash lookup
   if(0 != strcmp(str_req_type, "action")) {
      XLOGD_ERROR("unsupported request type <%s>", str_req_type);
      return(false);
   }

   json_t *obj_action = json_object_get(obj_json, "action");

   if(obj_action == NULL || !json_is_string(obj_action)) {
      XLOGD_ERROR("action invalid");
      return(false);
   }
   const char *str_action = json_string_value(obj_action);

   // Call handler based on action type
   xrsv_ws_action_handler_t *handler = (str_action == NULL) ? NULL : xrsv_ws_action_phone_handler_get(str_action, strlen(str_action));

   if(handler == NULL) {
      XLOGD_ERROR("no handler for request <%s>", str_req_type);
      return(false);
   }

   (*handler->func)(obj, obj_call);
   return(false);
}

json_t *xrsv_ws_phone_call_obj_get(json_t *obj_json) {
   json_t *obj_call = json_object_get(obj_json, "notify");
   if(obj_call != NULL && json_is_object(obj_call)) {
      return(obj_call);
   }
   obj_call = json_object_get(obj_json, "sip_call");
   if(obj_call != NULL && json_is_object(obj_call)) {
      return(obj_call);
   }
   return(NULL);
}

void xrsv_ws_act_phone_call_start(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_number = json_object_get(obj_json, "phoneNumber");
   json_t *obj_trans  = json_object_get(obj_json, "transcription");
   json_t *obj_url    = json_object_get(obj_json, "ttsAudioUrl");

   const char *str_number = NULL;
   if(obj_number != NULL && json_is_string(obj_number)) {
      str_number = json_string_value(obj_number);
   }
   const char *str_trans = NULL;
   if(obj_trans != NULL && json_is_string(obj_trans)) {
      str_trans = json_string_value(obj_trans);
   }
   const char *str_url = NULL;
   if(obj_url != NULL && json_is_string(obj_url)) {
      str_url = json_string_value(obj_url);
   }

   XLOGD_INFO("phone number <%s> transcription <%s> tts audio url <%s>", str_number ? str_number : "NULL", str_trans ? str_trans : "NULL", str_url ? str_url : "NULL");
   if(obj->handlers.phone_call_start != NULL) {
      (*obj->handlers.phone_call_start)(obj_number, obj_trans, obj_url, obj->user_data);
   }
}

void xrsv_ws_act_phone_call_answer(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.phone_call_answer != NULL) {
      (*obj->handlers.phone_call_answer)(obj->user_data);
   }
}

void xrsv_ws_act_phone_call_ignore(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.phone_call_ignore != NULL) {
      (*obj->handlers.phone_call_ignore)(obj->user_data);
   }
}

void xrsv_ws_act_notification_cancel(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.notify_cancel != NULL) {
      (*obj->handlers.notify_cancel)(obj->user_data);
   }
}

void xrsv_ws_act_notify(xrsv_ws_obj_t *obj, json_t *obj_json) {
   json_t *obj_url = json_object_get(obj_json, "ttsAudioUrl");

   const char *str_url = NULL;
   if(obj_url != NULL && json_is_string(obj_url)) {
      str_url = json_string_value(obj_url);
   }

   XLOGD_INFO("tts audio url <%s>", str_url ? str_url : "NULL");
   if(str_url != NULL && obj->handlers.notify != NULL) {
      (*obj->handlers.notify)(obj_url, obj->user_data);
   }
}

void xrsv_ws_act_tv_volume_up(xrsv_ws_obj_t *obj, json_t *obj_json) {
   uint32_t ir_repeat_count = 1;
   json_t *obj_irrc = json_object_get(obj_json, "irRepeatCount");

   if(obj_irrc != NULL && json_is_integer(obj_irrc)) {
      json_int_t value = json_integer_value(obj_irrc);
      if(value <= 0 || value > 100) {
         XLOGD_ERROR("ir repeat count out of range <%" JSON_INTEGER_FORMAT ">", value);
      } else {
         ir_repeat_count = (uint32_t)value;
      }
   }

   XLOGD_INFO("ir repeat count <%u>", ir_repeat_count);
   if(obj->handlers.tv_volume != NULL) {
      (*obj->handlers.tv_volume)(true, ir_repeat_count, obj->user_data);
   }
}

void xrsv_ws_act_tv_volume_down(xrsv_ws_obj_t *obj, json_t *obj_json) {
   uint32_t ir_repeat_count = 1;
   json_t *obj_irrc = json_object_get(obj_json, "irRepeatCount");

   if(obj_irrc != NULL && json_is_integer(obj_irrc)) {
      json_int_t value = json_integer_value(obj_irrc);
      if(value <= 0 || value > 100) {
         XLOGD_ERROR("ir repeat count out of range <%" JSON_INTEGER_FORMAT ">", value);
      } else {
         ir_repeat_count = (uint32_t)value;
      }
   }

   XLOGD_INFO("ir repeat count <%u>", ir_repeat_count);
   if(obj->handlers.tv_volume != NULL) {
      (*obj->handlers.tv_volume)(false, ir_repeat_count, obj->user_data);
   }
}

void xrsv_ws_act_tv_mute_on(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.tv_mute != NULL) {
      (*obj->handlers.tv_mute)(true, obj->user_data);
   }
}

void xrsv_ws_act_tv_mute_off(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.tv_mute != NULL) {
      (*obj->handlers.tv_mute)(false, obj->user_data);
   }
}

void xrsv_ws_act_tv_power_on(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.tv_power != NULL) {
      bool toggle = false;
      json_t *obj_toggle = json_object_get(obj_json, "toggleFallback");
      if(obj_toggle) {
         if(!json_is_boolean(obj_toggle)) {
            XLOGD_ERROR("toggleFallback of wrong type <%d>", json_typeof(obj_toggle));
         } else {
            toggle = json_is_true(obj_toggle);
         }
      }
      (*obj->handlers.tv_power)(true, toggle, obj->user_data);
   }
}

void xrsv_ws_act_tv_power_off(xrsv_ws_obj_t *obj, json_t *obj_json) {
   XLOGD_INFO("");
   if(obj->handlers.tv_power != NULL) {
      bool toggle = false;
      json_t *obj_toggle = json_object_get(obj_json, "toggleFallback");
      if(obj_toggle) {
         if(!json_is_boolean(obj_toggle)) {
            XLOGD_ERROR("toggleFallback of wrong type <%d>", json_typeof(obj_toggle));
         } else {
            toggle = json_is_true(obj_toggle);
         }
      }
      (*obj->handlers.tv_power)(false, toggle, obj->user_data);
   }
}

void xrsv_ws_update_init_last_command_times(xrsv_ws_obj_t *obj) {
   json_t *obj_init = obj->obj_init;
   json_t *obj_last_cmd = json_array();
   // Shift last command times once
   int i;
   for(i = XRSV_WS_LAST_TIMESTAMPS_SIZE - 2; i >= 0; i--) {
      obj->last_command_times[i + 1] = obj->last_command_times[i];
   }
   // Add current sessions time
   obj->last_command_times[0] = obj->current_command_time;
   
   if(obj_last_cmd) {
      // Make new JSON array
      for(i = 0; i < XRSV_WS_LAST_TIMESTAMPS_SIZE; i++) {
         if(obj->last_command_times[i] != 0) {
            json_array_append(obj_last_cmd, json_integer(obj->last_command_times[i]));
         }
      }
      json_object_set_new_nocheck(obj_init, XRSV_WS_JSON_KEY_LAST_CMD_TIMES, obj_last_cmd);
   } else {
      XLOGD_ERROR("failed to create json array");
      json_object_del(obj_init, XRSV_WS_JSON_KEY_LAST_CMD_TIMES);
   }
}
