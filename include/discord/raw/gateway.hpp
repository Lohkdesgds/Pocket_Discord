#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_websocket_client.h"
#include "esp_transport_ws.h"

#include <string>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <functional>
#include <memory>

#include "discord/raw/time_iso.hpp"
#include "tasker.hpp"
#include "memory_file.hpp"
#include "memory_json.hpp"

//#define LSW_GATEWAY_DEBUGGING_AS_INFO
//#define LSW_SHOW_DETAILED_EVENTS // event tasking and heartbeats
//#define LSW_SHOW_JSON_SENT_CONFIRMATION_SUCCESS
//#define LSW_SHOW_JSON_BEING_SENT

namespace LSW {
    namespace PocketDiscord {

        const char gateway_url[] = "wss://gateway.discord.gg/?v=9&encoding=json";
        constexpr int gateway_max_timeout = 5000;
        constexpr size_t gateway_stack_size = 16 * 1024;
        constexpr size_t gateway_poll_stack_size = 17 * 1024;
        constexpr unsigned gateway_queue_size_default = 30;
        constexpr unsigned gateway_poll_event_priority = 3;

        const std::string app_version = "V0.1 BETA";
        const std::string target_app = "ESP32";
        
        // from Discord @ https://discord.com/developers/docs/topics/opcodes-and-status-codes#gateway-gateway-opcodes
        enum class gateway_opcodes {
            UNKNOWN                 = -1,       // [MINE]           // Default creation, not related to Discord.
            DISPATCH                = 0,        // [RECEIVE]        // An event was dispatched.
            HEARTBEAT               = 1,        // [SEND/RECEIVE]   // Fired periodically by the client to keep the connection alive.
            IDENTIFY                = 2,        // [SEND]           // Starts a new session during the initial handshake.
            PRESENCE_UPDATE_SEND    = 3,        // [SEND]           // Update the client's presence.
            VOICE_STATE_UPDATE      = 4,        // [SEND]           // Used to join/leave or move between voice channels.
            RESUME                  = 6,        // [SEND]           // Resume a previous session that was disconnected.
            RECONNECT               = 7,        // [RECEIVE]        // You should attempt to reconnect and resume immediately.
            REQUEST_GUILD_MEMBERS   = 8,        // [SEND]           // Request information about offline guild members in a large guild.
            INVALID_SESSION         = 9,        // [RECEIVE]        // The session has been invalidated. You should reconnect and identify/resume accordingly.
            HELLO                   = 10,       // [RECEIVE]        // Sent immediately after connecting, contains the heartbeat_interval to use.
            HEARTBEAT_ACK           = 11,       // [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
            _MAX = HEARTBEAT_ACK                // for reference
        };

        // from Discord @ https://discord.com/developers/docs/topics/opcodes-and-status-codes#gateway-gateway-close-event-codes
        enum class gateway_close_event_codes {
            UNKNOWN_ERROR               = 4000,	    // We're not sure what went wrong. Try reconnecting?
            UNKNOWN_OPCODE              = 4001,	    // You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
            DECODE_ERROR                = 4002,	    // You sent an invalid payload to us. Don't do that!
            NOT_AUTHENTICATED           = 4003,	    // You sent us a payload prior to identifying.
            AUTHENTICATION_FAILED       = 4004,	    // The account token sent with your identify payload is incorrect.
            ALREADY_AUTHENTICATED       = 4005,	    // You sent more than one identify payload. Don't do that!
            INVALID_SEQ                 = 4007,	    // The sequence sent when resuming the session was invalid. Reconnect and start a new session.
            RATE_LIMITED                = 4008,	    // Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
            SESSION_TIMED_OUT           = 4009,	    // Your session timed out. Reconnect and start a new one.
            INVALID_SHARD               = 4010,	    // You sent us an invalid shard when identifying.
            SHARDING_REQUIRED           = 4011,	    // The session would have handled too many guilds - you are required to shard your connection in order to connect.
            INVALID_API_VERSION         = 4012,	    // You sent an invalid version for the gateway.
            INVALID_INTENTS             = 4013,	    // You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
            DISALLOWED_INTENTS          = 4014	    // You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
        };

        // from Discord, @ https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-commands
        // this is the STRING_THING I think
        enum class gateway_commands {
            UNKNOWN = static_cast<int>(gateway_opcodes::UNKNOWN),                             // Mine, unknown.
            IDENTIFY = static_cast<int>(gateway_opcodes::IDENTIFY),                           // Triggers the initial handshake with the gateway
            RESUME = static_cast<int>(gateway_opcodes::RESUME),                               // Resumes a dropped gateway connection
            HEARTBEAT = static_cast<int>(gateway_opcodes::HEARTBEAT),                         // Maintains an active gateway connection
            REQUEST_GUILD_MEMBERS = static_cast<int>(gateway_opcodes::REQUEST_GUILD_MEMBERS), // Requests members for a guild
            UPDATE_VOICE_STATE = static_cast<int>(gateway_opcodes::VOICE_STATE_UPDATE),       // Joins, moves, or disconnects the client from a voice channel
            UPDATE_PRESENCE = static_cast<int>(gateway_opcodes::PRESENCE_UPDATE_SEND)         // Updates a client's presence
        };

