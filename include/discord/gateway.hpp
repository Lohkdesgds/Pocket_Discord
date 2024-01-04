#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_websocket_client.h"
#include "esp_transport_ws.h"

#include "../eventhandler.h"
#include "../filehandler.h"
#include "heapstring.h"
#include "LJSON/json.h"
#include "smartjsonalloc.h"


namespace Lunaris {
    namespace PocketDiscord {

        const char gateway_url[] = "wss://gateway.discord.gg/?v=10&encoding=json";

        constexpr int gateway_max_timeout = 5000;
        constexpr size_t gateway_stack_size = 12 * 1024;        // USING
        constexpr size_t gateway_poll_stack_size = 16 * 1024;
        constexpr size_t gateway_buffer_size = 4096;            // USING

        constexpr unsigned gateway_self_priority = 1;           // USING
        constexpr unsigned gateway_heartbeat_task_priority = 5; // USING
        constexpr unsigned gateway_queue_size_default = 30;
        constexpr unsigned gateway_poll_event_priority = 50;
        
        const char cert_gateway_path[] = "cert/gateway.pem";
        
        // from Discord @ https://discord.com/developers/docs/topics/opcodes-and-status-codes#gateway-gateway-opcodes
        enum class gateway_opcodes : int16_t {
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
            HEARTBEAT_ACK           = 11        // [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
            //_MAX = HEARTBEAT_ACK                // for reference
        };
        gateway_opcodes str2gateway_opcode(const char*);

        // from Discord @ https://discord.com/developers/docs/topics/opcodes-and-status-codes#gateway-gateway-close-event-codes
        enum class gateway_close_event_codes : int16_t {
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
        gateway_close_event_codes str2gateway_close_event_codes(const char*);

