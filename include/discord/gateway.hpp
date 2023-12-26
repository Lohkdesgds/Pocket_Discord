#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_websocket_client.h"
#include "esp_transport_ws.h"

#include "../eventhandler.h"


namespace Lunaris {
    namespace PocketDiscord {

        const char gateway_url[] = "wss://gateway.discord.gg/?v=10&encoding=json";
        constexpr int gateway_max_timeout = 5000;
        constexpr size_t gateway_stack_size = 6 * 1024;
        constexpr size_t gateway_poll_stack_size = 16 * 1024;
        constexpr size_t gateway_buffer_size = 4096;
        constexpr unsigned gateway_self_priority = 1;
        constexpr unsigned gateway_queue_size_default = 30;
        constexpr unsigned gateway_poll_event_priority = 50;
        
        const char app_version[] = "V2.0.0 ALPHA";
        const char target_app[] = "ESP32";
        
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
            HEARTBEAT_ACK           = 11,       // [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
            _MAX = HEARTBEAT_ACK                // for reference
        };
        static gateway_opcodes str2gateway_opcode(const char*);

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
        static gateway_close_event_codes str2gateway_close_event_codes(const char*);

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
        static gateway_send_events str2gateway_send_events(const char*);

        
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
        static gateway_events str2gateway_events(const char*);

    }
}