#include "discord/gateway.hpp"

#include <unordered_map>
#include <memory>
#include "esp_log.h"
#include <time.h>
#include <stdexcept>
#include <string_view>

#include "LJSON/json.h"
#include "defaults.h"
#include "filehandler.h"
#include "useful.h"

#define RELEASE // remove spam

namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "GATEWAY";
        
        void __c_async_gateway_send_events(void*);
        void __c_event_handler_intermedium(void*);
        void __c_gateway_redirect_event(void*, esp_event_base_t, int32_t, void*);
        
        gateway_opcodes str2gateway_opcode(const char* s)
        {
            // size: 80 bytes + dynamic alloc
            static std::unordered_map<std::string_view, gateway_opcodes> const map = {
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
            static std::unordered_map<std::string_view, gateway_close_event_codes> const map = {
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
            static std::unordered_map<std::string_view, gateway_send_events> const map = {
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
            static std::unordered_map<std::string_view, gateway_events> const map = {
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
//                j = new JSON(d, d_len);
//
//                DEL_EM(d);

                if (last_event_block) delete last_event_block; // it was not moved lmao!
                last_event_block = new gateway_event_memory_block(d, d_len);
                d = nullptr;

                return 1;
            }

            return 0;
        }

        void gateway_payload_structure::free()
        {
            DEL_EM(d);
            DEL_IT(last_event_block); // delete if not moved
        }


        bool Gateway::gateway_data::send_raw_json(const char* str, const size_t len)
        {
            if (!m_client_handle) return false;
#ifndef RELEASE
            {
                ESP_LOGI(TAG, "\n# GATEWAY SENDING SIZE=%zu DATA=%s", len, str);
            }
#endif

            int sent_bytes = esp_websocket_client_send_text(m_client_handle, str, len, pdMS_TO_TICKS(gateway_max_timeout));
            const uint64_t rn = get_time_ms();
            
            if (sent_bytes != ESP_FAIL)
            {
                const uint64_t diff = rn - m_client_last_send;

                if (diff < 500 && m_client_last_send != 0) {
                    vTaskDelay(pdMS_TO_TICKS(500 - diff));
                }

                m_client_last_send = rn + 500;

                return true;
            }
            else {
                return false;
            }
        }

        void Gateway::gateway_data::summon_gateway_send_task()
        {
            if (m_gateway_send_task) return;
            xTaskCreate(__c_async_gateway_send_events, "GATEWAY2JOB", 3072, this, gateway_heartbeat_task_priority, &m_gateway_send_task);
            if (m_gateway_send_task == nullptr) {
                ESP_LOGE(TAG, "[EV] PANIC: FATAL ERROR ON XTASKCREATE AT HELLO EVENT, CANNOT CREATE ESSENTIAL TASK FOR LIFE. RESTARTING SELF. DROPPING OFF!");
                TABLE_FLIP_CHIP_I_AM_DEAD();
            }
        }

        // safe to call within itself
        void Gateway::gateway_data::destroy_gateway_send_task()
        {
            if (m_gateway_send_task) {
                auto* tmp = m_gateway_send_task;
                m_gateway_send_task = nullptr;
                vTaskDelete(tmp);
            }
        }

        Gateway::gateway_data::gateway_data(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : m_intents(intents), m_token(token), m_event_handler(evhlr), m_event_loop("GATEWAYC1E", 3, 4096, 4, 1)
        {
            ESP_LOGI(TAG, "Initializing Gateway for the first time, version %s for %s...", app_version, target_app);
            ESP_LOGI(TAG, "TOKEN: %s", m_token.c_str());

            ESP_LOGI(TAG, "Loading certificates...");
                        
            READFILE_FULLY_TO(m_gateway_cert_perm, cert_gateway_path, "Could not load gateway certificate.");

            ESP_LOGI(TAG, "Certificate file loaded:\n\n%s\n", m_gateway_cert_perm.c_str());
            
            start_gateway();
        }   

        Gateway::gateway_data::~gateway_data()
        {
            ESP_LOGI(TAG, "Ending gateway second task...");
            destroy_gateway_send_task();
            close_gateway();
        }

        void Gateway::gateway_data::start_gateway()
        {
            ESP_LOGI(TAG, "Starting gateway client...");

            esp_websocket_client_config_t ws_cfg = {
                .uri = (const char*) gateway_url,
                .disable_auto_reconnect = true,
                .task_prio = gateway_self_priority,
                .task_stack = gateway_stack_size,
                .buffer_size = gateway_buffer_size,
                .cert_pem = m_gateway_cert_perm.c_str()
            };
            
            if (!(m_client_handle = esp_websocket_client_init(&ws_cfg))) {
                throw std::runtime_error("Can't init esp websocket client");
            }

            ESP_LOGI(TAG, "Registering event handling...");

            if (esp_websocket_register_events(m_client_handle, WEBSOCKET_EVENT_ANY, &__c_gateway_redirect_event, (void*)this) != ESP_OK) {
                ESP_LOGE(TAG, "PANIC: Gateway could not have basic event set up properly. FORCING RESET.");
                TABLE_FLIP_CHIP_I_AM_DEAD();
            }
            
            ESP_LOGI(TAG, "Starting gateway client...");
            
            if (esp_websocket_client_start(m_client_handle) != ESP_OK) {
                ESP_LOGE(TAG, "PANIC: Gateway could not be launched. FORCING RESET.");
                TABLE_FLIP_CHIP_I_AM_DEAD();
            }

            ESP_LOGI(TAG, "Initialized Gateway!");
        }

        void Gateway::gateway_data::close_gateway()
        {
            if (!m_client_handle) return; // nothing to do

            ESP_LOGW(TAG, "Closing gateway...");
            
            if ((m_stats & gateway_status_binary::GW_CLOSED) == gateway_status_binary::GW_CLOSED) {
                ESP_LOGW(TAG, "Already closed? Ok...");
            }
            else if (esp_websocket_client_close_with_code(m_client_handle, 1012, nullptr, 0, pdMS_TO_TICKS(gateway_max_timeout)) != ESP_OK)
            {
                ESP_LOGE(TAG, "PANIC: Cannot close gateway with code 1012. FORCING RESET.");
                TABLE_FLIP_CHIP_I_AM_DEAD();
            }

            uint64_t timeout_wait = get_time_ms() + gateway_max_timeout * 2;
            
            ESP_LOGI(TAG, "Waiting for gateway to close...");
            
            while (((m_stats & gateway_status_binary::GW_CLOSED) != gateway_status_binary::GW_CLOSED) && (get_time_ms() < timeout_wait)) vTaskDelay(pdMS_TO_TICKS(100));
            if ((m_stats & gateway_status_binary::GW_CLOSED) != gateway_status_binary::GW_CLOSED) {
                ESP_LOGE(TAG, "PANIC: Gateway never closed. Timed out. FORCING RESET.");
                TABLE_FLIP_CHIP_I_AM_DEAD();
            }
            
            ESP_LOGI(TAG, "Gateway closed. Destroying it...");
            esp_websocket_client_destroy(m_client_handle);
            m_client_handle = nullptr;

            ESP_LOGI(TAG, "Cleaning up memory...");

            DEL_IT(m_pay_work);
            
            ESP_LOGI(TAG, "Gateway closed!");
        }
        


        Gateway::Gateway(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : data(new gateway_data(token, intents, evhlr))
        {            
        }

        Gateway::~Gateway()
        {
            DEL_IT(data);
        }
        
        void Gateway::stop()
        {
            DEL_IT(data);
        }

        gateway_event_memory_block::gateway_event_memory_block(char* s, const size_t l)
            : ref(s), j(new JSON((const char*)s, l)), d((*j)["d"])
        {
                const auto j_op = (*j)["op"];
                const auto j_s  = (*j)["s"];
                const auto j_t  = (*j)["t"];

                if (j_op.get_type() == JSON::type::NUMBER)   this->op = static_cast<gateway_opcodes>(j_op.get_int());
                else                                         this->op = gateway_opcodes::UNKNOWN;

                if (j_s.get_type() == JSON::type::NUMBER)    this->s = j_s.get_uint();
                else                                         this->s = -1;

                if (j_t.get_type() == JSON::type::STRING)    this->t = str2gateway_events(j_t);
                else                                         this->t = gateway_events::UNKNOWN;    
        }
        
        gateway_event_memory_block::~gateway_event_memory_block()
        {
            DEL_EM(ref);
            DEL_IT(j);
        }

        void __c_async_full_restart(void* param)
        {
            Gateway::gateway_data& gw_data = *(Gateway::gateway_data*)param;

            ESP_LOGI(TAG, "[GWEXT] RESTARTING GATEWAY EXTERNALLY IN 1 SECOND.");

            vTaskDelay(pdMS_TO_TICKS(1000));

            gw_data.close_gateway();
            gw_data.start_gateway();

            ESP_LOGI(TAG, "[GWEXT] DONE. DROPPING OFF.");

            vTaskDelete(NULL);
        }

        void __c_async_gateway_send_events(void* param)
        {
            Gateway::gateway_data& gw_data = *(Gateway::gateway_data*)param;
            
            ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND started with %lu ms of heartbeat time.", gw_data.m_heartbeat_interval_ms);

            char *_buffer = nullptr;
            size_t _len_buf = 0;
            uint64_t heartbeat_next = get_time_ms() + gw_data.m_heartbeat_interval_ms;
            bool heartbeat_caused_by_time = false;
            bool did_a_task = false;

            while ((gw_data.m_stats & gateway_status_binary::GW_CONNECTED) == gateway_status_binary::GW_CONNECTED)
            {
                heartbeat_caused_by_time = (get_time_ms() >= heartbeat_next);
                
                if (!did_a_task) vTaskDelay(pdMS_TO_TICKS(1000)); // free up resources a bit
                else vTaskDelay(pdMS_TO_TICKS(400)); // not 100% CPU usage from this thing
                did_a_task = false;

                if (
                    ((gw_data.m_stats & gateway_status_binary::DC_NEED_HEARTBEAT) == gateway_status_binary::DC_NEED_HEARTBEAT) || // if need, force it
                    (heartbeat_caused_by_time && ((gw_data.m_stats & gateway_status_binary::DC_HEARTBEAT_AWAKE) == gateway_status_binary::DC_HEARTBEAT_AWAKE)) // need to be awake
                ) {
                    did_a_task = true;
                    // Remove flags first
                    gw_data.m_stats -= gateway_status_binary::DC_NEED_HEARTBEAT;

                    if (!gw_data.m_heartbeat_got_confirm && heartbeat_caused_by_time) {
                        ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND did not get HEARTBEAT_ACK in time... Maybe restart?");
                        gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                    }

                    gw_data.m_heartbeat_got_confirm = false;

                    ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND sending heartbeat...");                    

                    if (gw_data.m_last_sequence_number >= 0) {
                        saprintf(_buffer, &_len_buf, "{\"op\":%i,\"d\":%lli}", static_cast<int>(gateway_send_events::HEARTBEAT), gw_data.m_last_sequence_number);
                    }
                    else {
                        saprintf(_buffer, &_len_buf, "{\"op\":%i,\"d\":null}", static_cast<int>(gateway_send_events::HEARTBEAT));
                    }


                    if (!gw_data.send_raw_json(_buffer, _len_buf)) {
                        ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND could not send heartbeat. Forcing try again with flag...");
                        gw_data.m_stats += gateway_status_binary::DC_NEED_HEARTBEAT;
                    }
                    else {
                        ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND sent heartbeat.");
                        heartbeat_next = get_time_ms() + static_cast<uint64_t>(gw_data.m_heartbeat_interval_ms);
                    }

                    DEL_EM(_buffer);
                }

                else if ((gw_data.m_stats & gateway_status_binary::DC_NEED_IDENTIFY) == gateway_status_binary::DC_NEED_IDENTIFY)
                {
                    did_a_task = true;
                    // Remove flags first
                    gw_data.m_stats -= gateway_status_binary::DC_NEED_IDENTIFY;

                    if (gw_data.m_session_id.size() == 0) {

                        ESP_LOGI(TAG, "[GWS] Identifying self...");

                        saprintf(_buffer, &_len_buf,
                            "{"
                                "\"op\":%i,"
                                "\"d\":{"
                                    "\"token\":\"%s\","
                                    "\"intents\":%lu,"
                                    "\"properties\":{"
                                        "\"$os\":\"esp-idf (%s)\","
                                        "\"$browser\":\"%s\","
                                        "\"$device\":\"%s\""
                                    "}"
                                "}"
                            "}",
                            static_cast<int>(gateway_send_events::IDENTIFY),
                            gw_data.m_token.c_str(),
                            static_cast<uint32_t>(gw_data.m_intents),
                            esp_get_idf_version(),
                            app_version,
                            target_app
                        );                    
                    }

                    else {                    
                        ESP_LOGI(TAG, "[GWS] Session ID still exists, trying to resume...");

                        saprintf(_buffer, &_len_buf,                            
                            "{"
                                "\"op\":%i,"
                                "\"d\":{"
                                    "\"token\":\"%s\","
                                    "\"session_id\":\"%s\","
                                    "\"seq\":%lli"
                                "}"
                            "}",
                            static_cast<int>(gateway_send_events::IDENTIFY),
                            gw_data.m_token.c_str(),
                            gw_data.m_session_id.c_str(),
                            gw_data.m_last_sequence_number
                        );
                    }
                    
                    if (!gw_data.send_raw_json(_buffer, _len_buf)) {
                        ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND could not send identify. Forcing try again with flag...");
                        gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                    }
                    else {
                        ESP_LOGI(TAG, "[GWS] Task GATEWAY_SEND sent identify.");
                    }

                    DEL_EM(_buffer);
                }
                else if ((gw_data.m_stats & gateway_status_binary::ANY_NEED_RESTART) == gateway_status_binary::ANY_NEED_RESTART)
                {
                    gw_data.m_stats -= gateway_status_binary::ANY_NEED_RESTART;
                    ESP_LOGW(TAG, "[GWS] Task GATEWAY_SEND got ANY_NEED_RESTART and is triggering assistant for gateway automated restart.");
                    xTaskCreate(__c_async_full_restart, "RESTARTGW", 1024, param, gateway_heartbeat_task_priority, NULL);
                    ESP_LOGW(TAG, "[GWS] Hopefully it does everything correct. Dropping this one off.");
                    // killing itself.
                    gw_data.destroy_gateway_send_task();
                    return;
                }
       
            }

            // removes self reference and kill itself.
            gw_data.destroy_gateway_send_task();
        }

        // workaround for single void* to Gateway func
        
        void ___del_event_data(void* p) {delete (gateway_event_memory_block*)p; }


        void __c_event_handler_intermedium(void* argument)
        {
            gateway_event_memory_block* evd = (gateway_event_memory_block*)argument;
            evd->func(evd->t, *evd->j);
        }
        
        void __c_gateway_redirect_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
        {
            Gateway::gateway_data& gw_data = *(Gateway::gateway_data*)event_handler_arg;
            esp_websocket_event_data_t* data = (esp_websocket_event_data_t*)event_data;

            try {
#ifndef RELEASE
                {
                    char* tmp = nullptr;
                    if (data->data_ptr && data->data_len > 0) {
                        tmp = new char[data->data_len + 1];
                        memcpy(tmp, data->data_ptr, data->data_len);
                        tmp[data->data_len] = '\0';
                    }

                    ESP_LOGI(TAG, "\n# RAW:\n# EVENT_ID=%li\n# OP_CODE=%i\n# PAYLOAD_LEN=%i\n# DATA_LEN=%i\n# PAYLOAD_OFFSET=%i\n# DATA=%s",
                        event_id,
                        data->op_code,
                        data->payload_len,
                        data->data_len,
                        data->payload_offset,
                        tmp ? tmp : "NULL");

                    if (tmp) delete[] tmp;
                }
#endif

                switch (event_id) {
                case WEBSOCKET_EVENT_CONNECTED:
                    ESP_LOGW(TAG, "[EV] Initialized Gateway.");
                    gw_data.m_stats = gateway_status_binary::GW_CONNECTED; // as start, start fresh
                    //gw_data.m_stats += gateway_status_binary::GW_CONNECTED;
                    //gw_data.m_stats -= gateway_status_binary::GW_CLOSED;
                    //gw_data.m_stats -= gateway_status_binary::GW_ERROR_NEED_HELP;
                    //gw_data.m_stats -= gateway_status_binary::ANY_NEED_RESTART;

                    break;
                case WEBSOCKET_EVENT_DATA:
                {
                    if(data->op_code != WS_TRANSPORT_OPCODES_TEXT && data->op_code != WS_TRANSPORT_OPCODES_CLOSE) break;

                    if (data->payload_offset == 0) { DEL_IT(gw_data.m_pay_work); gw_data.m_pay_work = new gateway_payload_structure(data->payload_len); }
                    const int ret = gw_data.m_pay_work->append(data->data_ptr, data->data_len, data->payload_offset);

                    gateway_payload_structure& ps = *gw_data.m_pay_work;
                    gateway_event_memory_block* eb = ps.last_event_block;

                    // SUCCESS! (end)
                    if (ret == 1 && eb) {
                        if (gw_data.m_last_sequence_number < eb->s) gw_data.m_last_sequence_number = eb->s;

                        switch(eb->op) {
                        case gateway_opcodes::DISPATCH:                 // [RECEIVE]        // An event was dispatched.
                            switch(eb->t) { // note: hello is managed @ opcode level
                            case gateway_events::READY:                 // Contains the initial state information

                                ESP_LOGI(TAG, "[EV] Retrieving user session and id...");

                                gw_data.m_session_id    = (eb->d)["session_id"].get_string();
                                gw_data.m_bot_id        = (eb->d)["user"]["id"].get_uint();
                                gw_data.m_bot_string    = (eb->d)["user"]["username"].get_string();
                                
                                ESP_LOGI(TAG, "[EV] Ready, started with session=%s, id=%llu, username=%s.", gw_data.m_session_id.c_str(), gw_data.m_bot_id, gw_data.m_bot_string.c_str());

                                break;
                            case gateway_events::RESUMED:
                                ESP_LOGI(TAG, "[EV] Got RESUMED!");
                                gw_data.m_stats += gateway_status_binary::DC_HEARTBEAT_AWAKE;
                                break;
                            case gateway_events::RECONNECT:
                                // NOT IMPLEMENTED
                                ESP_LOGW(TAG, "[EV] Got Reconnect from DISPATCH.");
                                gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                                //gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                                gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                                //TABLE_FLIP_CHIP_I_AM_DEAD();
                                break;
                            case gateway_events::INVALID_SESSION:
                                // NOT IMPLEMENTED
                                ESP_LOGE(TAG, "[EV] Got Invalid Session from DISPATCH.");
                                gw_data.m_session_id.free();
                                gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                                //gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                                gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                                //TABLE_FLIP_CHIP_I_AM_DEAD();
                                break;
                            default:
                                break;
                            }

                            if (gw_data.m_event_handler) {
                                eb->func = gw_data.m_event_handler;
                                //___event_data* evd = new ___event_data {ps.t, (JSON&&)(*ps.j), gw_data.m_event_handler };

                                // MOVING EB (ps.last_event_block) HERE:
                                gw_data.m_event_loop.post(FunctionWrapper(__c_event_handler_intermedium, (void*)eb , ___del_event_data));
                                eb =ps.last_event_block = nullptr;
                            }
                            break;
                        case gateway_opcodes::HEARTBEAT:                // [SEND/RECEIVE]   // Fired periodically by the client to keep the connection alive.
                            ESP_LOGW(TAG, "[EV] OP: HEARTBEAT, Discord asked for one.");
                            gw_data.m_stats += gateway_status_binary::DC_NEED_HEARTBEAT;
                            break;
                        case gateway_opcodes::RECONNECT:                // [RECEIVE]        // You should attempt to reconnect and resume immediately.
                            // NOT IMPLEMENTED
                            ESP_LOGW(TAG, "[EV] OP: Got Reconnect.");
                            gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                            //gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                            gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                            //TABLE_FLIP_CHIP_I_AM_DEAD();
                            break;
                        case gateway_opcodes::INVALID_SESSION:          // [RECEIVE]        // The session has been invalidated. You should reconnect and identify/resume accordingly.
                            // NOT IMPLEMENTED
                            ESP_LOGE(TAG, "[EV] OP: Got Invalid Session.");
                            gw_data.m_session_id.free();
                            gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                            //gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                            gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                            //TABLE_FLIP_CHIP_I_AM_DEAD();
                            break;
                        case gateway_opcodes::HELLO:                    // [RECEIVE]        // Sent immediately after connecting, contains the heartbeat_interval to use.
                            ESP_LOGI(TAG, "[EV] OP: HELLO, working on it...");
                            {
                                gw_data.m_heartbeat_interval_ms = static_cast<int32_t>((eb->d)["heartbeat_interval"].get_int());
                                if (gw_data.m_heartbeat_interval_ms < 100) {
                                    ESP_LOGE(TAG, "[EV] PANIC: FATAL ERROR ON HEARTBEAT_INTERVAL_MS AT HELLO EVENT, TIME IS BROKEN (< 100 ms). RESTARTING SELF. DROPPING OFF!");
                                    TABLE_FLIP_CHIP_I_AM_DEAD();
                                }

                                gw_data.m_stats += gateway_status_binary::DC_HEARTBEAT_AWAKE;
                                gw_data.m_stats += gateway_status_binary::DC_NEED_HEARTBEAT;
                                gw_data.m_stats += gateway_status_binary::DC_NEED_IDENTIFY;
                                gw_data.m_heartbeat_got_confirm = true;

                                ESP_LOGI(TAG, "[EV] HELLO triggered gateway send task, identifying soon...");
                                gw_data.summon_gateway_send_task();
                            }
                            break;

                        // ========== | OK | ========== //
                        case gateway_opcodes::HEARTBEAT_ACK:            // [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
                            ESP_LOGI(TAG, "[EV] OP: HEARTBEAT ACK, good.");
                            //gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_NO_ACK; // just unflag the error if gets heartbeat
                            gw_data.m_heartbeat_got_confirm = true;
                            break;
                        default:
                            break;
                        }
                    }
                }
                    break;
                case WEBSOCKET_EVENT_ERROR:
                    ESP_LOGE(TAG, "[EV] Error on gateway. Asking for restart...");
                    gw_data.m_stats += gateway_status_binary::ANY_NEED_RESTART;
                    break;
                case WEBSOCKET_EVENT_DISCONNECTED:            
                    ESP_LOGW(TAG, "[EV] Gateway disconnected.");
                    gw_data.m_stats -= gateway_status_binary::GW_CONNECTED;
                    gw_data.m_stats += gateway_status_binary::GW_CLOSED; // just so flag is set. Id doesn't mean it was clean.
                    gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                    break;
                case WEBSOCKET_EVENT_CLOSED:
                    ESP_LOGW(TAG, "[EV] Gateway closed cleanly.");
                    gw_data.m_stats += gateway_status_binary::GW_CLOSED;
                    gw_data.m_stats -= gateway_status_binary::DC_HEARTBEAT_AWAKE;
                    break;
                default:
                    ESP_LOGW(TAG, "[EV] Gateway got UNKNOWN EVENT ID. Not handling it.");
                    break;
                }
            }
            catch(const std::exception& e)
            {
                ESP_LOGE(TAG, "[EV] EXCEPTION! Error: %s", e.what());
            }
            catch(...) {                
                ESP_LOGE(TAG, "[EV] EXCEPTION! Unknown error");
            }

        }

    }
}