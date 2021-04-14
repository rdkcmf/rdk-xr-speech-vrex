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
#ifndef __XRSV_WS_PRIVATE__
#define __XRSV_WS_PRIVATE__

#include <bsd/string.h>
#include <errno.h>
#include <rdkx_logger.h>
#include <jansson.h>
#include <xrsv_ws.h>

#define XRSV_WS_LAST_TIMESTAMPS_SIZE (5)

#define XRSV_WS_QUERY_STRING_QTY_MAX (24)

typedef struct {
   uint32_t            identifier;
   xrsv_ws_handlers_t  handlers;
   xrsr_handler_send_t send;
   void *              param;
   json_t *            obj_init;
   json_t *            obj_stream_end;
   time_t              last_command_times[XRSV_WS_LAST_TIMESTAMPS_SIZE];
   time_t              current_command_time;
   const char *        query_strs[XRSV_WS_QUERY_STRING_QTY_MAX];
   char                query_element_trx[41];
   char                query_element_device_id[64];
   char                query_element_device_id_xbo[64];
   char                query_element_receiver_id[64];
   char                query_element_codec[17];
   char                query_element_app_id[64];
   char                query_element_partner_id[32];
   char                query_element_experience[32];
   char                query_element_language[32];
   char                query_element_aspect_ratio[16];
   void *              user_data;
} xrsv_ws_obj_t;

bool xrsv_ws_req_mic_status(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_mic_close(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_processing(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_listening(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_countdown(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_action(xrsv_ws_obj_t *obj, json_t *obj_json);
bool xrsv_ws_req_sip_call(xrsv_ws_obj_t *obj, json_t *obj_json);

void xrsv_ws_act_phone_call_start(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_phone_call_answer(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_phone_call_ignore(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_notification_cancel(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_notify(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_volume_up(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_volume_down(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_mute_on(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_mute_off(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_power_on(xrsv_ws_obj_t *obj, json_t *obj_json);
void xrsv_ws_act_tv_power_off(xrsv_ws_obj_t *obj, json_t *obj_json);

typedef void (*xrsv_ws_handler_void_t)(xrsv_ws_obj_t *obj, json_t *obj_json);
typedef bool (*xrsv_ws_handler_bool_t)(xrsv_ws_obj_t *obj, json_t *obj_json);

typedef struct xrsv_ws_request_handler_s { char *name; xrsv_ws_handler_bool_t func; } xrsv_ws_request_handler_t;
typedef struct xrsv_ws_action_handler_s  { char *name; xrsv_ws_handler_void_t func; } xrsv_ws_action_handler_t;

struct xrsv_ws_request_handler_s * xrsv_ws_request_handler_get(const char *str, unsigned int len);
struct xrsv_ws_action_handler_s *  xrsv_ws_action_handler_get(const char *str, unsigned int len);
struct xrsv_ws_action_handler_s *  xrsv_ws_action_phone_handler_get(const char *str, unsigned int len);

#endif
