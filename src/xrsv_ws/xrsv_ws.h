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
#ifndef __XR_SPEECH_VREX_WS__
#define __XR_SPEECH_VREX_WS__

#include <stdint.h>
#include <stdbool.h>
#include <xrsr.h>
#include <xr_timestamp.h>
#include <jansson.h>
#include <xrsv.h>

/// @file xrsv_ws.h
///
/// @defgroup XRSV_WS VREX SPEECH REQUEST HANDLER
/// @{
///
/// @defgroup XRSV_WS_DEFINITIONS Definitions
/// @defgroup XRSV_WS_ENUMS       Enumerations
/// @defgroup XRSV_WS_STRUCTS     Structures
/// @defgroup XRSV_WS_TYPEDEFS    Type Definitions
/// @defgroup XRSV_WS_FUNCTIONS   Functions
///

/// @addtogroup XRSV_WS_DEFINITIONS
/// @{
/// @brief Macros for constant values
/// @details The VREX speech request handler provides macros for some parameters which may change in the future.  User applications should use
/// these names to allow the application code to function correctly if the values change.

#define XRSV_WS_SESSION_ID_LEN_MAX      (64)  ///< Session identifier maximum length including NULL termination
#define XRSV_WS_SESSION_STR_LEN_MAX     (512) ///< Session strings maximum length including NULL termination

/// @}

/// @addtogroup XRSV_WS_STRUCTS
/// @{
/// @brief Structures
/// @details The VREX speech request handler provides structures for grouping of values.

/// @brief XRSV_WS param structure
/// @details The param data structure is used to provide input parameters to the xrsv_ws_open() function.  All string parameters must be NULL-terminated.  If a string parameter is not present, NULL must be set for it.
typedef struct {
   const char *device_id;        ///< The client device's unique identifier
   const char *receiver_id;      ///< The client device's receiver identifier
   const char *device_mac;       ///< The client device's MAC address
   const char *account_id;       ///< The user's account identifier
   const char *partner_id;       ///< The network's partner identifier
   const char *experience;       ///< User experience identifier
   const char *stb_device_id;    ///< The identifier of the STB which is being controlled
   const char *device_name;      ///< A descriptive name for the device
   const char *device_type;      ///< The type of device ("xr18")
   const char *app_id;           ///< The application identifier for the client device
   const char *software_version; ///< The software version of the client device
   const char *ip_addr_v4;       ///< IPV4 Address
   const char *ip_addr_v6;       ///< IPV6 Address
   const char *ssid;             ///< Wifi SSID to which the device is connected
   const char *pstn;             ///< The device's phone number
   const char *language;         ///< The device's language
   bool        test_flag;        ///< True if the device is used for testing only, otherwise false
   void       *user_data;        ///< User data that is passed in to all of the callbacks
} xrsv_ws_params_t;

/// @brief XRSV_WS received message structure
/// @details The received message data structure is returned in the receive message callback function.
typedef struct {
   long    ret_code;                                ///< Return code for the operation (if applicable)
   char    message[XRSV_WS_SESSION_STR_LEN_MAX];       ///< The message payload
   char    transcription[XRSV_WS_SESSION_STR_LEN_MAX]; ///< The transcription of the speech (if applicable)
   char    session_id[XRSV_WS_SESSION_ID_LEN_MAX];     ///< The session identifier (UUID or TRX)
} xrsv_ws_recv_msg_t;

/// @brief XRSV_WS stream parameter structure
/// @details The stream parameter data structure is returned in the session begin callback function.
typedef struct {
   uint32_t keyword_sample_begin;               ///< The offset in samples from the beginning of the buffered audio to the beginning of the keyword
   uint32_t keyword_sample_end;                 ///< The offset in samples from the beginning of the buffered audio to the end of the keyword
   uint16_t keyword_doa;                        ///< The direction of arrival in degrees (0-359)
   uint16_t keyword_sensitivity;                ///<
   bool     keyword_sensitivity_triggered;      ///<
   bool     keyword_sensitivity_high_support;   ///<
   bool     keyword_sensitivity_high_triggered; ///<
   uint16_t keyword_sensitivity_high;           ///<
   double   dynamic_gain;                       ///<
   double   signal_noise_ratio;                 ///<
   double   linear_confidence;                  ///<
   int32_t  nonlinear_confidence;               ///<
   bool     push_to_talk;                       ///< True if the session was started by the user pressing a button
} xrsv_ws_stream_params_t;

/// @}

