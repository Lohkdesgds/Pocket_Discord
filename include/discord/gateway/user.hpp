#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        inline const char UserTAG[] = "User";
        
        // based on Discord @ https://discord.com/developers/docs/resources/user#user-object-user-structure
        class User {
            unsigned long long id = 0;
            std::string username;
            std::string discriminator;
            std::string avatar; // may be null (empty)
            bool bot = false; // may not be in JSON
        public:
            User() = default;
            User(const User&);
            
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            const std::string& get_username() const;
            const std::string& get_discriminator() const;
            const std::string& get_avatar() const;
            bool get_bot() const;

            void set_id(const unsigned long long&);
            void set_username(const std::string&);
            void set_discriminator(const std::string&);
            void set_avatar(const std::string&);
            void set_bot(const bool&);

            std::string format_mention() const;
            std::string format_full_name() const; // username#discriminator
            std::string format_avatar_url(const size_t = 256) const; // size, 2^n only
        };
    }
}