        // from Discord @ https://discord.com/developers/docs/topics/gateway-events#send-events
        // previously called gateway_commands
        enum class gateway_send_events : int16_t {
            UNKNOWN                 = static_cast<int>(gateway_opcodes::UNKNOWN),                   // Mine, unknown.
            IDENTIFY                = static_cast<int>(gateway_opcodes::IDENTIFY),                  // Triggers the initial handshake with the gateway
            RESUME                  = static_cast<int>(gateway_opcodes::RESUME),                    // Resumes a dropped gateway connection
            HEARTBEAT               = static_cast<int>(gateway_opcodes::HEARTBEAT),                 // Maintains an active gateway connection
            REQUEST_GUILD_MEMBERS   = static_cast<int>(gateway_opcodes::REQUEST_GUILD_MEMBERS),     // Requests members for a guild
            UPDATE_VOICE_STATE      = static_cast<int>(gateway_opcodes::VOICE_STATE_UPDATE),        // Joins, moves, or disconnects the client from a voice channel
            UPDATE_PRESENCE         = static_cast<int>(gateway_opcodes::PRESENCE_UPDATE_SEND)       // Updates a client's presence
        };
        gateway_send_events str2gateway_send_events(const char*);

        
        // from Discord @ https://discord.com/developers/docs/topics/gateway#commands-and-events-gateway-events
        // this happens as STRING_THING
        enum class gateway_events : int16_t {
            UNKNOWN = -1,                               // Extra case we don't know
            HELLO,                                      // Defines the heartbeat interval
            READY,                                      // Contains the initial state information
            RESUMED,                                    // Response to Resume
            RECONNECT,                                  // Server is going away, client should reconnect to gateway and resume
            INVALID_SESSION,                            // Failure response to Identify or Resume or invalid active session
            APPLICATION_COMMAND_PERMISSION_UPDATE,      // Application command permission was updated
            AUTO_MODERATION_RULE_CREATE,                // Auto Moderation rule was created
            AUTO_MODERATION_RULE_UPDATE,                // Auto Moderation rule was updated
            AUTO_MODERATION_RULE_DELETE,                // Auto Moderation rule was deleted
            AUTO_MODERATION_ACTION_EXECUTION,           // Auto Moderation rule was triggered and an action was executed (e.g. a message was blocked)
            CHANNEL_CREATE,                             // New guild channel created
            CHANNEL_UPDATE,                             // Channel was updated
            CHANNEL_DELETE,                             // Channel was deleted
            CHANNEL_PINS_UPDATE,                        // Message was pinned or unpinned
            THREAD_CREATE,                              // Thread created, also sent when being added to a private thread
            THREAD_UPDATE,                              // Thread was updated
            THREAD_DELETE,                              // Thread was deleted
            THREAD_LIST_SYNC,                           // Sent when gaining access to a channel, contains all active threads in that channel
            THREAD_MEMBER_UPDATE,                       // Thread member for the current user was updated
            THREAD_MEMBERS_UPDATE,                      // Some user(s) were added to or removed from a thread
            ENTITLEMENT_CREATE,                         // Entitlement was created
            ENTITLEMENT_UPDATE,                         // Entitlement was updated or renewed
            ENTITLEMENT_DELETE,                         // Entitlement was deleted
            GUILD_CREATE,                               // Lazy-load for unavailable guild, guild became available, or user joined a new guild
            GUILD_UPDATE,                               // Guild was updated
            GUILD_DELETE,                               // Guild became unavailable, or user left/was removed from a guild
            GUILD_AUDIT_LOG_ENTRY_CREATE,               // A guild audit log entry was created
            GUILD_BAN_ADD,                              // User was banned from a guild
            GUILD_BAN_REMOVE,                           // User was unbanned from a guild
            GUILD_EMOJIS_UPDATE,                        // Guild emojis were updated
            GUILD_STICKERS_UPDATE,                      // Guild stickers were updated
            GUILD_INTEGRATIONS_UPDATE,                  // Guild integration was updated
            GUILD_MEMBER_ADD,                           // New user joined a guild
            GUILD_MEMBER_REMOVE,                        // User was removed from a guild
            GUILD_MEMBER_UPDATE,                        // Guild member was updated
            GUILD_MEMBERS_CHUNK,                        // Response to Request Guild Members
            GUILD_ROLE_CREATE,                          // Guild role was created
            GUILD_ROLE_UPDATE,                          // Guild role was updated
            GUILD_ROLE_DELETE,                          // Guild role was deleted
            GUILD_SCHEDULED_EVENT_CREATE,               // Guild scheduled event was created
            GUILD_SCHEDULED_EVENT_UPDATE,               // Guild scheduled event was updated
            GUILD_SCHEDULED_EVENT_DELETE,               // Guild scheduled event was deleted
            GUILD_SCHEDULED_EVENT_USER_ADD,             // User subscribed to a guild scheduled event
            GUILD_SCHEDULED_EVENT_USER_REMOVE,          // User unsubscribed from a guild scheduled event
            INTEGRATION_CREATE,                         // Guild integration was created
            INTEGRATION_UPDATE,                         // Guild integration was updated
            INTEGRATION_DELETE,                         // Guild integration was deleted
            INTERACTION_CREATE,                         // User used an interaction, such as an Application Command
            INVITE_CREATE,                              // Invite to a channel was created
            INVITE_DELETE,                              // Invite to a channel was deleted
            MESSAGE_CREATE,                             // Message was created
            MESSAGE_UPDATE,                             // Message was edited
            MESSAGE_DELETE,                             // Message was deleted
            MESSAGE_DELETE_BULK,                        // Multiple messages were deleted at once
            MESSAGE_REACTION_ADD,                       // User reacted to a message
            MESSAGE_REACTION_REMOVE,                    // User removed a reaction from a message
            MESSAGE_REACTION_REMOVE_ALL,                // All reactions were explicitly removed from a message
            MESSAGE_REACTION_REMOVE_EMOJI,              // All reactions for a given emoji were explicitly removed from a message
            PRESENCE_UPDATE,                            // User was updated
            STAGE_INSTANCE_CREATE,                      // Stage instance was created
            STAGE_INSTANCE_UPDATE,                      // Stage instance was updated
            STAGE_INSTANCE_DELETE,                      // Stage instance was deleted or closed
            TYPING_START,                               // User started typing in a channel
            USER_UPDATE,                                // Properties about the user changed
            VOICE_STATE_UPDATE,                         // Someone joined, left, or moved a voice channel
            VOICE_SERVER_UPDATE,                        // Guild's voice server was updated
            WEBHOOKS_UPDATE                             // Guild channel webhook was created, update, or deleted
        };
        gateway_events str2gateway_events(const char*);

