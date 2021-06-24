#include "discord/gateway/role.hpp"

namespace LSW {
    namespace PocketDiscord {

        bool Role::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(RoleTAG, "Role::load_from_json parsing failed! JSON was null!");
                return false;
            }

            if (/*MemoryFileJSON*/ const auto it = json["id"]; !it.is_null())          id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["name"]; !it.is_null())        name = handle_utf16(it.as_string());
            if (/*MemoryFileJSON*/ const auto it = json["color"]; !it.is_null())       color = it.as_int();
            if (/*MemoryFileJSON*/ const auto it = json["position"]; !it.is_null())    position = it.as_int();
            if (/*MemoryFileJSON*/ const auto it = json["permissions"]; !it.is_null()) permissions = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["managed"]; !it.is_null())     managed = it.as_bool();

            return id != 0;
        }

        unsigned long long Role::get_id() const
        {
            return id;
        }

        const std::string& Role::get_name() const
        {
            return name;
        }

        int Role::get_color() const
        {
            return color;
        }

        int Role::get_position() const
        {
            return position;
        }

        const std::string& Role::get_permissions() const
        {
            return permissions;
        }

        bool Role::get_managed() const
        {
            return managed;
        }

    }
}