/// @addtogroup XRSV_WS_TYPEDEFS
/// @{
/// @brief Type Definitions
/// @details The VREX speech request handler provides type definitions for renaming types.

/// @brief xrsv ws object type
/// @details The xrsv ws object type is returned by the xrsv_ws_open api.  It is used in all subsequent calls to xrsv_ws api's.
typedef void * xrsv_ws_object_t;

/// @brief XRSV_WS session begin handler
/// @details Function type to handle session begin events from the speech router.
/// @param[in] uuid          the session's unique identifier
/// @param[in] src           the input source for the audio stream
/// @param[in] configuration session configuration from the speech router
/// @param[in] stream_params parameters which describe the stream
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_session_begin_t)(const uuid_t uuid, xrsr_src_t src, uint32_t dst_index, xrsr_session_configuration_t *configuration, xrsv_ws_stream_params_t *stream_params, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS session end handler
/// @details Function type to handle session end events from the speech router.
/// @param[in] uuid  the session's unique identifier
/// @param[in] stats sesssion statistics from the speech router
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_session_end_t)(const uuid_t uuid, xrsr_session_stats_t *stats, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream begin handler
/// @details Function type to handle stream begin events from the speech router.
/// @param[in] uuid the session's unique identifier
/// @param[in] src  the input source for the error
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_stream_begin_t)(const uuid_t uuid, xrsr_src_t src, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream kwd handler
/// @details Function type to handle stream kwd events from the speech router.
/// @param[in] uuid the session's unique identifier
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_stream_kwd_t)(const uuid_t uuid, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream end handler
/// @details Function type to handle stream end events from the speech router.
/// @param[in] uuid  the session's unique identifier
/// @param[in] stats stream statistics from the speech router
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_stream_end_t)(const uuid_t uuid, xrsr_stream_stats_t *stats, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream connect handler
/// @details Function type to handle stream connect events from the speech router.
/// @param[in] uuid  the session's unique identifier
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_connected_t)(const uuid_t uuid, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream disconnect handler
/// @details Function type to handle stream disconnect events from the speech router.
/// @param[in] uuid  the session's unique identifier
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_disconnected_t)(const uuid_t uuid, bool retry, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS stream sent init handler
/// @details Function type to handle initilization message sent events from the speech router.
/// @param[in] uuid  the session's unique identifier
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_sent_init_t)(const uuid_t uuid, rdkx_timestamp_t *timestamp, void *user_data);

/// @brief XRSV_WS listening handler
/// @details Function type to handle listening events during the session.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_listening_t)(void *user_data);

/// @brief XRSV_WS processing handler
/// @details Function type to handle processing events during the session.
/// @param[in] obj_transcription JSON object associated with the processing event
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_processing_t)(const char *str_transcription, void *user_data);

/// @brief XRSV_WS countdown handler
/// @details Function type to handle countdown events during the session.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_countdown_t)(void *user_data);

/// @brief XRSV_WS mic close handler
/// @details Function type to handle microphone close events during the session.
/// @param[in] obj_reason JSON object associated with the microphone close event
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_mic_close_t)(json_t *obj_reason, void *user_data);

/// @brief XRSV_WS receive message handler
/// @details Function type to handle received messages during the session.
/// @param[in] msg Pointer to the received message.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_recv_msg_t)(xrsv_ws_recv_msg_t *msg, void *user_data);

/// @brief XRSV_WS source error handler
/// @details Function type to handle source error events during the session.
/// @param[in] src Source input which generated the error
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_source_error_t)(xrsr_src_t src, void *user_data);

/// @brief XRSV_WS data send handler
/// @details Function type to handle TV mute events during the session.
/// @param[in] mute True to mute the TV, False to unmute
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_tv_mute_t)(bool mute, void *user_data);

/// @brief XRSV_WS TV power handler
/// @details Function type to handle TV power events during the session.
/// @param[in] power True to turn TV power ON, False to turn TV power OFF
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_tv_power_t)(bool power, bool toggle, void *user_data);

/// @brief XRSV_WS TV volume handler
/// @details Function type to handle TV volume events during the session.
/// @param[in] up           True to increase volume, False to decrease volume
/// @param[in] repeat_count Quantity of increment to the volume
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_tv_volume_t)(bool up, uint32_t repeat_count, void *user_data);

/// @brief XRSV_WS notify handler
/// @details Function type to handle notification events during the session.
/// @param[in] obj_url JSON object associated with the notification event
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_notify_t)(json_t *obj_url, void *user_data);

