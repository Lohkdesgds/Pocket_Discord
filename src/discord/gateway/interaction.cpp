#include "discord/gateway/interaction.hpp"

namespace LSW {
    namespace PocketDiscord {

        std::string InteractionApplicationCommandCallbackData::to_json() const
        {
            if (content.empty()) return {};

            std::string fp;

            fp = "{";
            fp += "\"tts\":" + std::string(tts ? "true" : "false") + ",";
            fp += "\"content\":\"" + content + "\",";
            fp += "\"flags\":" + std::to_string(flags) + "";
            
            if (embeds.size() > 0){
                fp += ",\"embeds\":[";

                for(const auto& i : embeds){
                    fp += i.to_json() + ",";
                }

                fp.pop_back(); // , should be the last thing
                fp += "]";
            }

            //fp.pop_back(); // , should be the last thing
            fp += "}"; // end

            return std::move(fp);
        }

        InteractionApplicationCommandCallbackData& InteractionApplicationCommandCallbackData::set_tts(const bool val)
        {
            tts = val;
            return *this;
        }

        InteractionApplicationCommandCallbackData& InteractionApplicationCommandCallbackData::set_content(const std::string& str)
        {
            content = str;
            return *this;
        }

        InteractionApplicationCommandCallbackData& InteractionApplicationCommandCallbackData::set_flags(const int i)
        {
            flags = i;
            return *this;
        }

        InteractionApplicationCommandCallbackData& InteractionApplicationCommandCallbackData::add_embed(Embed&& emb)
        {
            embeds.push_back(std::move(emb));
            return *this;
        }

        bool InteractionApplicationCommandCallbackData::get_tts() const
        {
            return tts;
        }

        const std::string& InteractionApplicationCommandCallbackData::get_content() const
        {
            return content;
        }
        
        const std::vector<Embed>& InteractionApplicationCommandCallbackData::get_embed() const
        {
            return embeds;
        }
        
        int InteractionApplicationCommandCallbackData::get_flags() const
        {
            return flags;
        }
        
        std::vector<Embed>& InteractionApplicationCommandCallbackData::get_embed_ref()
        {
            return embeds;
        }

        std::string InteractionResponse::to_json() const
        {
            std::string fp;

            fp = "{";
            fp += "\"type\":" + std::to_string(static_cast<int>(type));
            if (data.has_value()) fp += ",\"data\":" + data->to_json();

            //fp.pop_back(); // ,
            fp += "}";
            return std::move(fp);
        }

        void InteractionResponse::set_type(const interaction_type_response t)
        {
            type = t;
        }

        InteractionApplicationCommandCallbackData& InteractionResponse::generate_data()
        {
            data = InteractionApplicationCommandCallbackData();
            return *data;
        }
                
        Interaction::Interaction(BotCore& othcore)
            : core(othcore), member(othcore), message(othcore)
        {
        }
        
        bool Interaction::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                logg << L::SL << Color::RED << "[Interaction] Failed to load JSON, it was null!" << L::EL;
                return false;
            }
            
            if (const auto it = json["id"]; !it.is_null())                   id = it.as_llu();
            if (const auto it = json["token"]; !it.is_null())                token = it.as_string();
            if (const auto it = json["application_id"]; !it.is_null())       application_id = it.as_llu();
            if (const auto it = json["type"]; !it.is_null())                 type = static_cast<interation_type>(it.as_int());
            if (const auto it = json["guild_id"]; !it.is_null())             guild_id = it.as_llu();
            if (const auto it = json["channel_id"]; !it.is_null())           channel_id = it.as_llu();

            if (      auto it = json["data"]; !it.is_null()){
                if (const auto inn = it["name"]; !inn.is_null())             { interation_name = inn.as_string(); was_command = true;  } // for /command
                else if (const auto inn = it["custom_id"]; !inn.is_null())   { interation_name = inn.as_string(); was_command = false; } // for button reaction

                data = std::make_shared<MemoryFileJSON>(std::move(it));
            }

            if (const auto it = json["member"]; !it.is_null()) {
                member.load_from_json(it, true);
            }
            else if (const auto it = json["user"]; !it.is_null()) { // if no member, it may be DM, so it'll be USER instead.
                if (!member.get_user_ref().load_from_json(it)){
                    logg << L::SL << Color::YELLOW << "[Interaction] Failed to load Member, parse failed?!" << L::EL;
                }
            }

            if (const auto it = json["message"]; !it.is_null()) {
                if (!message.load_from_json(it)){
                    logg << L::SL << Color::YELLOW << "[Interaction] Failed to load Message, parse failed?!" << L::EL;
                }
            }

            return !token.empty() && id != 0 && channel_id != 0;
        }

        unsigned long long Interaction::get_id() const
        {
            return id;
        }

        const std::string& Interaction::get_token() const
        {
            return token;
        }
        
        unsigned long long Interaction::get_application_id() const
        {
            return application_id;
        }
        
        interation_type Interaction::get_type() const
        {
            return type;
        }
        
        const MemoryFileJSON& Interaction::get_data() const
        {
            return *data;
        }
        
        unsigned long long Interaction::get_guild_id() const
        {
            return id;
        }
        
        unsigned long long Interaction::get_channel_id() const
        {
            return channel_id;
        }
        
        const Member& Interaction::get_member() const
        {
            return member;
        }
        
        const User& Interaction::get_user() const
        {
            return member.get_user();
        }
        
        const Message& Interaction::get_message() const
        {
            return message;
        }

        bool Interaction::get_was_interaction_command() const
        {
            return was_command;
        }
        
        const std::string& Interaction::get_interaction_name() const
        {
            return interation_name;
        }
        
        std::future<request_response> Interaction::create_response(const InteractionResponse& res) const
        {
            if (!id || token.empty()) return fake_future<request_response>();
            
            const std::string request = "/interactions/" + std::to_string(id) + "/" + token + "/callback";
            const http_request method = http_request::POST;
            const std::string json = res.to_json();
            
            //logg << L::SL << Color::BRIGHT_GRAY << "[Interaction] Tasking:\nPath=" << request << "\nMethod=POST\nJSON=" << json << L::EL;

            return core.post_task(request, method, json);
        }
        
        std::future<request_response> Interaction::delete_trigger() const
        {
            if (!id || !channel_id) return fake_future<request_response>();

            const std::string request = "/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(id);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }

    }
}