        // from Discord @ https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events
        // this happens as STRING_THING
        enum class gateway_events : int32_t {
            UNKNOWN = -1,                   // Extra case we don't know
            HELLO,                          // Defines the heartbeat interval
            READY,                          // Contains the initial state information
            RESUMED,                        // Response to Resume
            RECONNECT,                      // Server is going away, client should reconnect to gateway and resume
            INVALID_SESSION,                // Failure response to Identify or Resume or invalid active session
            APPLICATION_COMMAND_CREATE,     // New Slash Command was created
            APPLICATION_COMMAND_UPDATE,     // Slash Command was updated
            APPLICATION_COMMAND_DELETE,     // Slash Command was deleted
            CHANNEL_CREATE,                 // New guild channel created
            CHANNEL_UPDATE,                 // Channel was updated
            CHANNEL_DELETE,                 // Channel was deleted
            CHANNEL_PINS_UPDATE,            // Message was pinned or unpinned
            GUILD_CREATE,                   // Lazy-load for unavailable guild, guild became available, or user joined a new guild
            GUILD_UPDATE,                   // Guild was updated
            GUILD_DELETE,                   // Guild became unavailable, or user left/was removed from a guild
            GUILD_BAN_ADD,                  // User was banned from a guild
            GUILD_BAN_REMOVE,               // User was unbanned from a guild
            GUILD_EMOJIS_UPDATE,            // Guild emojis were updated
            GUILD_INTEGRATIONS_UPDATE,      // Guild integration was updated
            GUILD_MEMBER_ADD,               // New user joined a guild
            GUILD_MEMBER_REMOVE,            // User was removed from a guild
            GUILD_MEMBER_UPDATE,            // Guild member was updated
            GUILD_MEMBERS_CHUNK,            // Response to Request Guild Members
            GUILD_ROLE_CREATE,              // Guild role was created
            GUILD_ROLE_UPDATE,              // Guild role was updated
            GUILD_ROLE_DELETE,              // Guild role was deleted
            INTEGRATION_CREATE,             // Guild integration was created
            INTEGRATION_UPDATE,             // Guild integration was updated
            INTEGRATION_DELETE,             // Guild integration was deleted
            INTERACTION_CREATE,             // User used an interaction, such as a Slash Command
            INVITE_CREATE,                  // Invite to a channel was created
            INVITE_DELETE,                  // Invite to a channel was deleted
            MESSAGE_CREATE,                 // Message was created
            MESSAGE_UPDATE,                 // Message was edited
            MESSAGE_DELETE,                 // Message was deleted
            MESSAGE_DELETE_BULK,            // Multiple messages were deleted at once
            MESSAGE_REACTION_ADD,           // User reacted to a message
            MESSAGE_REACTION_REMOVE,        // User removed a reaction from a message
            MESSAGE_REACTION_REMOVE_ALL,    // All reactions were explicitly removed from a message
            MESSAGE_REACTION_REMOVE_EMOJI,  // All reactions for a given emoji were explicitly removed from a message
            PRESENCE_UPDATE,                // User was updated
            TYPING_START,                   // User started typing in a channel
            USER_UPDATE,                    // Properties about the user changed
            VOICE_STATE_UPDATE,             // Someone joined, left, or moved a voice channel
            VOICE_SERVER_UPDATE,            // Guild's voice server was updated
            WEBHOOKS_UPDATE                 // Guild channel webhook was created, update, or deleted
        };

        // from Discord @ https://discord.com/developers/docs/topics/gateway#gateway-intents
        enum class gateway_intents {
            GUILDS                      = (1 << 0),
            GUILD_MEMBERS               = (1 << 1),
            GUILD_BANS                  = (1 << 2),
            GUILD_EMOJIS                = (1 << 3),
            GUILD_INTEGRATIONS          = (1 << 4),
            GUILD_WEBHOOKS              = (1 << 5),
            GUILD_INVITES               = (1 << 6),
            GUILD_VOICE_STATES          = (1 << 7),
            GUILD_PRESENCES             = (1 << 8),
            GUILD_MESSAGES              = (1 << 9),
            GUILD_MESSAGE_REACTIONS     = (1 << 10),
            GUILD_MESSAGE_TYPING        = (1 << 11),
            DIRECT_MESSAGES             = (1 << 12),
            DIRECT_MESSAGE_REACTIONS    = (1 << 13),
            DIRECT_MESSAGE_TYPING       = (1 << 14),
            _ALL                        = (1 << 15) - 1
        };