/// @brief XRSV_WS notify cancel handler
/// @details Function type to handle notification cancel events during the session.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_notify_cancel_t)(void *user_data);

/// @brief XRSV_WS phone call answer handler
/// @details Function type to handle phone call answering events during the session.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_phone_call_answer_t)(void *user_data);

/// @brief XRSV_WS phone call ignore handler
/// @details Function type to handle phone call ignore events during the session.
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_phone_call_ignore_t)(void *user_data);

/// @brief XRSV_WS phone call start handler
/// @details Function type to handle phone call start events during the session.
/// @param[in] obj_number JSON object containing phone number details
/// @param[in] obj_trans  JSON object containing transcription details
/// @param[in] obj_url    JSON object containing URL associated with the call
/// @param[in] user_data     the data set by the user
/// @return The function has no return value.
typedef void (*xrsv_ws_handler_phone_call_start_t)(json_t *obj_number, json_t *obj_trans, json_t *obj_url, void *user_data);

/// @}

/// @addtogroup XRSV_WS_STRUCTS
/// @{

/// @brief XRSV_WS handlers structure
/// @details The handlers data structure is used to set all callback handlers for the vrex speech handler.  If a handler is not needed, it must be set to NULL.
typedef struct {
   xrsv_ws_handler_session_begin_t     session_begin;     ///< Indicates that a voice session has started
   xrsv_ws_handler_session_end_t       session_end;       ///< Indicates that a voice session has ended
   xrsv_ws_handler_stream_begin_t      stream_begin;      ///< An audio stream has started
   xrsv_ws_handler_stream_kwd_t        stream_kwd;        ///< The keyword has been passed in the stream
   xrsv_ws_handler_stream_end_t        stream_end;        ///< An audio stream has ended
   xrsv_ws_handler_connected_t         connected;         ///< The session has connected
   xrsv_ws_handler_disconnected_t      disconnected;      ///< The session has disconnected
   xrsv_ws_handler_sent_init_t         sent_init;         ///< The init message was sent to the server
   xrsv_ws_handler_recv_msg_t          recv_msg;          ///< A message was received from the server
   xrsv_ws_handler_listening_t         listening;         ///< The server is listening to the audio
   xrsv_ws_handler_processing_t        processing;        ///< The server is processing the audio
   xrsv_ws_handler_countdown_t         countdown;         ///< The server has started the countdown timer
   xrsv_ws_handler_mic_close_t         mic_close;         ///< The server wants to close the microphone
   xrsv_ws_handler_source_error_t      source_error;      ///< The source has encountered an error
   xrsv_ws_handler_tv_mute_t           tv_mute;           ///< The server wants to mute/unmute the TV
   xrsv_ws_handler_tv_power_t          tv_power;          ///< The server wants to control the TV power
   xrsv_ws_handler_tv_volume_t         tv_volume;         ///< The server wants to control the TV's volume
   xrsv_ws_handler_notify_t            notify;            ///< The server has issued a notification
   xrsv_ws_handler_notify_cancel_t     notify_cancel;     ///< The server has cancelled a notification
   xrsv_ws_handler_phone_call_answer_t phone_call_answer; ///< The server has indicated to answer the phone call
   xrsv_ws_handler_phone_call_ignore_t phone_call_ignore; ///< The server has indicated to ignore the phone call
   xrsv_ws_handler_phone_call_start_t  phone_call_start;  ///< The server has indicated to place a phone call
} xrsv_ws_handlers_t;

/// @}

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup XRSV_WS_FUNCTIONS
/// @{
/// @brief Function definitions
/// @details The VREX speech request handler provides functions to be called directly by the user application.

/// @brief Open the vrex speech request handler
/// @details Function which opens the vrex speech request handler interface.
/// @param[in] params Pointer to a structure of params which will be used while opening the interace.  Many of these parameters can be updated while the interface is open.
/// @return The function returns true for success, otherwise false.
xrsv_ws_object_t xrsv_ws_create(const xrsv_ws_params_t *params);

/// @brief Set the vrex speech request handlers
/// @details Function type to set the handlers for a given protocol.
/// @param[in] handlers_in  Set of handlers that are set by the application.
/// @param[in] handlers_out Set of handlers provided by this component for use by the application.
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_handlers(xrsv_ws_object_t object, const xrsv_ws_handlers_t *handlers_in, xrsr_handlers_t *handlers_out);

/// @brief Update the user data for vrex speech handlers
/// @details Function to update the user data for all of the handlers.
/// @param[in] user_data the data the user wants to be passed in every handler.
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_user_data(xrsv_ws_object_t object, void *user_data);

