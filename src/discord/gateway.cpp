#include "discord/gateway.hpp"

#include <unordered_map>
#include <memory>
#include <cJSON.h>
#include "esp_log.h"

#include "jsoninterface.h"
#include "defaults.h"

namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "GATEWAY";
        
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


        gateway_payload_structure::gateway_payload_structure(const size_t payload_len)
            : d(new char[payload_len]), d_len(payload_len)
        {}

        gateway_payload_structure::~gateway_payload_structure()
        {
            free();
        }

        bool gateway_payload_structure::append(const char* data, const size_t length, const size_t offset)
        {
            if (!d) return false;

            memcpy(d + offset, data, length);

            // Got to end?
            if (offset + length == d_len) { 
                j = new pJSON(d, d_len);
                DEL_EM(d);
            }

            return true;
        }

        void gateway_payload_structure::free()
        {
            DEL_EM(d);
            DEL_IT(j);
        }


        Gateway::gateway_data::gateway_data(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : m_intents(intents), m_token(token), m_event_handler(evhlr)
        {
        }

        Gateway::Gateway(const char* token, const gateway_intents intents, const Gateway::event_handler evhlr)
            : data(token, intents, evhlr)
        {
            ESP_LOGI(TAG, "Initializing Gateway version %s for %s...", app_version, target_app);

            ESP_LOGI(TAG, "Initialized Gateway.");
        }

        Gateway::~Gateway()
        {
            stop();
        }
        
        void Gateway::stop()
        {

        }

    }
}