        // from Discord @ https://discord.com/developers/docs/topics/gateway#gateway-intents
        enum class gateway_intents : uint32_t {
            NONE                            = 0,
            GUILDS                          = (1 << 0),
            GUILD_MEMBERS                   = (1 << 1),
            GUILD_MODERATION                = (1 << 2),
            GUILD_EMOJIS_AND_STICKERS       = (1 << 3),
            GUILD_INTEGRATIONS              = (1 << 4),
            GUILD_WEBHOOKS                  = (1 << 5),
            GUILD_INVITES                   = (1 << 6),
            GUILD_VOICE_STATES              = (1 << 7),
            GUILD_PRESENCES                 = (1 << 8),
            GUILD_MESSAGES                  = (1 << 9),
            GUILD_MESSAGE_REACTIONS         = (1 << 10),
            GUILD_MESSAGE_TYPING            = (1 << 11),
            DIRECT_MESSAGES                 = (1 << 12),
            DIRECT_MESSAGE_REACTIONS        = (1 << 13),
            DIRECT_MESSAGE_TYPING           = (1 << 14),
            MESSAGE_CONTENT                 = (1 << 15),
            GUILD_SCHEDULED_EVENTS          = (1 << 16),
            AUTO_MODERATION_CONFIGURATION   = (1 << 20),
            AUTO_MODERATION_EXECUTION       = (1 << 21)
        };

        gateway_intents operator|(const gateway_intents&, const gateway_intents&);

        // mine, based on ESP gateway response possibilites.
        //enum class gateway_status {
        //    UNKNOWN,            // other event not in list and not WEBSOCKET_EVENT_DATA (common data work)
        //    RECONNECT,          // Had it running, Discord called RECONNECT
        //    STARTUP,            // Started thread right now, first step.
        //    CONNECTED,          // WEBSOCKET_EVENT_CONNECTED
        //    ERROR,              // WEBSOCKET_EVENT_ERROR
        //    DISCONNECTED,       // WEBSOCKET_EVENT_DISCONNECTED
        //    CLOSED              // WEBSOCKET_EVENT_CLOSED
        //};

        enum class gateway_status_binary : uint16_t {
            NONE                = 0,
            ANY_NEED_RESTART    = (1 << 0),     // From any source, if this is triggered, things should restart

            DC_HEARTBEAT_AWAKE  = (1 << 1),     // Can start sending heartbeats
            DC_NEED_IDENTIFY    = (1 << 2),     // Send IDENTIFY (new connection or Discord asked for reconnect)
            DC_NEED_HEARTBEAT   = (1 << 3),     // Tell task that heatbeat was needed!
            //DC_NEED_RECONNECT   = (1 << 3),   // Discord asked for reconnect
            //DC_HEARTBEAT_NO_ACK = (1 << 5),   // No ack after so much time. Also causes ANY_NEED_RESTART

            GW_CONNECTED        = (1 << 10),    // Websocket connected to server
            GW_CLOSED           = (1 << 11)     // Websocket closed cleanly
            //GW_ERROR_NEED_HELP  = (1 << 12)   // Websocket got error. Also causes ANY_NEED_RESTART
        };        
        gateway_status_binary operator+=(gateway_status_binary&, const gateway_status_binary&);
        gateway_status_binary operator-=(gateway_status_binary&, const gateway_status_binary&);
        gateway_status_binary operator+(const gateway_status_binary&, const gateway_status_binary&);
        gateway_status_binary operator-(const gateway_status_binary&, const gateway_status_binary&);
        gateway_status_binary operator&(const gateway_status_binary&, const gateway_status_binary&);

