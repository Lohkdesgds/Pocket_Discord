#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/member.hpp"
#include "discord/gateway/channel.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char GuildTAG[] = "Guild";
        
        // based on Discord @ https://discord.com/developers/docs/resources/guild#guild-object
        class Guild {
            BotCore& core;

            unsigned long long id = 0;
            std::string name;
            std::string icon; // can be empty
            std::string icon_hash; // can be empty
            unsigned long long owner_id = 0;
            std::string permissions;
            std::string joined_at; // only on GUILD_CREATE
            bool large = false; // only on GUILD_CREATE
            bool unavailable = false; // outage?
            unsigned long long member_count = 0;
            std::vector<Member> members;
            std::vector<Channel> channels;
            // threads are not useful right now.
            unsigned long long max_members = 0; // total max members allowed in guild?!
            bool nsfw = false; // true if guild is set as nsfw by Discord
        public:
            Guild(BotCore&);

            bool load_from_json(const MemoryFileJSON&);
        };

    }
}