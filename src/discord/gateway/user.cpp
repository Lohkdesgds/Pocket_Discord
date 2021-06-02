#include "discord/gateway/user.hpp"

namespace LSW {
    namespace PocketDiscord {

        bool User::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(UserTAG, "User::load_from_json parsing failed! JSON was null!");
                return false;
            }

            if (/*MemoryFileJSON*/ const auto it = json["id"]; !it.is_null())              id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["username"]; !it.is_null())        username = handle_utf16(it.as_string());
            if (/*MemoryFileJSON*/ const auto it = json["discriminator"]; !it.is_null())   discriminator = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["avatar"]; !it.is_null())          avatar = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["bot"]; !it.is_null())             bot = it.as_bool();

            return id != 0;
        }
        
        unsigned long long User::get_id() const
        {
            return id;
        }

        const std::string& User::get_username() const
        {
            return username;
        }

        const std::string& User::get_discriminator() const
        {
            return discriminator;
        }

        const std::string& User::get_avatar() const
        {
            return avatar;
        }

        bool User::get_bot() const
        {
            return bot;
        }

        void User::set_id(const unsigned long long& arg)
        {
            id = arg;
        }

        void User::set_username(const std::string& arg)
        {
            username = arg;
        }
        
        void User::set_discriminator(const std::string& arg)
        {
            discriminator = arg;
        }
        
        void User::set_avatar(const std::string& arg)
        {
            avatar = arg;
        }
        
        void User::set_bot(const bool& arg)
        {
            bot = arg;
        }
        std::string User::format_mention() const
        {
            return id ? "<@" + std::to_string(id) + ">" : "";
        }

    }
}