        gateway_commands string_to_gateway_commands(const std::string&);
        std::string gateway_commands_to_string(const gateway_commands&);

        gateway_events string_to_gateway_events(const std::string&);
        std::string gateway_events_to_string(const gateway_events&);
        
        // from Discord @ https://discord.com/developers/docs/topics/gateway#payloads-gateway-payload-structure
        // this is used FROM and TO gateway.
        struct gateway_payload_structure {
            MemoryFile __fporig;
            MemoryFileJSON json;
            

            gateway_opcodes op = gateway_opcodes::UNKNOWN;
            long long s = -1;
            gateway_events t = gateway_events::UNKNOWN;

            bool all_good = false;


            gateway_payload_structure();

            gateway_payload_structure(const gateway_payload_structure&) = delete;
            void operator=(const gateway_payload_structure&) = delete;

            gateway_payload_structure(gateway_payload_structure&&);
            void operator=(gateway_payload_structure&&);

            ~gateway_payload_structure();

            void reset();

            gateway_payload_structure(MemoryFile&&);
        };

        // mine, based on ESP gateway response possibilites.
        enum class gateway_status {
            RECONNECT,          // Had it running, Discord called RECONNECT
            STARTUP,            // Started thread right now, first step.
            CONNECTED,          // WEBSOCKET_EVENT_CONNECTED
            ERROR,              // WEBSOCKET_EVENT_ERROR
            DISCONNECTED,       // WEBSOCKET_EVENT_DISCONNECTED
            CLOSED,             // WEBSOCKET_EVENT_CLOSED
            UNKNOWN             // other event not in list and not WEBSOCKET_EVENT_DATA (common data work)
        };

        struct unique_configuration {
            // USER CONFIG:
            std::string token;
            int intents = 0;

            // WORKING CONFIG:
            std::string session_id;
            std::string bot_string;
            unsigned long long bot_id = 0;
            gateway_status status = gateway_status::UNKNOWN;
            std::shared_mutex data_mtx; // lock if changing something!

            bool no_gateway_please = false; // skip gateway events for now?
            bool confirm_close = false; // gateway closed?
            bool resumed_successfully = false;

            volatile unsigned long long ack_heartbeat = 0; // OPCODE 11 HEARTBEAT ACK (see enum gateway_opcodes)
            volatile unsigned long long heartbeat_at = 0; // right now. Use get_time_now_ms().
            unsigned long long heartbeat_interval = 45000; // this is set on GATEWAY_HELLO
            int sequence_number = -1; // -1 must be sent as 'null' (no ''). (ref: https://discord.com/developers/docs/topics/gateway#heartbeat)

            std::mutex func_mtx;
            std::function<void(const gateway_events&, const MemoryFileJSON&)> func;

            std::shared_ptr<gateway_payload_structure> gw_data_temp;

            MemoryFile event_buffer_fp;
            EventPoll async_task;
        };

        gateway_commands string_to_gateway_commands(const std::string&);
        std::string gateway_commands_to_string(const gateway_commands&);

        gateway_events string_to_gateway_events(const std::string&);
        std::string gateway_events_to_string(const gateway_events&);

        class GatewayControl {
            unique_configuration unique_cfg;

            std::thread gateway_thr;
            bool gateway_thr_run = false;

            //std::mutex gateway_send_mtx; // safe send_json

            esp_websocket_client_handle_t client_handle = nullptr;

            unsigned long long last_gateway_send = 0; // count time, < 500 on send -> wait

            void gateway_handling();
            void heartbeat_interval_auto();

            // closes gateway, but not the gateway_thr! It might restart things up!
            void close_gateway();
            bool reconnect();

            bool send_raw_json(const std::string&);
        public:

            // setup and start discord gateway connection. string -> token, int -> intents, event_function_ptr -> function to handle discord events
            bool setup(const std::string&, const int, std::function<void(const gateway_events&, const MemoryFileJSON&)>);

            void stop();

            const std::string& get_token() const;
            unsigned long long get_bot_id() const;
            const std::string& get_bot_name() const;
            // op, json
            bool send_command(const gateway_commands&, const std::string&);
        };

        void __c_event_redir_autofree(void*, esp_event_base_t, int32_t, void*);
        void __c_gateway_redirect_event(void*, esp_event_base_t, int32_t, void*);
        // reason, (optional) websocket to close
        void __c_gateway_got_unhandled_disconnect_so_restart(const std::string&/*, esp_websocket_client_handle_t* = nullptr*/);
    }
}