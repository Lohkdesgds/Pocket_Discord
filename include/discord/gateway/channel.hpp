#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/embed.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char ChannelTAG[] = "Channel";

        // based on Discord @ https://discord.com/developers/docs/resources/channel#channel-object-channel-structure
        class Channel {
            BotCore& core;

            unsigned long long id = 0;
            int type = 0;
            unsigned long long guild_id = 0;
            int position = 0;
            std::string name;
            std::string topic;
            std::string icon;
            bool nsfw = false;
            unsigned long long last_message_id = 0;
            int bitrate = 0;
            int user_limit = 0;
            int rate_limit_per_user = 0; // channel delay
            unsigned long long parent_id = 0;
        public:
            Channel(BotCore&);
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            int get_type() const;
            unsigned long long get_guild_id() const;
            int get_position() const;
            const std::string& get_name() const;
            const std::string& get_topic() const;
            bool get_nsfw() const;
            unsigned long long get_last_message_id() const;
            int get_bitrate() const;
            int get_user_limit() const;
            int get_rate_limit_per_user() const;
            unsigned long long get_parent_id() const;

            // based on Discord message related calls
            
            // tries to get information from HTTP of this channel ID (channel id, opt: full load (true, download) or just set (false, incomplete)?)
            bool load_channel(const unsigned long long&, const bool = true);
            // modify channel (new name, opt: new icon base64 (?))
            //std::future<request_response> modify_channel(const std::string&, const std::string& = "");
            // add more in the future


            // deletes a message:
            std::future<request_response> delete_message(const unsigned long long);
            // create a answer to THIS message:
            std::future<request_response> create_message(const std::string&);
            // create a answer to THIS message: (with embed)
            std::future<request_response> create_message(const std::string&, const Embed&);
        };
    }
}