        // as JSON need memory ref, this will keep them both allocated until destroyed.
        // constructor generates JSON automatically and destroys them at delete
        struct gateway_event_memory_block;

        struct gateway_payload_structure {
            //char* d = nullptr;          // payload
            File* d_mx = nullptr; // new payload way
            const size_t d_len = 0;       // total payload length
            //gateway_opcodes op;         // j["op"]
            //int64_t s = -1;             // j["s"]
            //gateway_events t;           // j["t"]

            gateway_event_memory_block* last_event_block = nullptr;
            //JSON* j = nullptr;          // json, parsed when append() is completed. format: { op: opcode_int, d: {...}, s: sequence_number_int, t: event_name_string }


            gateway_payload_structure(const gateway_payload_structure&) = delete;
            gateway_payload_structure(gateway_payload_structure&&) = delete;
            void operator=(const gateway_payload_structure&) = delete;
            void operator=(gateway_payload_structure&&) = delete;

            // get payload length and allocate memory for receiving it.
            gateway_payload_structure(const size_t);
            ~gateway_payload_structure();

            // append <data> with <length> size at <offset>. Automatic parse if (offset + length == this->d_len). return 1 on end, 0 on non error or -1 if memory is null
            int append(const char*, size_t, size_t);
            // free all memory
            void free();
        };


        class Gateway {
        public:
	        typedef void(*event_handler)(const gateway_events&, const JSON&);
        private:
            // === // USER SET DATA // ==================================================================
            gateway_intents m_intents;      // from constructor
            HeapString m_token;             // from constructor
            event_handler m_event_handler;  // from constructor
            HeapString m_gateway_cert_perm;

            // === // DATA OF BOT // ==================================================================
            HeapString m_session_id;
            HeapString m_bot_string;
            uint64_t m_bot_id = 0;
            
            // === // HEARTBEAT RELATED // ==================================================================
            uint32_t m_heartbeat_interval_ms = 0;               // if no heartbeat ack after this, reconnect.
            int64_t m_last_sequence_number = -1;                // if last_sequence_number_set == false, consider this null.
            bool m_heartbeat_got_confirm = true;                // on heartbeat sent, set false. If next heartbeat this is false, reconnect.
            
            // === // WORKING DATA // ==================================================================
            esp_websocket_client_handle_t m_client_handle = nullptr;        // SETUP DONE (partially)
            gateway_payload_structure* m_pay_work = nullptr;                // BEING USED
            EventHandler m_event_loop;                                      // NOT USED YET
            gateway_status_binary m_stats = gateway_status_binary::NONE;    // Partially used
            uint64_t m_client_last_send = 0;
            TaskHandle_t m_gateway_send_task = nullptr;
            //gateway_status m_status = gateway_status::UNKNOWN;

            bool send_raw_json(const char*, const size_t);

            void summon_gateway_send_task();
            void destroy_gateway_send_task();

            void start_gateway();
            void close_gateway();
            void full_restart_cleanup(); // close and start

            void handle_secondary_tasks(); // ASYNC
            void handle_gateway_events(int32_t event_id, void* event_data); // ASYNC

        public:
            // token, gateway
            Gateway(const char* token, const gateway_intents, const event_handler);
            ~Gateway();

            void stop();
            void start();
        };

        struct gateway_event_memory_block {
            //char* ref;
            const JSON* j;
            const JSON d;
            gateway_events t;           // j["t"] aka ev_id
            int64_t s = -1;             // j["s"]
            gateway_opcodes op;         // j["op"]
            Gateway::event_handler func;

            gateway_event_memory_block(FileJSON*&&);
            ~gateway_event_memory_block();
        };
    }
}