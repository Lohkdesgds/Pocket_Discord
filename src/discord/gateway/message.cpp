#include "discord/gateway/message.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        void GenericComponentObject::set_type(const component_type& t)
        {
            type = t;
        }
        
        GenericComponentObject::GenericComponentObject(const component_type& t)
        {
            type = t;
        }
        
        component_type GenericComponentObject::get_type() const
        {
            return type;
        }

        ButtonObject::ButtonObject()
            : GenericComponentObject(component_type::BUTTON)
        {
        }

        ButtonObject::ButtonObject(const ButtonObject& oth)
            : GenericComponentObject(component_type::BUTTON)
        {
            *this = oth;
        }

        ButtonObject::ButtonObject(ButtonObject&& oth)
            : GenericComponentObject(component_type::BUTTON)
        {
            *this = std::move(oth);
        }

        void ButtonObject::operator=(const ButtonObject& oth)
        {
            style  = oth.style;
            label = oth.label;
            emoji = oth.emoji;
            custom_id = oth.custom_id;
            url = oth.url;
            disabled = oth.disabled;
        }

        void ButtonObject::operator=(ButtonObject&& oth)
        {
            style  = oth.style;
            label = std::move(oth.label);
            emoji = std::move(oth.emoji);
            custom_id = std::move(oth.custom_id);
            url = std::move(oth.url);
            disabled = oth.disabled;
        }

        bool ButtonObject::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                logg << L::SL << Color::RED << "[ButtonObject] Failed to load JSON, it was null!" << L::EL;
                return false;
            }


            //if (const auto it = json["type"]; !it.is_null())             type = static_cast<component_type>(it.as_int()); // this is a button only
            if (const auto it = json["style"]; !it.is_null())            style = static_cast<button_style>(it.as_int());
            if (const auto it = json["label"]; !it.is_null())            label = it.as_string();
            if (const auto it = json["custom_id"]; !it.is_null())        custom_id = it.as_string();
            if (const auto it = json["url"]; !it.is_null())              url = it.as_string();
            if (const auto it = json["disabled"]; !it.is_null())         disabled = it.as_bool();

            if (const auto it = json["emoji"]; !it.is_null()) {
                if (!emoji.load_from_json(it)){
                    logg << L::SL << Color::YELLOW << "[ButtonObject] Failed to load JSON, parsing failed!" << L::EL;
                }
            }

            return !url.empty() || !custom_id.empty();
        }

        std::string ButtonObject::to_json() const
        {
            std::string fp;

            fp = "{";
            fp += "\"type\":" + std::to_string(static_cast<int>(get_type())) + ",";
            fp += "\"style\":" + std::to_string(static_cast<int>(style)) + ",";
            if (!label.empty())                 fp += "\"label\":\"" + label + "\",";
            if (!custom_id.empty())             fp += "\"custom_id\":\"" + custom_id + "\",";
            if (!url.empty())                   fp += "\"url\":\"" + url + "\",";
            fp += "\"disabled\":" + std::string(disabled ? "true" : "false") + ",";
            if (!emoji.empty())                 fp += "\"emoji\":" + emoji.to_json() + ",";

            fp.pop_back(); // , should be the last thing
            fp += "}"; // end

            return std::move(fp);
        }

        button_style ButtonObject::get_style() const
        {
            return style;
        }
        
        const std::string& ButtonObject::get_label() const
        {
            return label;
        }
        
        const Emoji& ButtonObject::get_emoji() const
        {
            return emoji;
        }
        
        const std::string& ButtonObject::get_custom_id() const
        {
            return custom_id;
        }
        
        const std::string& ButtonObject::get_url() const
        {
            return url;
        }
        
        bool ButtonObject::get_disabled() const
        {
            return disabled;
        }
        
        ButtonObject& ButtonObject::set_style(const button_style val)
        {
            style = val;
            return *this;
        }
        
        ButtonObject& ButtonObject::set_label(const std::string& val)
        {
            label = val;
            return *this;
        }
        
        ButtonObject& ButtonObject::set_emoji(const Emoji& val)
        {
            emoji = val;
            return *this;
        }
        
        ButtonObject& ButtonObject::set_custom_id(const std::string& val)
        {
            custom_id = val;
            return *this;
        }
        
        ButtonObject& ButtonObject::set_url(const std::string& val)
        {
            url = val;
            return *this;
        }
        
        ButtonObject& ButtonObject::set_disabled(const bool val)
        {
            disabled = val;
            return *this;
        }

        ActionRow::ActionRow()
            : GenericComponentObject(component_type::ACTION_ROW)
        {
        }

        ActionRow::ActionRow(const ActionRow& oth)
            : GenericComponentObject(component_type::ACTION_ROW)
        {
            *this = oth;
        }

        ActionRow::ActionRow(ActionRow&& oth)
            : GenericComponentObject(component_type::ACTION_ROW)
        {
            *this = std::move(oth);
        }

        void ActionRow::operator=(const ActionRow& oth)
        {
            components = oth.components;
        }

        void ActionRow::operator=(ActionRow&& oth)
        {
            components = std::move(oth.components);
        }

        bool ActionRow::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                logg << L::SL << Color::RED << "[ActionRow] Failed to load JSON, it was null!" << L::EL;
                return false;
            }

            if (const auto it = json["components"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {
                    ButtonObject newbuttonobj;
                    
                    if (newbuttonobj.load_from_json(inn.as_json())){
                        components.push_back(newbuttonobj);
                        continue;
                    }

                    logg << L::SL << Color::YELLOW << "[ActionRow] Failed to load one ButtonObject!" << L::EL;
                }
            }

            return true;
        }

        std::string ActionRow::to_json() const
        {            
            std::string fp;

            fp = "{";
            fp += "\"type\":" + std::to_string(static_cast<int>(get_type())) + ",";

            if (components.size() > 0){
                fp += "\"components\":[";

                for(const auto& i : components){
                    fp += i.to_json() + ",";
                }

                fp.pop_back(); // , should be the last thing
                fp += "],";
            }

            fp.pop_back(); // , should be the last thing
            fp += "}"; // end

            return std::move(fp);
        }

        bool ActionRow::add(const ButtonObject& val)
        {
            if (components.size() < 5) { // good with <= 5
                components.push_back(val);
                return true;
            } 
            return false;
        }

        const std::vector<ButtonObject>& ActionRow::get() const
        {
            return components;
        }

        std::vector<ButtonObject>& ActionRow::get_ref()
        {
            return components;
        }

        void GenericComponentObjectPTR::destroy()
        {
            if (generic){
                switch(generic->get_type()){
                case component_type::BUTTON:
                    {
                        ButtonObject* obj = (ButtonObject*)generic;
                        delete obj;
                        generic = nullptr;
                    }
                    break;
                case component_type::ACTION_ROW:
                    {
                        ActionRow* obj = (ActionRow*)generic;
                        delete obj;
                        generic = nullptr;
                    }
                    break;
                default:
                    logg << L::SL << Color::RED << "[GenericComponentObjectPTR] FATAL ERROR CANNOT CONTINUE, SERIOUSLY, MEMORY LEAK! RESTARTING ESP32!" << L::EL;
                    vTaskDelay(500 / portTICK_PERIOD_MS); // no rush
                    esp_restart();
                    break;
                }
            }
        }

        GenericComponentObjectPTR::~GenericComponentObjectPTR()
        {
            destroy();
        }
        
        GenericComponentObjectPTR::GenericComponentObjectPTR(GenericComponentObjectPTR&& oth)
        {
            if (generic) destroy();
            generic = oth.generic;
            oth.generic = nullptr;
        }

        GenericComponentObjectPTR::GenericComponentObjectPTR(GenericComponentObject* abs)
        {
            generic = (GenericComponentObject*)abs;
        }
        
        bool GenericComponentObjectPTR::gen_button()
        {
            if (generic) destroy();
            return (generic = (GenericComponentObject*)(new ButtonObject()));
        }

        bool GenericComponentObjectPTR::gen_actionrow()
        {
            if (generic) destroy();
            return (generic = (GenericComponentObject*)(new ActionRow()));
        }

        GenericComponentObject* GenericComponentObjectPTR::as_generic()
        {
            return generic;
        }

        ButtonObject* GenericComponentObjectPTR::as_button()
        {
            return (ButtonObject*)generic;
        }

        ActionRow* GenericComponentObjectPTR::as_actionrow()
        {
            return (ActionRow*)generic;
        }

        const ButtonObject* GenericComponentObjectPTR::as_button() const
        {
            return (ButtonObject*)generic;
        }

        const ActionRow* GenericComponentObjectPTR::as_actionrow() const
        {
            return (ActionRow*)generic;
        }

        component_type GenericComponentObjectPTR::get_type() const
        {
            if (generic) return generic->get_type();
            return component_type::BUTTON; // hmm
        }
        
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
            
            if (/*MemoryFileJSON*/ const auto it = json["id"]; !it.is_null())                   id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["channel_id"]; !it.is_null())           channel_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["parent_id"]; !it.is_null())            parent_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["guild_id"]; !it.is_null())             guild_id = it.as_llu();
            if (/*MemoryFileJSON*/ const auto it = json["content"]; !it.is_null())              content = handle_utf16(it.as_string());
            if (/*MemoryFileJSON*/ const auto it = json["timestamp"]; !it.is_null())            timestamp = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["edited_timestamp"]; !it.is_null())     edited_timestamp = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["mentions_everyone"]; !it.is_null())    mentions_everyone = it.as_bool();
            if (/*MemoryFileJSON*/ const auto it = json["referenced_message"]; !it.is_null()) { referenced_message = std::make_shared<Message>(core); referenced_message->load_from_json(it); } // replies and stuff lmao
            if (/*MemoryFileJSON*/ const auto it = json["type"]; !it.is_null())                 type = static_cast<message_type>(it.as_int());

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
                for(const auto& inn : it) {
                    unsigned long long plusid = inn["id"].as_llu();
                    if (!plusid) continue;
                    mentions.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["mention_roles"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {
                    unsigned long long plusid = inn.as_llu();
                    if (!plusid) continue;
                    mention_roles.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["mention_channels"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {
                    unsigned long long plusid = inn["id"].as_llu();
                    if (!plusid) continue;
                    mention_channels.push_back(plusid);
                }
            }

            if (/*MemoryFileJSON*/ const auto it = json["attachments"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {                    
                    Attachment newattach;
                    if (newattach.load_from_json(inn.as_json())){
                        attachments.push_back(std::move(newattach));
                        continue;
                    }

                    ESP_LOGW(MessageTAG, "Message::load_from_json parsing failed! Attachment parsing had issues parsing! Skipping.");
                }
            }

            if (const auto it = json["embeds"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {                    
                    Embed newembed;
                    newembed.load_from_json(inn.as_json());
                    embeds.push_back(std::move(newembed));
                }
            }

            if (const auto it = json["components"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {
                    const int res = inn["type"].as_int();
                    switch(res){
                    case static_cast<int>(component_type::ACTION_ROW):
                        {
                            GenericComponentObjectPTR gptr(new ActionRow());
                            gptr.as_actionrow()->load_from_json(inn.as_json());
                            components.push_back(std::move(gptr));
                        }
                        break;
                    case static_cast<int>(component_type::BUTTON):
                        {
                            GenericComponentObjectPTR gptr(new ButtonObject());
                            gptr.as_button()->load_from_json(inn.as_json());
                            components.push_back(std::move(gptr));
                        }
                        break;
                    }
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
        
        unsigned long long Message::get_parent_id() const
        {
            return parent_id;
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

        const std::vector<Embed>& Message::get_embeds() const
        {
            return embeds;
        }
        
        const std::vector<GenericComponentObjectPTR>& Message::get_components() const
        {
            return components;
        }

        const std::shared_ptr<Message> Message::get_referenced_message() const
        {
            return referenced_message;
        }
        
        message_type Message::get_type() const
        {
            return type;
        }

        Message& Message::set_id(const unsigned long long i)
        {
            id = i;
            return *this;
        }

        Message& Message::set_channel_id(const unsigned long long i)
        {
            channel_id = i;
            return *this;
        }

        Message& Message::set_parent_id(const unsigned long long i)
        {
            parent_id = i;
            return *this;
        }

        Message& Message::set_guild_id(const unsigned long long i)
        {
            guild_id = i;
            return *this;
        }

        Message& Message::set_content(const std::string& i)
        {
            content = i;
            return *this;
        }
        
        std::vector<Attachment> Message::get_attachments_ref()
        {
            return attachments;
        }

        std::vector<Embed>& Message::get_embeds_ref()
        {
            return embeds;
        }

        std::vector<GenericComponentObjectPTR>& Message::get_components_ref()
        {
            return components;
        }
        
        std::future<request_response> Message::delete_message()
        {
            if (!id || !channel_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(id);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_answer(const std::string& str)
        {
            if (!id || !channel_id || !guild_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            std::string json = "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\"," +
                "\"message_reference\":{"
                    "\"message_id\":\"" + std::to_string(id) + "\""
                "}"
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_answer(const std::string& str, const Embed& embed)
        {
            if (!id || !channel_id || !guild_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            std::string json = "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\"," +
                "\"embed\":[" + embed.to_json() + "]," +
                "\"message_reference\":{"
                    "\"message_id\":\"" + std::to_string(id) + "\""
                "}"
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_same_channel(const std::string& str)
        {
            if (!channel_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\""
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_message_same_channel(const std::string& str, const Embed& embed)
        {
            if (!channel_id) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            const std::string json = 
            "{"
                "\"content\":\"" + fix_quotes_string_for_json(str) + "\","
                "\"embed\":[" + embed.to_json() + "]"
            "}";

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::create_this_message()
        {
            if (!channel_id || (content.empty() && !embeds.size() && !components.size())) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages";
            const http_request method = http_request::POST;
            std::string json;

            json += "{";
            json += "\"content\":\"" + fix_quotes_string_for_json(content) + "\",";
            if (embeds.size() > 0)
            {
                json += "\"embeds\":[";

                for(const auto& i : embeds){
                    json += i.to_json() + ",";
                }

                json.pop_back(); // has ,
                json += "],";
            }

            if (components.size() > 0)
            {
                json += "\"components\":[";

                for(const auto& i : components) {
                    switch(i.get_type()){
                    case component_type::ACTION_ROW:
                        {
                            const ActionRow* obj = i.as_actionrow();
                            json += obj->to_json() + ",";
                        }
                        break;
                    case component_type::BUTTON:
                        {
                            const ButtonObject* obj = i.as_button();
                            json += obj->to_json() + ",";
                        }
                        break;
                    default:
                        logg << L::SL << Color::RED << "[recreate_this_message] FATAL ERROR CANNOT CONTINUE, SERIOUSLY, HOW IS THIS EVEN POSSIBLE?" << L::EL;
                        vTaskDelay(500 / portTICK_PERIOD_MS); // no rush
                        esp_restart();
                        break;
                    }
                }

                json.pop_back(); // has ,
                json += "],";
            }
            json.pop_back();
            json += "}";

            //logg << L::SL << Color::BRIGHT_GRAY << "[MSG] Tasking:\nPath=" << request << "\nMethod=POST\nJSON=" << json << L::EL;

            return core.post_task(request, method, json);
        }

        std::future<request_response> Message::edit_this_message_as_is()
        {
            if (!channel_id || !id || (content.empty() && !embeds.size() && !components.size())) return fake_future<request_response>();
            
            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(id);
            const http_request method = http_request::PATCH;
            std::string json;

            json += "{";
            json += "\"content\":\"" + fix_quotes_string_for_json(content) + "\",";
            if (embeds.size() > 0)
            {
                json += "\"embeds\":[";

                for(const auto& i : embeds){
                    json += i.to_json() + ",";
                }

                json.pop_back(); // has ,
                json += "],";
            }

            if (components.size() > 0)
            {
                json += "\"components\":[";

                for(const auto& i : components) {
                    switch(i.get_type()){
                    case component_type::ACTION_ROW:
                        {
                            const ActionRow* obj = i.as_actionrow();
                            json += obj->to_json() + ",";
                        }
                        break;
                    case component_type::BUTTON:
                        {
                            const ButtonObject* obj = i.as_button();
                            json += obj->to_json() + ",";
                        }
                        break;
                    default:
                        logg << L::SL << Color::RED << "[edit_this_message_as_is] FATAL ERROR CANNOT CONTINUE, SERIOUSLY, HOW IS THIS EVEN POSSIBLE?" << L::EL;
                        vTaskDelay(500 / portTICK_PERIOD_MS); // no rush
                        esp_restart();
                        break;
                    }
                }

                json.pop_back(); // has ,
                json += "],";
            }
            json.pop_back();
            json += "}";

            //logg << L::SL << Color::BRIGHT_GRAY << "[Message] Tasking:\nPath=" << request << "\nMethod=POST\nJSON=" << json << L::EL;

            return core.post_task(request, method, json);
        }

    }
}