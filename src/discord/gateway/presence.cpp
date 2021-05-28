#include "discord/gateway/presence.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        std::string activity::to_json() const
        {
            const std::string emoji_str = emoji.empty() ? "" : emoji.to_json();

            return "{"
                "\"name\":\"" + fix_quotes_string_for_json(name) + "\","
                "\"type\":" + std::to_string(static_cast<int>(type)) + 
                (url.empty() ? "" : ",\"url\":\"" + fix_quotes_string_for_json(url) + "\"") +
                (details.empty() ? "" : ",\"details\":\"" + fix_quotes_string_for_json(details) + "\"") + 
                (emoji_str.empty() ? "" : ",\"emoji\":" + emoji_str) +
            "}";
        }

        status_types string_to_status_types(const std::string& str)
        {
            if      (str == "online")                return status_types::ONLINE;
            else if (str == "dnd")                   return status_types::DO_NOT_DISTURB;
            else if (str == "idle")                  return status_types::IDLE;
            else if (str == "invisible")             return status_types::INVISIBLE;
            else if (str == "offline")               return status_types::OFFLINE;
            return status_types::INVISIBLE;
        }

        std::string status_types_to_string(const status_types& cod)
        {
            switch(cod){
            case status_types::ONLINE:
                return "online";
            case status_types::DO_NOT_DISTURB:
                return "dnd";
            case status_types::IDLE:
                return "idle";
            case status_types::INVISIBLE:
                return "invisible";
            case status_types::OFFLINE:
                return "offline";
            default:
                return ""; // empty
            }
        }
        
        std::string GatewayPresence::to_json() const
        {
            return "{"
                "\"since\":" + (since ? std::to_string(since) : "null") + ","
                "\"activities\":[" + activities.to_json() + "],"
                "\"status\":\"" + fix_quotes_string_for_json(status) + "\","
                "\"afk\":" + (afk ? "true" : "false") +
            "}";
        }

        GatewayPresence::GatewayPresence(BotCore& othcore)
            : core(othcore)
        {
        }
        
        // applpy to bot's presence
        bool GatewayPresence::apply()
        {
            since = get_time_now_ms();

            std::string raw = to_json();                        

            if (core._get_gateway().send_command(gateway_commands::UPDATE_PRESENCE, raw)){
                return true;
            }
            else ESP_LOGE(EmojiTAG, "GatewayPresence::apply JSON couldn't be applied. JSON: %s", raw.c_str());

            return false;
        }

        void GatewayPresence::set_status(const status_types& arg)
        {
            status = status_types_to_string(arg);
        }

        void GatewayPresence::set_afk(const bool& arg)
        {
            afk = arg;
        }
        

        // type, name, details, url
        void GatewayPresence::set_activity(const activity_types& type, const std::string& name, const std::string& details, const std::string& url)
        {
            activities.type = type;
            activities.name = name;
            activities.details = details;
            activities.url = url;
        }
        
        void GatewayPresence::set_activity_emoji(const GatewayEmoji& emoji)
        {
            activities.emoji = emoji;
        }
        

    }
}