#include "discord/gateway/guild.hpp"

namespace LSW {
    namespace PocketDiscord {

        bool Guild::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(GuildTAG, "Guild::load_from_json parsing failed! JSON was null!");
                return false;
            }
            
            if (/*MemoryFileJSON*/ const auto _val = json["id"]; !_val.is_null())               id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["name"]; !_val.is_null())             name = handle_utf16(_val.as_string());
            if (/*MemoryFileJSON*/ const auto _val = json["icon"]; !_val.is_null())             icon = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["icon_hash"]; !_val.is_null())        icon_hash = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["owner_id"]; !_val.is_null())         owner_id = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["permissions"]; !_val.is_null())      permissions = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["joined_at"]; !_val.is_null())        joined_at = _val.as_string();
            if (/*MemoryFileJSON*/ const auto _val = json["large"]; !_val.is_null())            large = _val.as_bool();
            if (/*MemoryFileJSON*/ const auto _val = json["unavailable"]; !_val.is_null())      unavailable = _val.as_bool();
            if (/*MemoryFileJSON*/ const auto _val = json["member_count"]; !_val.is_null())     member_count = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["max_members"]; !_val.is_null())      max_members = _val.as_llu();
            if (/*MemoryFileJSON*/ const auto _val = json["nsfw"]; !_val.is_null())             nsfw = _val.as_bool();

            if (/*MemoryFileJSON*/ const auto it = json["members"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    
                    Member member(core);
                    if (member.load_from_json(inn.as_json())) {
                        members.push_back(std::move(member));
                        continue;
                    }
                    
                    ESP_LOGW(GuildTAG, "Guild::load_from_json parsing failed! Member was not cool! Skipping.");
                }
            }
            
            if (/*MemoryFileJSON*/ const auto it = json["channels"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    
                    Channel channel(core);
                    if (channel.load_from_json(inn.as_json())) {
                        channels.push_back(std::move(channel));
                        continue;
                    }

                    ESP_LOGW(GuildTAG, "Guild::load_from_json parsing failed! Channel was not cool! Skipping.");
                }
            }

            return id != 0 && owner_id != 0;
        }

    }
}