/// @brief Update the vrex speech request handler's device id
/// @details Function used to update the device id.
/// @param[in] device_id Unique identifier for the device stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_device_id(xrsv_ws_object_t object, const char *device_id);

/// @brief Update the vrex speech request handler's receiver id
/// @details Function used to update the receiver id.
/// @param[in] receiver_id Unique identifier for the receiver stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_receiver_id(xrsv_ws_object_t object, const char *receiver_id);

/// @brief Update the vrex speech request handler's mac address
/// @details Function used to update the device's mac address used by the vrex speech request handler.
/// @param[in] device_mac MAC address as a NULL-terminated string (ie. "11:22:33:44:55:66")
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_device_mac(xrsv_ws_object_t object, const char *device_mac);

/// @brief Update the vrex speech request handler's account id
/// @details Function used to update the user's account identifier.
/// @param[in] account_id User's account identifier stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_account_id(xrsv_ws_object_t object, const char *account_id);

/// @brief Update the vrex speech request handler's partner id
/// @details Function used to update the partner identifier.
/// @param[in] partner_id Network's partner identifier stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_partner_id(xrsv_ws_object_t object, const char *partner_id);

/// @brief Update the vrex speech request handler's user experience id
/// @details Function used to update the user experience identifier.
/// @param[in] experience User experience identifier stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_experience(xrsv_ws_object_t object, const char *experience);

/// @brief Update the vrex speech request handler's STB device id
/// @details Function used to update the device identifier for an associated STB.
/// @param[in] device_id Device identifier for an associated STB stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_stb_device_id(xrsv_ws_object_t object, const char *device_id);

/// @brief Update the vrex speech request handler's device name
/// @details Function used to update the device's name.
/// @param[in] device_name Device's name stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_device_name(xrsv_ws_object_t object, const char *device_name);

/// @brief Update the vrex speech request handler's device type
/// @details Function used to update the device type.
/// @param[in] device_type Device's type stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_device_type(xrsv_ws_object_t object, const char *device_type);

/// @brief Update the vrex speech request handler's device software version
/// @details Function used to update the device's software version.
/// @param[in] device_firmware Device's software version stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_device_software(xrsv_ws_object_t object, const char *device_firmware);

/// @brief Update the vrex speech request handler's application id
/// @details Function used to update the application identifer.
/// @param[in] app_id Application identifier stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_app_id(xrsv_ws_object_t object, const char *app_id);

/// @brief Update the vrex speech request handler's software version
/// @details Function used to update the device's software version.
/// @param[in] software_version Device's software version stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_software_version(xrsv_ws_object_t object, const char *software_version);

/// @brief Update the vrex speech request handler's IP address
/// @details Function used to update the device's IP addresses.
/// @param[in] ip_addr_v4 IPV4 address stored as a NULL-terminated string
/// @param[in] ip_addr_v6 IPV6 address stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_ip_addr(xrsv_ws_object_t object, const char *ip_addr_v4, const char *ip_addr_v6);

/// @brief Update the vrex speech request handler's SSID
/// @details Function used to update the device's SSID (if applicable).
/// @param[in] ssid The access point to which the device is associated stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_ssid(xrsv_ws_object_t object, const char *ssid);

/// @brief Update the vrex speech request handler's PSTN
/// @details Function used to update the device's public switched telephone number.
/// @param[in] pstn device's phone number stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_pstn(xrsv_ws_object_t object, const char *pstn);

/// @brief Update the vrex speech request handler's language
/// @details Function used to update the device's language.
/// @param[in] language Current language setting stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_language(xrsv_ws_object_t object, const char *language);

/// @brief Update the vrex speech request handler's session parameters
/// @details Function used to update session's state.
/// @param[in] call_state Indicates the state of the phone stored as a NULL-terminated string
/// @return The function returns true for success, otherwise false.
bool xrsv_ws_update_session_params(xrsv_ws_object_t object, const char *call_state);

/// @brief Close the vrex speech request handler
/// @details Function used to close the vrex speech request interface.
/// @return The function has no return value.
void xrsv_ws_destroy(xrsv_ws_object_t object);

/// @brief vrex speech request handler message string
/// @details Function used to convert an enum to a string.
/// @param[in] type Receive message type to convert to a string
/// @return The function returns the result of the operation.
const char *xrsr_recv_msg_str(xrsr_recv_msg_t type);

/// @}

#ifdef __cplusplus
}
#endif

/// @}

#endif
