#include "discord/gateway/embed.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        void EmbedFooter::load_from_json(const MemoryFileJSON& json)
        {
            if (const auto it = json["name"]; !it.is_null())                name = it.as_string();
            if (const auto it = json["icon_url"]; !it.is_null())            icon_url = it.as_string();
            if (const auto it = json["proxy_icon_url"]; !it.is_null())      proxy_icon_url = it.as_string();
        }

        std::string EmbedFooter::to_json() const
        {
            if (empty()) return {};

            std::string fp;

            fp = "{";
            if (!name.empty())              fp += "\"name\":\"" + name + "\",";
            if (!icon_url.empty())          fp += "\"icon_url\":\"" + icon_url + "\",";
            if (!proxy_icon_url.empty())    fp += "\"proxy_icon_url\":\"" + proxy_icon_url + "\",";

            fp.pop_back(); // makes sense because of design (one)
            fp += "}"; // end
            
            return fp;
        }

        bool EmbedFooter::empty() const
        {
            return name.empty() && icon_url.empty() && proxy_icon_url.empty();
        }
            
        EmbedFooter& EmbedFooter::set_name(const std::string& obj)
        {
            name = obj;
            return *this;
        }

        EmbedFooter& EmbedFooter::set_icon_url(const std::string& obj)
        {
            icon_url = obj;
            return *this;
        }
        
        EmbedFooter& EmbedFooter::set_proxy_icon_url(const std::string& obj)
        {
            proxy_icon_url = obj;
            return *this;
        }

        
        void EmbedImageThumbVideo::load_from_json(const MemoryFileJSON& json)
        {
            if (const auto it = json["url"]; !it.is_null())                 url = it.as_string();
            if (const auto it = json["proxy_url"]; !it.is_null())           proxy_url = it.as_string();
            if (const auto it = json["height"]; !it.is_null())              height = it.as_int();
            if (const auto it = json["width"]; !it.is_null())               width = it.as_int();
        }
        
        std::string EmbedImageThumbVideo::to_json() const
        {
            if (empty()) return {};

            std::string fp;

            fp = "{";
            if (!url.empty())               fp += "\"url\":\"" + url + "\",";
            if (!proxy_url.empty())         fp += "\"proxy_url\":\"" + proxy_url + "\",";
            if (height != 0)                fp += "\"height\":" + std::to_string(height) + ",";
            if (width != 0)                 fp += "\"width\":" + std::to_string(width) + ",";

            fp.pop_back();            
            fp += "}"; // end

            return fp;
        }

        bool EmbedImageThumbVideo::empty() const
        {
            return url.empty() && proxy_url.empty();
        }

        EmbedImageThumbVideo& EmbedImageThumbVideo::set_url(const std::string& obj)
        {
            url = obj;
            return *this;
        }

        EmbedImageThumbVideo& EmbedImageThumbVideo::set_proxy_url(const std::string& obj)
        {
            proxy_url = obj;
            return *this;
        }

        EmbedImageThumbVideo& EmbedImageThumbVideo::set_height(const int obj)
        {
            height = obj;
            return *this;
        }

        EmbedImageThumbVideo& EmbedImageThumbVideo::set_width(const int obj)
        {
            width = obj;
            return *this;
        }

        
        void EmbedProvider::load_from_json(const MemoryFileJSON& json)
        {
            if (const auto it = json["name"]; !it.is_null())                name = it.as_string();
            if (const auto it = json["url"]; !it.is_null())                 url = it.as_string();
        }
        
        std::string EmbedProvider::to_json() const
        {
            if (empty()) return {};
            
            std::string fp;
            
            fp = "{";
            if (!name.empty())              fp += "\"name\":\"" + name + "\",";
            if (!url.empty())               fp += "\"url\":\"" + url + "\",";

            fp.pop_back();
            fp += "}"; // end

            return fp;
        }

        bool EmbedProvider::empty() const
        {
            return name.empty() && url.empty();
        }

        EmbedProvider& EmbedProvider::set_name(const std::string& obj)
        {
            name = obj;
            return *this;
        }

        EmbedProvider& EmbedProvider::set_url(const std::string& obj)
        {
            url = obj;
            return *this;
        }
        
        void EmbedAuthor::load_from_json(const MemoryFileJSON& json)
        {
            if (const auto it = json["name"]; !it.is_null())                name = it.as_string();
            if (const auto it = json["url"]; !it.is_null())                 url = it.as_string();
            if (const auto it = json["icon_url"]; !it.is_null())            icon_url = it.as_string();
            if (const auto it = json["proxy_icon_url"]; !it.is_null())      proxy_icon_url = it.as_string();
        }
        
        std::string EmbedAuthor::to_json() const
        {
            if (empty()) return {};
            
            std::string fp;

            fp = "{";
            if (!name.empty())              fp += "\"name\":\"" + name + "\",";
            if (!url.empty())               fp += "\"url\":\"" + url + "\",";
            if (!icon_url.empty())          fp += "\"icon_url\":\"" + icon_url + "\",";
            if (!proxy_icon_url.empty())    fp += "\"proxy_icon_url\":\"" + proxy_icon_url + "\",";

            fp.pop_back();
            fp += "}"; // end

            return fp;
        }

        bool EmbedAuthor::empty() const
        {
            return name.empty() && url.empty() && icon_url.empty();
        }
            
        EmbedAuthor& EmbedAuthor::set_name(const std::string& obj)
        {
            name = obj;
            return *this;
        }

        EmbedAuthor& EmbedAuthor::set_url(const std::string& obj)
        {
            url = obj;
            return *this;
        }

        EmbedAuthor& EmbedAuthor::set_icon_url(const std::string& obj)
        {
            icon_url = obj;
            return *this;
        }

        EmbedAuthor& EmbedAuthor::set_proxy_icon_url(const std::string& obj)
        {
            proxy_icon_url = obj;
            return *this;
        }

        
        void EmbedField::load_from_json(const MemoryFileJSON& json)
        {
            if (const auto it = json["name"]; !it.is_null())                name = it.as_string();
            if (const auto it = json["value"]; !it.is_null())               value = it.as_string();
            if (const auto it = json["inline"]; !it.is_null())              _inline = it.as_bool();
        }
        
        std::string EmbedField::to_json() const
        {
            if (empty()) return {};
            
            std::string fp;

            fp = "{";
            if (!name.empty())              fp += "\"name\":\"" + fix_quotes_string_for_json(name).substr(0, 256) + "\",";
            if (!value.empty())             fp += "\"value\":\"" + fix_quotes_string_for_json(value).substr(0, 1024) + "\",";
            fp += "\"inline\":" + std::string(_inline ? "true" : "false") + ",";

            fp.pop_back(); // , should be the last thing
            fp += "}"; // end

            return std::move(fp);
        }

        bool EmbedField::empty() const
        {
            return name.empty() && value.empty();
        }

        EmbedField& EmbedField::set_name(const std::string& obj)
        {
            name = obj;
            return *this;
        }

        EmbedField& EmbedField::set_value(const std::string& obj)
        {
            value = obj;
            return *this;
        }

        EmbedField& EmbedField::set_inline(const bool obj)
        {
            _inline = obj;
            return *this;
        }
        
        void Embed::load_from_json(const MemoryFileJSON& json)
        {
            if (json.is_empty()) {
                logg << L::SL << Color::RED << "[Embed] Failed to load JSON, it was null!" << L::EL;
                return;
            }

            if (const auto it = json["title"]; !it.is_null())                title = it.as_string();
            if (const auto it = json["description"]; !it.is_null())          description = it.as_string();
            if (const auto it = json["url"]; !it.is_null())                  url = it.as_string();
            if (const auto it = json["timestamp"]; !it.is_null())            timestamp = it.as_string();
            if (const auto it = json["color"]; !it.is_null())                color = it.as_int();
            if (const auto it = json["footer"]; !it.is_null())               { footer = EmbedFooter();              footer->load_from_json(it);    }
            if (const auto it = json["image"]; !it.is_null())                { image = EmbedImageThumbVideo();      image->load_from_json(it);     }
            if (const auto it = json["thumbnail"]; !it.is_null())            { thumbnail = EmbedImageThumbVideo();  thumbnail->load_from_json(it); }
            if (const auto it = json["video"]; !it.is_null())                { video = EmbedImageThumbVideo();      video->load_from_json(it);     }
            if (const auto it = json["provider"]; !it.is_null())             { provider = EmbedProvider();          provider->load_from_json(it);  }
            if (const auto it = json["author"]; !it.is_null())               { author = EmbedAuthor();              author->load_from_json(it);    }

            if (const auto it = json["fields"]; !it.is_null() && it.is_array()) {
                for(const auto& inn : it) {                    
                    EmbedField newfield;
                    newfield.load_from_json(inn.as_json());
                    fields.push_back(std::move(newfield));
                }
            }
        }

        std::string Embed::to_json() const
        {
            std::string fp;

            fp = "{";
            if (!title.empty())                                              fp += "\"title\":\"" + fix_quotes_string_for_json(title) + "\",";
            if (!description.empty())                                        fp += "\"description\":\"" + fix_quotes_string_for_json(description) + "\",";
            if (!url.empty())                                                fp += "\"url\":\"" + url + "\",";
            if (!timestamp.empty())                                          fp += "\"timestamp\":\"" + timestamp + "\",";
            fp += "\"color\":" + std::to_string(color) + ",";
            if (footer.has_value() && !footer->empty())                      fp += "\"footer\":" + footer->to_json() + ",";
            if (image.has_value() && !image->empty())                        fp += "\"image\":" + image->to_json() + ",";
            if (thumbnail.has_value() && !thumbnail->empty())                fp += "\"thumbnail\":" + thumbnail->to_json() + ",";
            if (video.has_value() && !video->empty())                        fp += "\"video\":" + video->to_json() + ",";
            if (provider.has_value() && !provider->empty())                  fp += "\"provider\":" + provider->to_json() + ",";
            if (author.has_value() && !author->empty())                      fp += "\"author\":" + author->to_json() + ",";
            if (fields.size() > 0){
                fp += "\"fields\":[";

                for(const auto& i : fields){
                    fp += i.to_json() + ",";
                }

                fp.pop_back(); // , should be the last thing
                fp += "],";
            }

            fp.pop_back(); // , should be the last thing
            fp += "}"; // end

            return std::move(fp);
        }
        
        const std::string& Embed::get_title() const
        {
            return title;
        }

        const std::string& Embed::get_description() const
        {
            return description;
        }
        
        const std::string& Embed::get_url() const
        {
            return url;
        }
        
        const std::string& Embed::get_timestamp() const
        {
            return timestamp;
        }
        
        int Embed::get_color() const
        {
            return color;
        }
        
        bool Embed::has_footer() const
        {
            return footer.has_value();
        }
        
        const EmbedFooter& Embed::get_footer() const
        {
            return *footer;
        }
        
        bool Embed::has_image() const
        {
            return image.has_value();
        }
        
        const EmbedImageThumbVideo& Embed::get_image() const
        {
            return *image;
        }
        
        bool Embed::has_thumbnail() const
        {
            return thumbnail.has_value();
        }
        
        const EmbedImageThumbVideo& Embed::get_thumbnail() const
        {
            return *thumbnail;
        }
        
        bool Embed::has_video() const
        {
            return video.has_value();
        }
        
        const EmbedImageThumbVideo& Embed::get_video() const
        {
            return *video;
        }
        
        bool Embed::has_provider() const
        {
            return provider.has_value();
        }
        
        const EmbedProvider& Embed::get_provider() const
        {
            return *provider;
        }
        
        bool Embed::has_author() const
        {
            return author.has_value();
        }
        
        const EmbedAuthor& Embed::get_author() const
        {
            return *author;
        }
        
        const std::vector<EmbedField>& Embed::get_fields() const
        {
            return fields;
        }     

        Embed& Embed::set_title(const std::string& str)
        {
            title = str;
            return *this;
        }
        
        Embed& Embed::set_description(const std::string& str)
        {
            description = str;
            return *this;
        }
        
        Embed& Embed::set_url(const std::string& str)
        {
            url = str;
            return *this;
        }
        
        Embed& Embed::set_timestamp(const std::string& str)
        {
            timestamp = str;
            return *this;
        }
        
        Embed& Embed::set_color(const int i)
        {
            color = i;
            return *this;
        }
        
        Embed& Embed::set_footer(EmbedFooter&& obj)
        {
            footer = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_image(EmbedImageThumbVideo&& obj)
        {
            image = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_thumbnail(EmbedImageThumbVideo&& obj)
        {
            thumbnail = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_video(EmbedImageThumbVideo&& obj)
        {
            video = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_provider(EmbedProvider&& obj)
        {
            provider = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_author(EmbedAuthor&& obj)
        {
            author = std::move(obj);
            return *this;
        }
        
        Embed& Embed::set_author(const User& obj)
        {
            author = std::move(EmbedAuthor()
                .set_name(obj.format_full_name())
                .set_icon_url(obj.format_avatar_url()));
            return *this;
        }
        
        Embed& Embed::add_field(EmbedField&& obj)
        {
            fields.push_back(std::move(obj));
            return *this;
        }

        std::vector<EmbedField>& Embed::get_fields_ref()
        {
            return fields;
        }

    }
}