#include "discord/gateway/emoji.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        std::string Emoji::encoded_name() const
        {
            std::ostringstream escaped;
            escaped.fill('0');
            escaped << std::hex;

            for (std::string::value_type c : name)
            {
                // Keep alphanumeric and other accepted characters intact
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                {
                    escaped << c;
                    continue;
                }

                // Any other characters are percent-encoded
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int((unsigned char)c);
                escaped << std::nouppercase;
            }

            return escaped.str();
        }

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
            return id ? (name + ":" + std::to_string(id)) : encoded_name();
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

        Emoji& Emoji::set_name(const std::string& arg)
        {
            name = arg;
            return *this;
        }

        Emoji& Emoji::set_id(const unsigned long long& arg)
        {
            id = arg;
            return *this;
        }
        
        Emoji& Emoji::set_animated(const bool& arg)
        {
            animated = arg;
            return *this;
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