#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <optional>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/core.hpp"
#include "discord/gateway/user.hpp"

namespace LSW {
    namespace PocketDiscord {

        class EmbedFooter {
            std::string name;
            std::string icon_url;
            std::string proxy_icon_url;
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;
            bool empty() const;
            
            EmbedFooter& set_name(const std::string&);
            EmbedFooter& set_icon_url(const std::string&);
            EmbedFooter& set_proxy_icon_url(const std::string&);
        };

        class EmbedImageThumbVideo { // image, thumbnail and video are the same
            std::string url;
            std::string proxy_url;
            int height = 0;
            int width = 0;
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;
            bool empty() const;

            EmbedImageThumbVideo& set_url(const std::string&);
            EmbedImageThumbVideo& set_proxy_url(const std::string&);
            EmbedImageThumbVideo& set_height(const int);
            EmbedImageThumbVideo& set_width(const int);
        };

        class EmbedProvider {
            std::string name;
            std::string url;
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;
            bool empty() const;

            EmbedProvider& set_name(const std::string&);
            EmbedProvider& set_url(const std::string&);
        };

        class EmbedAuthor {
            std::string name;
            std::string url;
            std::string icon_url; // https or attachment
            std::string proxy_icon_url;
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;
            bool empty() const;
            
            EmbedAuthor& set_name(const std::string&);
            EmbedAuthor& set_url(const std::string&);
            EmbedAuthor& set_icon_url(const std::string&);
            EmbedAuthor& set_proxy_icon_url(const std::string&);
        };

        class EmbedField {
            std::string name;
            std::string value;
            bool _inline = false; // inline is a key in C/C++, so _ is necessary here
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;
            bool empty() const;

            EmbedField& set_name(const std::string&);
            EmbedField& set_value(const std::string&);
            EmbedField& set_inline(const bool);
        };

        class Embed { 
            std::string title;
            // embed TYPE is deprecated.
            std::string description;
            std::string url;
            std::string timestamp;
            int color = 0;
            std::optional<EmbedFooter> footer;
            std::optional<EmbedImageThumbVideo> image;
            std::optional<EmbedImageThumbVideo> thumbnail;
            std::optional<EmbedImageThumbVideo> video;
            std::optional<EmbedProvider> provider;
            std::optional<EmbedAuthor> author;
            std::vector<EmbedField> fields;
        public:
            void load_from_json(const MemoryFileJSON&);
            std::string to_json() const;

            const std::string& get_title() const;
            const std::string& get_description() const;
            const std::string& get_url() const;
            const std::string& get_timestamp() const;
            int get_color() const;
            bool has_footer() const;
            const EmbedFooter& get_footer() const;
            bool has_image() const;
            const EmbedImageThumbVideo& get_image() const;
            bool has_thumbnail() const;
            const EmbedImageThumbVideo& get_thumbnail() const;
            bool has_video() const;
            const EmbedImageThumbVideo& get_video() const;
            bool has_provider() const;
            const EmbedProvider& get_provider() const;
            bool has_author() const;
            const EmbedAuthor& get_author() const;
            const std::vector<EmbedField>& get_fields() const;

            Embed& set_title(const std::string&);
            Embed& set_description(const std::string&);
            Embed& set_url(const std::string&);
            Embed& set_timestamp(const std::string&);
            Embed& set_color(const int);
            Embed& set_footer(EmbedFooter&&);
            Embed& set_image(EmbedImageThumbVideo&&);
            Embed& set_thumbnail(EmbedImageThumbVideo&&);
            Embed& set_video(EmbedImageThumbVideo&&);
            Embed& set_provider(EmbedProvider&&);
            Embed& set_author(EmbedAuthor&&);
            Embed& set_author(const User&);
            Embed& add_field(EmbedField&&);

            std::vector<EmbedField>& get_fields_ref();
        };

    }
}