#include "discord/gateway/channel.hpp"

namespace LSW {
    namespace PocketDiscord {

        Channel::Channel(BotCore& othcore)
            : core(othcore)
        {
        } 
        
        bool Channel::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(ChannelTAG, "Message::load_from_json parsing failed! JSON was null!");
                return false;
            }

            if (/*MemoryFileJSON*/ const auto _val = json["id"]; !_val.is_null())                  id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["type"]; !_val.is_null())                type = _val.as_int();
            if (/*MemoryFileJSON*/ const auto _val = json["guild_id"]; !_val.is_null())            guild_id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["position"]; !_val.is_null())            position = _val.as_int();
            if (/*MemoryFileJSON*/ const auto _val = json["name"]; !_val.is_null())                name = handle_utf16(_val.as_string());
            if (/*MemoryFileJSON*/ const auto _val = json["topic"]; !_val.is_null())               topic = handle_utf16(_val.as_string());
            if (/*MemoryFileJSON*/ const auto _val = json["icon"]; !_val.is_null())                icon = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["nsfw"]; !_val.is_null())                nsfw = _val.as_bool();
            if (/*MemoryFileJSON*/ const auto _val = json["last_message_id"]; !_val.is_null())     last_message_id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["bitrate"]; !_val.is_null())             bitrate = _val.as_int();
            if (/*MemoryFileJSON*/ const auto _val = json["user_limit"]; !_val.is_null())          user_limit = _val.as_int();
            if (/*MemoryFileJSON*/ const auto _val = json["rate_limit_per_user"]; !_val.is_null()) rate_limit_per_user = _val.as_int();
            if (/*MemoryFileJSON*/ const auto _val = json["parent_id"]; !_val.is_null())           parent_id = _val.as_llu();

            return id != 0;
        }

        unsigned long long Channel::get_id() const
        {
            return id;
        }

        int Channel::get_type() const
        {
            return type;
        }
        
        unsigned long long Channel::get_guild_id() const
        {
            return guild_id;
        }
        
        int Channel::get_position() const
        {
            return position;
        }
        
        const std::string& Channel::get_name() const
        {
            return name;
        }
        
        const std::string& Channel::get_topic() const
        {
            return topic;
        }
        
        bool Channel::get_nsfw() const
        {
            return nsfw;
        }
        
        unsigned long long Channel::get_last_message_id() const
        {
            return last_message_id;
        }
        
        int Channel::get_bitrate() const
        {
            return bitrate;
        }
        
        int Channel::get_user_limit() const
        {
            return user_limit;
        }
        
        int Channel::get_rate_limit_per_user() const
        {
            return rate_limit_per_user;
        }
        
        unsigned long long Channel::get_parent_id() const
        {
            return parent_id;
        }
        
        
        bool Channel::load_channel(const unsigned long long& nid, const bool fullload)
        {
            if (nid == 0) return false;

            id = nid;

            if (!fullload){
                return id != 0;
            }

            const std::string request = "/channels/" + std::to_string(id);
            const http_request method = http_request::GET;
            const std::string json = "";

            std::future<request_response> req = core.post_task(request, method, json);

            req.wait();

            request_response res = req.get();

            return load_from_json(res.mj) && id != 0 && !name.empty();
        }
        
        std::future<request_response> Channel::delete_message(const unsigned long long mid)
        {
            if (!mid || !id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(id) + "/messages/" + std::to_string(mid);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Channel::create_message(const std::string& str)
        {
            if (!id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\""
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Channel::create_message(const std::string& str, const Embed& embed)
        {
            if (!id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\","
                "\"embed\":[" + embed.to_json() + "]"
            "}";

            return core.post_task(request, method, json);
        }

    }
}
