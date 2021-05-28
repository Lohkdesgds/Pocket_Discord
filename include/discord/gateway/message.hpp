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
#include "discord/gateway/user.hpp"
#include "discord/gateway/attachment.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char MessageTAG[] = "Message";
        
        // based on Discord @ https://discord.com/developers/docs/resources/channel#message-object
        class Message {
            BotCore& core;

            unsigned long long id = 0;
            unsigned long long channel_id = 0;
            unsigned long long guild_id = 0; // optional
            Member member; // author is used here.
            std::string content;
            std::string timestamp;
            std::string edited_timestamp; // optional
            bool mentions_everyone = false;
            std::vector<unsigned long long> mentions; // (user) -> id
            std::vector<unsigned long long> mention_roles; // direct
            std::vector<unsigned long long> mention_channels; // (channel mention) -> id
            std::vector<Attachment> attachments;

            //std::vector<std::pair<unsigned long long, std::string>> mentions; // optional, only ID & name so no giant memory usage
            //std::vector<std::pair<unsigned long long, std::string>> mention_roles;// optional, only ID & name so no giant memory usage
        public:
            Message(BotCore&);
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            unsigned long long get_channel_id() const;
            unsigned long long get_guild_id() const;
            const User& get_author() const;
            const Member& get_member() const;
            const std::string& get_content() const;
            const std::string& get_timestamp() const;
            const std::string& get_edited_timestamp() const;
            bool get_mentions_everyone() const;
            const std::vector<unsigned long long>& get_mentions() const;
            bool get_mentions_one(const unsigned long long&);
            const std::vector<unsigned long long>& get_mention_roles() const;
            bool get_mention_roles_one(const unsigned long long&);
            const std::vector<unsigned long long>& get_mention_channels() const;
            bool get_mention_channels_one(const unsigned long long&);
            const std::vector<Attachment>& get_attachments() const;

            // based on Discord message related calls

            // deletes THIS message:
            std::future<request_response> delete_message();
            // create a answer to THIS message: (can embed)
            std::future<request_response> create_message_answer(const std::string&, const std::string& = "");
            // create a message in this same channel (can embed)
            std::future<request_response> create_message_same_channel(const std::string&, const std::string& = "");
            
            // to add.
            /*// create a reaction to the message (what emoji)
            std::future<request_response> create_reaction(const std::string&);
            // delete own reaction (what emoji)
            std::future<request_response> delete_own_reaction(const std::string&);
            // delete someone's reaction (who, what emoji)
            std::future<request_response> delete_user_reaction(const unsigned long long&, const std::string&);
            // get users that reacted this (opt: after what user id, opt: max reactions list size)
            std::future<request_response> get_reactions(const unsigned long long& = 0, const int& = 25);
            // remove all reactions in message
            std::future<request_response> delete_all_reactions();
            // delete all reactions of that emoji. (emoji)
            std::future<request_response> delete_all_reactions_for_emoji(const std::string&);
            // edit message with this
            std::future<request_response> edit_message(const std::string&, const std::string& = "");*/
        };
    }
}