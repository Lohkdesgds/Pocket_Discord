#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char EmojiTAG[] = "Emoji";

        // based on Discord @ https://discord.com/developers/docs/topics/gateway#activity-object-activity-emoji
        class Emoji {
            std::string name;
            unsigned long long id = 0;
            bool animated = false;

            std::string encoded_name() const; // only needed if id == 0
        public:
            Emoji() = default;
            Emoji(const std::string&, const unsigned long long, const bool);

            std::string to_json() const;
            std::string format_text() const;
            std::string format_react() const;
            bool load_from_json(const MemoryFileJSON&);

            bool empty() const;

            Emoji& set_name(const std::string&);
            Emoji& set_id(const unsigned long long&);
            Emoji& set_animated(const bool&);

            const std::string& get_name() const;
            unsigned long long get_id() const;
            bool get_animated() const;
        };


        // based on Discord @ https://discord.com/developers/docs/resources/emoji#emoji-object-emoji-structure
    }
}