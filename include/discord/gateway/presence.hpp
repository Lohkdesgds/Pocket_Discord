#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/emoji.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char PresenceTAG[] = "Presence";

        // based on Discord @ https://discord.com/developers/docs/topics/gateway#activity-object-activity-types
        enum class activity_types {
            GAME,       // Playing {name}
            STREAMING,  // Streaming {details}
            LISTENING,  // Listening to {name}
            WATCHING,   // Watching {name}
            CUSTOM,     // {emoji} {name}
            COMPETING   // Competing in {name}
        };

        enum class status_types {
            ONLINE,
            DO_NOT_DISTURB,
            IDLE,
            INVISIBLE,
            OFFLINE // not used for self status
        };

        // maybe add more later @ https://discord.com/developers/docs/topics/gateway#activity-object-activity-structure
        struct activity {
            std::string name;
            activity_types type; // cast to int
            std::string url; // valid if STREAMING
            std::string details; // what is being done rn (opt)
            Emoji emoji; // valid if CUSTOM

            std::string to_json() const;
        };

        status_types string_to_status_types(const std::string&);
        std::string status_types_to_string(const status_types&);
        
        // based on Discord @ https://discord.com/developers/docs/topics/gateway#update-presence-gateway-presence-update-structure
        class GatewayPresence {
            BotCore& core;

            unsigned long long since = 0; // since when is this with this status? (in millisec)
            activity activities; // one is good enough.
            std::string status;
            bool afk = false;

            std::string to_json() const;
        public:
            GatewayPresence(BotCore&);
            
            // applpy to bot's presence
            bool apply();

            void set_status(const status_types&);
            void set_afk(const bool&);

            // type, name, details, url
            void set_activity(const activity_types&, const std::string&, const std::string& = "", const std::string& = "");
            void set_activity_emoji(const Emoji&);

        };
    }
}