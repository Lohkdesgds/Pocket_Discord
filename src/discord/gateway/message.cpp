#include "discord/gateway/message.hpp"

namespace LSW {
    namespace PocketDiscord {

        Message::Message(BotCore& othcore)
            : core(othcore), member(othcore)
        {
        }

        bool Message::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(MessageTAG, "Message::load_from_json parsing failed! JSON was null!");
                return false;
            }
            
            if (/*MemoryFileJSON*/ const auto it = json["id"]; !it.is_null())                  id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["channel_id"]; !it.is_null())          channel_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["guild_id"]; !it.is_null())            guild_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["content"]; !it.is_null())             content = handle_utf16(it.as_string());
            if (/*MemoryFileJSON*/ const auto it = json["timestamp"]; !it.is_null())           timestamp = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["edited_timestamp"]; !it.is_null())    edited_timestamp = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["mentions_everyone"]; !it.is_null())   mentions_everyone = it.as_bool();

            if (/*MemoryFileJSON*/ const auto it = json["member"]; !it.is_null()) {
                if (!member.load_from_json(it, true)){
                    ESP_LOGW(MessageTAG, "Message::load_from_json parsing failed! Member failed somewhere while parsing.");
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["author"]; !it.is_null()) {
                if (!member.get_user_ref().load_from_json(it)) {
                    ESP_LOGW(MessageTAG, "Message::load_from_json parsing failed! Author failed somewhere while parsing.");
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["mentions"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    unsigned long long plusid = inn["id"].as_llu();
                    if (!plusid) continue;
                    mentions.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["mention_roles"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    unsigned long long plusid = inn.as_llu();
                    if (!plusid) continue;
                    mention_roles.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["mention_channels"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    unsigned long long plusid = inn["id"].as_llu();
                    if (!plusid) continue;
                    mention_channels.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["attachments"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    
                    Attachment newattach;
                    if (newattach.load_from_json(inn.as_json())){
                        attachments.push_back(std::move(newattach));
                        continue;
                    }

                    ESP_LOGW(MessageTAG, "Message::load_from_json parsing failed! Attachment parsing had issues parsing! Skipping.");
                }
            }
            
            return id != 0;
        }
        
        unsigned long long Message::get_id() const
        {
            return id;
        }

        unsigned long long Message::get_channel_id() const
        {
            return channel_id;
        }
        
        unsigned long long Message::get_guild_id() const
        {
            return guild_id;
        }
        
        const User& Message::get_author() const
        {
            return member.get_user();
        }
        
        const Member& Message::get_member() const
        {
            return member;
        }
        
        const std::string& Message::get_content() const
        {
            return content;
        }
        
        const std::string& Message::get_timestamp() const
        {
            return timestamp;
        }
        
        const std::string& Message::get_edited_timestamp() const
        {
            return edited_timestamp;
        }

        bool Message::get_mentions_everyone() const
        {
            return mentions_everyone;
        }

        const std::vector<unsigned long long>& Message::get_mentions() const
        {
            return mentions;
        }

        bool Message::get_mentions_one(const unsigned long long& id)
        {
            return std::find(mentions.begin(), mentions.end(), id) != mentions.end();
        }

        const std::vector<unsigned long long>& Message::get_mention_roles() const
        {
            return mention_roles;
        }

        bool Message::get_mention_roles_one(const unsigned long long& id)
        {
            return std::find(mention_roles.begin(), mention_roles.end(), id) != mention_roles.end();
        }

        const std::vector<unsigned long long>& Message::get_mention_channels() const
        {
            return mention_channels;
        }

        bool Message::get_mention_channels_one(const unsigned long long& id)
        {
            return std::find(mention_channels.begin(), mention_channels.end(), id) != mention_channels.end();
        }

        const std::vector<Attachment>& Message::get_attachments() const
        {
            return attachments;
        }
        
        std::future<request_response> Message::delete_message()
        {
            if (!id || !channel_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(id);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_answer(const std::string& str, const std::string& embed)
        {
            if (!id || !channel_id || !guild_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\"," + // NOTE: , exists because more items all the time
                std::string(embed.empty() ? "" : ("\"embed\":" + embed + ",")) +  // NOTE: , because there will be a item later for sure
                "\"message_reference\":{"
                    "\"message_id\":\"" + std::to_string(id) + "\"" // if uncomment, put ','! (don't forget)
                    /*"\"channel_id\":\"" + std::to_string(channel_id) + "\","
                    "\"guild_id\":\"" + std::to_string(guild_id) + "\","
                    "\"fail_if_not_exists\":false" // maybe change later*/
                "}"
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_same_channel(const std::string& str, const std::string& embed)
        {
            if (!channel_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\"" + // NOTE: this doesn't have , by default, it's added on embed
                std::string(embed.empty() ? "" : (",\"embed\":" + embed)) + // NOTE: embed doesn't have , because it's last
            "}";

            return core.post_task(request, method, json);
        }
    }
}