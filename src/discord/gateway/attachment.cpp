#include "discord/gateway/attachment.hpp"

namespace LSW {
    namespace PocketDiscord {

        bool Attachment::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                ESP_LOGE(AttachmentTAG, "Attachment::load_from_json parsing failed! JSON was null!");
                return false;
            }

            if (MemoryFileJSON _val = json["id"]; !_val.is_null())              id = _val.as_llu();
            if (MemoryFileJSON _val = json["filename"]; !_val.is_null())        filename = _val.as_string();
            if (MemoryFileJSON _val = json["size"]; !_val.is_null())            size = _val.as_int();
            if (MemoryFileJSON _val = json["url"]; !_val.is_null())             url = _val.as_string();

            return id != 0;
        }

        unsigned long long Attachment::get_id() const
        {
            return id;
        }
        
        const std::string& Attachment::get_filename() const
        {
            return filename;
        }
        
        int Attachment::get_size() const
        {
            return size;
        }
        
        const std::string& Attachment::get_url() const
        {
            return url;
        }
        
    }
}