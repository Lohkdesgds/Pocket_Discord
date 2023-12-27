#include "discord/gateway.hpp"

#include <unordered_map>
#include <memory>
#include "esp_log.h"
#include <time.h>

#include "jsoninterface.h"
#include "defaults.h"
#include "filehandler.h"
#include "exception.h"

namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "GATEWAY";
        
        void __c_gateway_redirect_event(void*, esp_event_base_t, int32_t, void*);
        
        gateway_opcodes str2gateway_opcode(const char* s)
        {
            // size: 80 bytes + dynamic alloc
            static std::unordered_map<const char*, gateway_opcodes> const map = {
                { "DISPATCH"                , gateway_opcodes::DISPATCH },
                { "HEARTBEAT"               , gateway_opcodes::HEARTBEAT },
                { "IDENTIFY"                , gateway_opcodes::IDENTIFY },
                { "PRESENCE_UPDATE_SEND"    , gateway_opcodes::PRESENCE_UPDATE_SEND },
                { "VOICE_STATE_UPDATE"      , gateway_opcodes::VOICE_STATE_UPDATE },
                { "RESUME"                  , gateway_opcodes::RESUME },
                { "RECONNECT"               , gateway_opcodes::RECONNECT },
                { "REQUEST_GUILD_MEMBERS"   , gateway_opcodes::REQUEST_GUILD_MEMBERS },
                { "INVALID_SESSION"         , gateway_opcodes::INVALID_SESSION },
                { "HELLO"                   , gateway_opcodes::HELLO },
                { "HEARTBEAT_ACK"           , gateway_opcodes::HEARTBEAT_ACK }
            };

            if (const auto& it = map.find(s); it != map.end()) return it->second;
            return gateway_opcodes::UNKNOWN;
        }

        gateway_close_event_codes str2gateway_close_event_codes(const char* s)
        {
            // size: 80 bytes + dynamic alloc
            static std::unordered_map<const char*, gateway_close_event_codes> const map = {
                { "UNKNOWN_ERROR"             , gateway_close_event_codes::UNKNOWN_ERROR },
                { "UNKNOWN_OPCODE"            , gateway_close_event_codes::UNKNOWN_OPCODE },
                { "DECODE_ERROR"              , gateway_close_event_codes::DECODE_ERROR },
                { "NOT_AUTHENTICATED"         , gateway_close_event_codes::NOT_AUTHENTICATED },
                { "AUTHENTICATION_FAILED"     , gateway_close_event_codes::AUTHENTICATION_FAILED },
                { "ALREADY_AUTHENTICATED"     , gateway_close_event_codes::ALREADY_AUTHENTICATED },
                { "INVALID_SEQ"               , gateway_close_event_codes::INVALID_SEQ },
                { "RATE_LIMITED"              , gateway_close_event_codes::RATE_LIMITED },
                { "SESSION_TIMED_OUT"         , gateway_close_event_codes::SESSION_TIMED_OUT },
                { "INVALID_SHARD"             , gateway_close_event_codes::INVALID_SHARD },
                { "SHARDING_REQUIRED"         , gateway_close_event_codes::SHARDING_REQUIRED },
                { "INVALID_API_VERSION"       , gateway_close_event_codes::INVALID_API_VERSION },
                { "INVALID_INTENTS"           , gateway_close_event_codes::INVALID_INTENTS },
                { "DISALLOWED_INTENTS"        , gateway_close_event_codes::DISALLOWED_INTENTS }
            };

            if (const auto& it = map.find(s); it != map.end()) return it->second;
            return gateway_close_event_codes::UNKNOWN_ERROR;
        }


        gateway_send_events str2gateway_send_events(const char* s)
        {
            // size: 80 bytes + dynamic alloc
            static std::unordered_map<const char*, gateway_send_events> const map = {
                { "UNKNOWN"                    , gateway_send_events::UNKNOWN },
                { "IDENTIFY"                   , gateway_send_events::IDENTIFY },
                { "RESUME"                     , gateway_send_events::RESUME },
                { "HEARTBEAT"                  , gateway_send_events::HEARTBEAT },
                { "REQUEST_GUILD_MEMBERS"      , gateway_send_events::REQUEST_GUILD_MEMBERS },
                { "UPDATE_VOICE_STATE"         , gateway_send_events::UPDATE_VOICE_STATE },
                { "UPDATE_PRESENCE"            , gateway_send_events::UPDATE_PRESENCE }
            };

            if (const auto& it = map.find(s); it != map.end()) return it->second;
            return gateway_send_events::UNKNOWN;
        }
        
        gateway_events str2gateway_events(const char* s)
        {
            // size: 80 bytes + dynamic alloc
            static std::unordered_map<const char*, gateway_events> const map = {
                { "UNKNOWN"                                 , gateway_events::UNKNOWN},
                { "HELLO"                                   , gateway_events::HELLO},
                { "READY"                                   , gateway_events::READY},
                { "RESUMED"                                 , gateway_events::RESUMED},
                { "RECONNECT"                               , gateway_events::RECONNECT},
                { "INVALID_SESSION"                         , gateway_events::INVALID_SESSION},
                { "APPLICATION_COMMAND_PERMISSION_UPDATE"   , gateway_events::APPLICATION_COMMAND_PERMISSION_UPDATE},
                { "AUTO_MODERATION_RULE_CREATE"             , gateway_events::AUTO_MODERATION_RULE_CREATE},
                { "AUTO_MODERATION_RULE_UPDATE"             , gateway_events::AUTO_MODERATION_RULE_UPDATE},
                { "AUTO_MODERATION_RULE_DELETE"             , gateway_events::AUTO_MODERATION_RULE_DELETE},
                { "AUTO_MODERATION_ACTION_EXECUTION"        , gateway_events::AUTO_MODERATION_ACTION_EXECUTION},
                { "CHANNEL_CREATE"                          , gateway_events::CHANNEL_CREATE},
                { "CHANNEL_UPDATE"                          , gateway_events::CHANNEL_UPDATE},
                { "CHANNEL_DELETE"                          , gateway_events::CHANNEL_DELETE},
                { "CHANNEL_PINS_UPDATE"                     , gateway_events::CHANNEL_PINS_UPDATE},
                { "THREAD_CREATE"                           , gateway_events::THREAD_CREATE},
                { "THREAD_UPDATE"                           , gateway_events::THREAD_UPDATE},
                { "THREAD_DELETE"                           , gateway_events::THREAD_DELETE},
                { "THREAD_LIST_SYNC"                        , gateway_events::THREAD_LIST_SYNC},
                { "THREAD_MEMBER_UPDATE"                    , gateway_events::THREAD_MEMBER_UPDATE},
                { "THREAD_MEMBERS_UPDATE"                   , gateway_events::THREAD_MEMBERS_UPDATE},
                { "ENTITLEMENT_CREATE"                      , gateway_events::ENTITLEMENT_CREATE},
                { "ENTITLEMENT_UPDATE"                      , gateway_events::ENTITLEMENT_UPDATE},
                { "ENTITLEMENT_DELETE"                      , gateway_events::ENTITLEMENT_DELETE},
                { "GUILD_CREATE"                            , gateway_events::GUILD_CREATE},
                { "GUILD_UPDATE"                            , gateway_events::GUILD_UPDATE},
                { "GUILD_DELETE"                            , gateway_events::GUILD_DELETE},
                { "GUILD_AUDIT_LOG_ENTRY_CREATE"            , gateway_events::GUILD_AUDIT_LOG_ENTRY_CREATE},
                { "GUILD_BAN_ADD"                           , gateway_events::GUILD_BAN_ADD},
                { "GUILD_BAN_REMOVE"                        , gateway_events::GUILD_BAN_REMOVE},
                { "GUILD_EMOJIS_UPDATE"                     , gateway_events::GUILD_EMOJIS_UPDATE},
                { "GUILD_STICKERS_UPDATE"                   , gateway_events::GUILD_STICKERS_UPDATE},
                { "GUILD_INTEGRATIONS_UPDATE"               , gateway_events::GUILD_INTEGRATIONS_UPDATE},
                { "GUILD_MEMBER_ADD"                        , gateway_events::GUILD_MEMBER_ADD},
                { "GUILD_MEMBER_REMOVE"                     , gateway_events::GUILD_MEMBER_REMOVE},
                { "GUILD_MEMBER_UPDATE"                     , gateway_events::GUILD_MEMBER_UPDATE},
                { "GUILD_MEMBERS_CHUNK"                     , gateway_events::GUILD_MEMBERS_CHUNK},
                { "GUILD_ROLE_CREATE"                       , gateway_events::GUILD_ROLE_CREATE},
                { "GUILD_ROLE_UPDATE"                       , gateway_events::GUILD_ROLE_UPDATE},
                { "GUILD_ROLE_DELETE"                       , gateway_events::GUILD_ROLE_DELETE},
                { "GUILD_SCHEDULED_EVENT_CREATE"            , gateway_events::GUILD_SCHEDULED_EVENT_CREATE},
                { "GUILD_SCHEDULED_EVENT_UPDATE"            , gateway_events::GUILD_SCHEDULED_EVENT_UPDATE},
                { "GUILD_SCHEDULED_EVENT_DELETE"            , gateway_events::GUILD_SCHEDULED_EVENT_DELETE},
                { "GUILD_SCHEDULED_EVENT_USER_ADD"          , gateway_events::GUILD_SCHEDULED_EVENT_USER_ADD},
                { "GUILD_SCHEDULED_EVENT_USER_REMOVE"       , gateway_events::GUILD_SCHEDULED_EVENT_USER_REMOVE},
                { "INTEGRATION_CREATE"                      , gateway_events::INTEGRATION_CREATE},
                { "INTEGRATION_UPDATE"                      , gateway_events::INTEGRATION_UPDATE},
                { "INTEGRATION_DELETE"                      , gateway_events::INTEGRATION_DELETE},
                { "INTERACTION_CREATE"                      , gateway_events::INTERACTION_CREATE},
                { "INVITE_CREATE"                           , gateway_events::INVITE_CREATE},
                { "INVITE_DELETE"                           , gateway_events::INVITE_DELETE},
                { "MESSAGE_CREATE"                          , gateway_events::MESSAGE_CREATE},
                { "MESSAGE_UPDATE"                          , gateway_events::MESSAGE_UPDATE},
                { "MESSAGE_DELETE"                          , gateway_events::MESSAGE_DELETE},
                { "MESSAGE_DELETE_BULK"                     , gateway_events::MESSAGE_DELETE_BULK},
                { "MESSAGE_REACTION_ADD"                    , gateway_events::MESSAGE_REACTION_ADD},
                { "MESSAGE_REACTION_REMOVE"                 , gateway_events::MESSAGE_REACTION_REMOVE},
                { "MESSAGE_REACTION_REMOVE_ALL"             , gateway_events::MESSAGE_REACTION_REMOVE_ALL},
                { "MESSAGE_REACTION_REMOVE_EMOJI"           , gateway_events::MESSAGE_REACTION_REMOVE_EMOJI},
                { "PRESENCE_UPDATE"                         , gateway_events::PRESENCE_UPDATE},
                { "STAGE_INSTANCE_CREATE"                   , gateway_events::STAGE_INSTANCE_CREATE},
                { "STAGE_INSTANCE_UPDATE"                   , gateway_events::STAGE_INSTANCE_UPDATE},
                { "STAGE_INSTANCE_DELETE"                   , gateway_events::STAGE_INSTANCE_DELETE},
                { "TYPING_START"                            , gateway_events::TYPING_START},
                { "USER_UPDATE"                             , gateway_events::USER_UPDATE},
                { "VOICE_STATE_UPDATE"                      , gateway_events::VOICE_STATE_UPDATE},
                { "VOICE_SERVER_UPDATE"                     , gateway_events::VOICE_SERVER_UPDATE},
                { "WEBHOOKS_UPDATE"                         , gateway_events::WEBHOOKS_UPDATE}
            };

            if (const auto& it = map.find(s); it != map.end()) return it->second;
            return gateway_events::UNKNOWN;
        }
        
        gateway_intents operator|(const gateway_intents& a, const gateway_intents& b)
        {
            return static_cast<gateway_intents>(static_cast<std::underlying_type_t<gateway_intents>>(a) | static_cast<std::underlying_type_t<gateway_intents>>(b));
        }
        

        gateway_status_binary operator+=(gateway_status_binary& a, const gateway_status_binary& b)
        {
            return a = static_cast<gateway_status_binary>(static_cast<std::underlying_type_t<gateway_status_binary>>(a) | static_cast<std::underlying_type_t<gateway_status_binary>>(b));
        }

        gateway_status_binary operator-=(gateway_status_binary& a, const gateway_status_binary& b)
        {
            return a = static_cast<gateway_status_binary>(static_cast<std::underlying_type_t<gateway_status_binary>>(a) & ~static_cast<std::underlying_type_t<gateway_status_binary>>(b));
        }

        gateway_status_binary operator+(const gateway_status_binary& a, const gateway_status_binary& b)
        {
            return static_cast<gateway_status_binary>(static_cast<std::underlying_type_t<gateway_status_binary>>(a) | static_cast<std::underlying_type_t<gateway_status_binary>>(b));
        }

        gateway_status_binary operator-(const gateway_status_binary& a, const gateway_status_binary& b)
        {
            return static_cast<gateway_status_binary>(static_cast<std::underlying_type_t<gateway_status_binary>>(a) & ~static_cast<std::underlying_type_t<gateway_status_binary>>(b));
        }
        gateway_status_binary operator&(const gateway_status_binary& a, const gateway_status_binary& b)
        {
            return static_cast<gateway_status_binary>(static_cast<std::underlying_type_t<gateway_status_binary>>(a) & static_cast<std::underlying_type_t<gateway_status_binary>>(b));
        }


        gateway_payload_structure::gateway_payload_structure(const size_t payload_len)
            : d(new char[payload_len]), d_len(payload_len)
        {}

        gateway_payload_structure::~gateway_payload_structure()
        {
            free();
        }

        int gateway_payload_structure::append(const char* data, const size_t length, const size_t offset)
        {
            if (!d) return -1;

            memcpy(d + offset, data, length);

            // Got to end?
            if (offset + length == d_len) { 
                j = new pJSON(d, d_len);
                DEL_EM(d);

                const auto j_op = (*j)["op"];
                const auto j_s  = (*j)["s"];
                const auto j_t  = (*j)["t"];

                this->op = static_cast<gateway_opcodes>(j_op.to_int());
                this->s  = j_s.to_uint64();
                this->t  = str2gateway_events(j_t.to_string());

                return 1;
            }

            return 0;
        }

        void gateway_payload_structure::free()
        {
            DEL_EM(d);
            DEL_IT(j);
        }


        Gateway::gateway_data::gateway_data(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : m_intents(intents), m_token(token), m_event_handler(evhlr)
        {
            ESP_LOGI(TAG, "Initializing Gateway version %s for %s...", app_version, target_app);

            ESP_LOGI(TAG, "Loading certificates...");
                        
            READFILE_FULLY_TO(m_gateway_cert_perm, cert_gateway_path, "Could not load gateway certificate.");

            ESP_LOGI(TAG, "Certificate file loaded:\n\n%s\n", m_gateway_cert_perm.c_str());
            
            esp_websocket_client_config_t ws_cfg = {
                .uri = (const char*) gateway_url,
                .disable_auto_reconnect = true,
                .task_prio = gateway_self_priority,
                .task_stack = gateway_stack_size,
                .buffer_size = gateway_buffer_size,
                .cert_pem = m_gateway_cert_perm.c_str()
            };
            
            ESP_LOGI(TAG, "ESP Websocket client...");

            if (!(m_client_handle = esp_websocket_client_init(&ws_cfg))) {
                throw Exception("Can't init esp websocket client");
            }

            if (esp_websocket_register_events(m_client_handle, WEBSOCKET_EVENT_ANY, &__c_gateway_redirect_event, (void*)this) != ESP_OK) {

                esp_websocket_client_destroy(m_client_handle);
                m_client_handle = nullptr;
            }
            

            ESP_LOGI(TAG, "Initialized Gateway.");
        }

        Gateway::gateway_data::~gateway_data()
        {
            DEL_IT(m_pay_work);

            if (m_heartbeat_task) vTaskDelete(m_heartbeat_task);
            m_heartbeat_task = nullptr;
        }


        Gateway::Gateway(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : data(new gateway_data(token, intents, evhlr))
        {            
        }

        Gateway::~Gateway()
        {
            stop();
            DEL_IT(data);
        }
        
        void Gateway::stop()
        {

        }

        // Send heartbeats
        void __c_async_heartbeat(void* param)
        {
            Gateway::gateway_data& gw_data = *(Gateway::gateway_data*)param;
            
            ESP_LOGI(TAG, "[HB] Task HEARTBEAT started with %lu ms of heartbeat time.", gw_data.m_heartbeat_interval_ms);

            while((gw_data.m_stats & gateway_status_binary::CONNECTED) == gateway_status_binary::CONNECTED)
            {
                if (!gw_data.m_heartbeat_got_confirm) {
                    ESP_LOGI(TAG, "[HB] Task HEARTBEAT did not get ACK in time... Restart?");
                    gw_data.m_stats += gateway_status_binary::HEARTBEAT_NO_ACK;
                }

                gw_data.m_heartbeat_got_confirm = false;

                ESP_LOGI(TAG, "[HB] Task HEARTBEAT sending heartbeat...");


                char* _allocated;
                if (gw_data.m_last_sequence_number >= 0) {
                    saprintf(_allocated, "{\"op\": %i, \"d\": %lli}", static_cast<int>(gateway_send_events::HEARTBEAT), gw_data.m_last_sequence_number);
                }
                else {
                    saprintf(_allocated, "{\"op\": %i, \"d\": null}", static_cast<int>(gateway_send_events::HEARTBEAT));
                }


                // SEND HEARTBEAT THINGY

                //const std::string json_to_send = 
                //"{"
                //    "\"op\":" + std::to_string(static_cast<int>(gateway_send_events::HEARTBEAT)) + ","
                //    "\"d\":" + (unique_cfg.sequence_number != -1 ? std::to_string(unique_cfg.sequence_number) : "null") +
                //"}";
                // SEE GATEWAY_HANDLING

                time_t end_time = time(nullptr) + static_cast<time_t>(gw_data.m_heartbeat_interval_ms);

                ESP_LOGI(TAG, "[HB] Task HEARTBEAT idle.");

                while(time(nullptr) < end_time) {
                    if ((gw_data.m_stats & gateway_status_binary::HEARTBEAT_NEEDED) == gateway_status_binary::HEARTBEAT_NEEDED) {
                        gw_data.m_stats -= gateway_status_binary::HEARTBEAT_NEEDED;
                        gw_data.m_heartbeat_got_confirm = true; // assume yes
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(100));
                }                
            }

            vTaskDelete(nullptr);
        }
        
        void __c_gateway_redirect_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
        {
            Gateway::gateway_data& gw_data = *(Gateway::gateway_data*)event_handler_arg;            
            esp_websocket_event_data_t* data = (esp_websocket_event_data_t*)event_data;

            switch (event_id) {
            case WEBSOCKET_EVENT_CONNECTED:
                ESP_LOGW(TAG, "[EV] Initialized Gateway.");
                gw_data.m_stats += gateway_status_binary::CONNECTED;
                gw_data.m_stats -= gateway_status_binary::CLOSED;
                gw_data.m_stats -= gateway_status_binary::ERROR_NEED_HELP;

                break;
            case WEBSOCKET_EVENT_DATA:
            {
                if(data->op_code != WS_TRANSPORT_OPCODES_TEXT && data->op_code != WS_TRANSPORT_OPCODES_CLOSE) break;

                if (data->payload_offset == 0) { DEL_IT(gw_data.m_pay_work); gw_data.m_pay_work = new gateway_payload_structure(data->payload_len); }
                const int ret = gw_data.m_pay_work->append(data->data_ptr, data->data_len, data->payload_offset);

                gateway_payload_structure& ps = *gw_data.m_pay_work;

                // SUCCESS!
                if (ret == 1) {
                    if (ps.s >= 0) gw_data.m_last_sequence_number = ps.s;

                    switch(ps.op) {
                    case gateway_opcodes::DISPATCH:                 // [RECEIVE]        // An event was dispatched.
                        switch(ps.t) { // note: hello is managed @ opcode level
                        case gateway_events::READY:                 // Contains the initial state information
                            gw_data.m_session_id    = (*ps.j)["d"]["session_id"].to_string();
                            gw_data.m_bot_id        = (*ps.j)["d"]["user"]["id"].to_uint64();
                            gw_data.m_bot_string    = (*ps.j)["d"]["user"]["username"].to_string();
                            
                            ESP_LOGI(TAG, "[EV] Ready, started with session=%s, id=%llu, username=%s.", gw_data.m_session_id.c_str(), gw_data.m_bot_id, gw_data.m_bot_string.c_str());

                            break;
                        case gateway_events::RESUMED:

                            break;
                        /*case gateway_events::RECONNECT:

                            break;
                        case gateway_events::INVALID_SESSION:

                            break;*/
                        default:
                            break;
                        }
                        break;
                    case gateway_opcodes::HEARTBEAT:                // [SEND/RECEIVE]   // Fired periodically by the client to keep the connection alive.
                        ESP_LOGW(TAG, "[EV] OP: HEARTBEAT, Discord asked for one.");
                        gw_data.m_stats += gateway_status_binary::HEARTBEAT_NEEDED;
                        break;
                    case gateway_opcodes::RECONNECT:                // [RECEIVE]        // You should attempt to reconnect and resume immediately.
                        // NOT IMPLEMENTED
                        TABLE_FLIP_CHIP_I_AM_DEAD();
                        break;
                    case gateway_opcodes::INVALID_SESSION:          // [RECEIVE]        // The session has been invalidated. You should reconnect and identify/resume accordingly.
                        // NOT IMPLEMENTED
                        TABLE_FLIP_CHIP_I_AM_DEAD();
                        break;
                    case gateway_opcodes::HELLO:                    // [RECEIVE]        // Sent immediately after connecting, contains the heartbeat_interval to use.
                        ESP_LOGI(TAG, "[EV] OP: HELLO, working on it...");
                        {
                            gw_data.m_heartbeat_interval_ms = static_cast<int32_t>((*ps.j)["d"]["heartbeat_interval"].to_int());
                            if (gw_data.m_heartbeat_interval_ms < 100) {
                                ESP_LOGE(TAG, "[EV] PANIC: FATAL ERROR ON HEARTBEAT_INTERVAL_MS AT HELLO EVENT, TIME IS BROKEN (< 100 ms). RESTARTING SELF. DROPPING OFF!");
                                TABLE_FLIP_CHIP_I_AM_DEAD();
                            }

                            gw_data.m_heartbeat_got_confirm = true;
                            if (gw_data.m_heartbeat_task) vTaskDelete(gw_data.m_heartbeat_task);

                            xTaskCreate(__c_async_heartbeat, "HEARTBEAT", 2048, event_handler_arg, gateway_heartbeat_task_priority, &gw_data.m_heartbeat_task);
                            if (gw_data.m_heartbeat_task == nullptr) {
                                ESP_LOGE(TAG, "[EV] PANIC: FATAL ERROR ON XTASKCREATE AT HELLO EVENT, CANNOT CREATE ESSENTIAL TASK FOR LIFE. RESTARTING SELF. DROPPING OFF!");
                                TABLE_FLIP_CHIP_I_AM_DEAD();
                            }
                        }
                        break;
                    case gateway_opcodes::HEARTBEAT_ACK:            // [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
                        ESP_LOGI(TAG, "[EV] OP: HEARTBEAT ACK, good.");
                        gw_data.m_heartbeat_got_confirm = true;
                        break;
                    default:
                        break;
                    }
                }
            }
                break;
            case WEBSOCKET_EVENT_ERROR:
                ESP_LOGE(TAG, "[EV] Error on gateway.");
                gw_data.m_stats += gateway_status_binary::ERROR_NEED_HELP;

                break;
            case WEBSOCKET_EVENT_DISCONNECTED:            
                ESP_LOGW(TAG, "[EV] Gateway disconnected.");
                gw_data.m_stats -= gateway_status_binary::CONNECTED;

                break;
            case WEBSOCKET_EVENT_CLOSED:
                ESP_LOGW(TAG, "[EV] Gateway closed cleanly.");
                gw_data.m_stats += gateway_status_binary::CLOSED;

                break;
            default:
                ESP_LOGW(TAG, "[EV] Gateway got UNKNOWN EVENT ID. Not handling it.");
                break;
            }
        }

    }
}