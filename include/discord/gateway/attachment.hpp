#pragma once

#include <string>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char AttachmentTAG[] = "Attachment";
        
        // based on Discord @ https://discord.com/developers/docs/resources/channel#attachment-object
        class Attachment {
            unsigned long long id = 0;
            std::string filename;
            int size = 0;
            std::string url;
        public:
            bool load_from_json(const MemoryFileJSON&);
            
            unsigned long long get_id() const;
            const std::string& get_filename() const;
            int get_size() const;
            const std::string& get_url() const;
        };

    }
}