#include "discord/gateway/reaction.hpp"

namespace LSW {
    namespace PocketDiscord {

        Reaction::Reaction(BotCore& othcore)
            : core(othcore), member(othcore)
        {
        }
        
        Reaction::Reaction(const Reaction& oth)
            : core(oth.core), user_id(oth.user_id), channel_id(oth.channel_id), message_id(oth.message_id), guild_id(oth.guild_id), member(oth.member), emoji(oth.emoji)
        {
        }
        
        bool Reaction::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(ReactionTAG, "Reaction::load_from_json parsing failed! JSON was null!");
                return false;
            }

            bool good = true;

            if (/*MemoryFileJSON*/ const auto it = json["user_id"]; !it.is_null())         user_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["channel_id"]; !it.is_null())      channel_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["message_id"]; !it.is_null())      message_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["guild_id"]; !it.is_null())        guild_id = it.as_llu();

            if (/*MemoryFileJSON*/ const auto it = json["member"]; !it.is_null()) {
                if (!member.load_from_json(it)){
                    ESP_LOGW(ReactionTAG, "Reaction::load_from_json parsing failed! Member failed somewhere while parsing.");
                }
            }

            if (user_id != 0) member.get_user_ref().set_id(user_id); // be sure

            if (/*MemoryFileJSON*/ const auto it = json["emoji"]; !it.is_null()) {
                if (!emoji.load_from_json(it)){
                    ESP_LOGW(ReactionTAG, "Reaction::load_from_json parsing failed! Emoji failed somewhere while parsing.");
                    good = false;
                }
            }
            else {
                ESP_LOGW(ReactionTAG, "Reaction::load_from_json parsing failed! Emoji was null.");
                good = false;
            }

            return good && user_id != 0;
        }

        unsigned long long Reaction::get_user_id() const
        {
            return user_id;
        }

        unsigned long long Reaction::get_channel_id() const
        {
            return channel_id;
        }
        
        unsigned long long Reaction::get_message_id() const
        {
            return message_id;
        }
        
        unsigned long long Reaction::get_guild_id() const
        {
            return guild_id;
        }
        
        const Member& Reaction::get_member() const
        {
            return member;
        }
        
        const Emoji Reaction::get_emoji() const
        {
            return emoji;
        }

        void Reaction::set_user_id(const unsigned long long& arg)
        {
            user_id = arg;
        }

        void Reaction::set_channel_id(const unsigned long long& arg)
        {
            channel_id = arg;
        }
        
        void Reaction::set_message_id(const unsigned long long& arg)
        {
            message_id = arg;
        }
        
        void Reaction::set_guild_id(const unsigned long long& arg)
        {
            guild_id = arg;
        }
        
        Member& Reaction::get_member_ref()
        {
            return member;
        }
        
        Emoji& Reaction::get_emoji_ref()
        {
            return emoji;
        }
        
        
    
        // reacts the same emoji in this message
        std::future<request_response> Reaction::react_on_top()
        {
            if (emoji.empty() || !channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + emoji.format_react() + "/@me";
            const http_request method = http_request::PUT;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // react a emoji in this message
        std::future<request_response> Reaction::react_on_same_message(const Emoji& arg)
        {
            if (arg.empty() || !channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + arg.format_react() + "/@me";
            const http_request method = http_request::PUT;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // remove this reaction (needs permission)
        std::future<request_response> Reaction::delete_reaction()
        {
            if (emoji.empty() || !channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + emoji.format_react() + "/@me";
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // remove someone's reaction (of this emoji)
        std::future<request_response> Reaction::delete_reaction_of_user(const unsigned long long& userid)
        {
            if (emoji.empty() || !channel_id || !message_id || !userid) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + emoji.format_react() + "/" + std::to_string(userid);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // remove all reactions of this message
        std::future<request_response> Reaction::delete_all_reactions()
        {
            if (!channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions";
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // remove all reactions with this emoji
        std::future<request_response> Reaction::delete_this_emoji_reactions()
        {
            if (emoji.empty() || !channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + emoji.format_react();
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // remove all reactions of a certain emoji in this message
        std::future<request_response> Reaction::delete_all_reactions_of_emoji(const Emoji& arg)
        {
            if (arg.empty() || !channel_id || !message_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(message_id) + "/reactions/" + arg.format_react();
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }        

    }
}