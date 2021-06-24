#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char RoleTAG[] = "Role";
        
        // based on Discord @ https://discord.com/developers/docs/topics/permissions#role-object-role-structure
        class Role {
            unsigned long long id = 0;
            std::string name;
            int color = 0;
            int position = 0;
            std::string permissions;
            bool managed = false;
        public:
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            const std::string& get_name() const;
            int get_color() const;
            int get_position() const;
            const std::string& get_permissions() const;
            bool get_managed() const;
        };

    }
}