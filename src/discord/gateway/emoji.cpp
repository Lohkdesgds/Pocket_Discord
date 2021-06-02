#include "discord/gateway/emoji.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        std::string Emoji::to_json() const
        {
            if (empty()) {
                ESP_LOGE(EmojiTAG, "Emoji::to_json had invalid configuration!");
                return "";
            }

            return "{"
                "\"id\":" + (id ? ("\"" + std::to_string(id) + "\"") : ("null")) + ","
                "\"name\":" + (name.empty() ? ("null") : ("\"" + name + "\"")) + 
                (animated ? ",\"animated\":true" : "") +
            "}";
        }

        std::string Emoji::format_text() const
        {
            return id ? ("<" + std::string(animated ? "a" : "") + ":" + name + ":" + std::to_string(id) + ">") : name;
        }

        std::string Emoji::format_react() const
        {
            return id ? (name + ":" + std::to_string(id)) : name;
        }
        
        bool Emoji::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(EmojiTAG, "Emoji::load_from_json parsing failed! JSON was null!");
                return false;
            }
            
            if (/*MemoryFileJSON*/ const auto _val = json["id"]; !_val.is_null())               id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["name"]; !_val.is_null())             name = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["animated"]; !_val.is_null())         animated = _val.as_bool();

            return id != 0 || !name.empty();
        }

        bool Emoji::empty() const
        {
            return id == 0 && name.empty();
        }

        void Emoji::set_name(const std::string& arg)
        {
            name = arg;
        }

        void Emoji::set_id(const unsigned long long& arg)
        {
            id = arg;
        }
        
        void Emoji::set_animated(const bool& arg)
        {
            animated = arg;
        }

        const std::string& Emoji::get_name() const
        {
            return name;
        }

        unsigned long long Emoji::get_id() const
        {
            return id;
        }
        
        bool Emoji::get_animated() const
        {
            return animated;
        }
        
    }
}