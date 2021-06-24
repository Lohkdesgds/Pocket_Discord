#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/member.hpp"
#include "discord/gateway/emoji.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char ReactionTAG[] = "Reaction";

        // based on Discord @ https://discord.com/developers/docs/topics/gateway#message-reaction-add-message-reaction-add-event-fields (others are this but smaller)
        class Reaction {
            BotCore& core;

            unsigned long long user_id = 0; // not in remove all or remove emoji
            unsigned long long channel_id = 0;
            unsigned long long message_id = 0;
            unsigned long long guild_id = 0;
            Member member; // only on created reactions
            Emoji emoji; // partial emoji, yes
        public:
            Reaction(BotCore&);
            Reaction(const Reaction&);

            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_user_id() const;
            unsigned long long get_channel_id() const;
            unsigned long long get_message_id() const;
            unsigned long long get_guild_id() const;
            const Member& get_member() const;
            const Emoji get_emoji() const;

            void set_user_id(const unsigned long long&);
            void set_channel_id(const unsigned long long&);
            void set_message_id(const unsigned long long&);
            void set_guild_id(const unsigned long long&);
            Member& get_member_ref();
            Emoji& get_emoji_ref();
            
            // based on Discord message related calls

            // reacts the same emoji in this message
            std::future<request_response> react_on_top();
            // react a emoji in this message
            std::future<request_response> react_on_same_message(const Emoji&);
            // remove this reaction (needs permission)
            std::future<request_response> delete_reaction();
            // remove someone's reaction (of this emoji)
            std::future<request_response> delete_reaction_of_user(const unsigned long long&);
            // remove all reactions of this message
            std::future<request_response> delete_all_reactions();
            // remove all reactions with this emoji
            std::future<request_response> delete_this_emoji_reactions();
            // remove all reactions of a certain emoji in this message
            std::future<request_response> delete_all_reactions_of_emoji(const Emoji&);
        };
    }
}