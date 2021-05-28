#include "discord/raw/http_interface.hpp"

namespace LSW {
    namespace PocketDiscord {

        int HttpConnection::_work(esp_http_client_event_t* evt)
        {
            switch(evt->event_id) {
            case HTTP_EVENT_ERROR:
                //ESP_LOGW(HTTPTAG, " - > HttpConnection::_work got ERROR");
                async_r.taskend = true;
                break;
            case HTTP_EVENT_ON_CONNECTED:
                //ESP_LOGI(HTTPTAG, " - > HttpConnection::_work CONNECTED");
                async_r.taskend = false;
                async_r.is_alive = true;
                break;
            case HTTP_EVENT_HEADER_SENT: // first this, then ON_HEADER event is triggered.
                //ESP_LOGI(HTTPTAG, "HttpConnection::_work HEADER SENT!");
                //async_r.headers[evt->header_key].append((char*)evt->data, evt->data_len);
                break;
            case HTTP_EVENT_ON_HEADER:
                //ESP_LOGI(HTTPTAG, " - > HttpConnection::_work working on HEADER [%s]->%s!",(char*)evt->header_key, (char*)evt->header_value);
                async_r.headers[evt->header_key].append((char*)evt->header_value);
                break;
            case HTTP_EVENT_ON_DATA:
                //ESP_LOGI(HTTPTAG, " - > HttpConnection::_work working on DATA %.*s", evt->data_len, (char*)evt->data);
                async_r.data.append((char*)evt->data, evt->data_len);
                break;
            case HTTP_EVENT_ON_FINISH:
                //ESP_LOGI(HTTPTAG, " - > HttpConnection::_work FINISH!");
                async_r.taskend = true;
                break;
            case HTTP_EVENT_DISCONNECTED:
                //ESP_LOGI(HTTPTAG, " - > HttpConnection::_work DISCONNECTED!");
                async_r.taskend = true;
                async_r.is_alive = false;
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1));

            return ESP_OK;
        }

        bool HttpConnection::reconnect(const bool silent)
        {
            if (!silent) logg << L::SL << Color::MAGENTA << "[HTTP] Cleaning up connections..." << L::EL;
            close();

            logg << L::SL << Color::GREEN << "[HTTP] Creating new connection..." << L::EL;

            const std::string user_agent = "LSW_DISCORD V1.0 Bot";
            const std::string bot_header = "Bot " + token_copy;

#ifndef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
            extern const uint8_t api_crt[] asm("_binary_api_pem_start");
#endif
            esp_http_client_config_t config = {
                .url = http_url.c_str(),
#ifndef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
                .cert_pem = (const char*) api_crt,
#endif
                .timeout_ms = http_max_timeout,
                .event_handler = [](esp_http_client_event_t* t){
                        HttpConnection* mee = (HttpConnection*)t->user_data;
                        return mee->_work(t);
                    },
                .user_data = this,
                //.is_async = true,
                .keep_alive_enable = true // can do multiple before close()!
            };

            if (!(client = esp_http_client_init(&config))){
                logg << L::SL << Color::RED << "[HTTP] Failed to initialize HTTP client." << L::EL;
                return false;
            }

            if (!silent) logg << L::SL << Color::MAGENTA << "[HTTP] Initialized, setting up HTTP headers and stuff..." << L::EL;
            
            esp_http_client_set_header(client, "User-Agent", user_agent.c_str());

            // these are for Discord thingy:
            esp_http_client_set_header(client, "Authorization", bot_header.c_str());
            esp_http_client_set_header(client, "Content-Type", "application/json");

            if (!silent) logg << L::SL << Color::GREEN << "[HTTP] Connection all good." << L::EL;

            async_r.is_alive = true;

            return true;
        }

        HttpConnection::~HttpConnection()
        {
            close();
        }
        
        bool HttpConnection::setup(const std::string& token, const bool silent)
        {
            token_copy = token;

            return reconnect(silent);
        }
        
        bool HttpConnection::task(const std::string& path, const http_request& method_orig, const std::string& data)
        {
            if (!async_r.is_alive){
                logg << L::SL << Color::GREEN << "[HTTP] Refreshing connection..." << L::EL;
                if (!reconnect(true)){
                    logg << L::SL << Color::YELLOW << "[HTTP] Refresh connection failed!" << L::EL;
                    return false; // can't reconnect
                }
                else logg << L::SL << Color::GREEN << "[HTTP] Refresh connection all good." << L::EL;
            } 
            if (!async_r.taskend) return false; // task didn't end
            if (!client) return false;

            async_r.headers.clear();
            async_r.data.clear();

            //good_to_read = false;
            const esp_http_client_method_t method = http_request_to_esp_method(method_orig);

            esp_http_client_set_url(client, (http_url + path).c_str());
            esp_http_client_set_method(client, method);

            int maxsiz = static_cast<int>(data.length());
            esp_err_t err = ESP_OK;

            for(int u = 0; u < 10; u++){
                if ((err = esp_http_client_open(client, maxsiz)) == ESP_OK) break;
                else logg << L::SL << Color::YELLOW << "[HTTP] Task timedout once [" << (u + 1) << "/10] | path=" << path << ", method=" << static_cast<int>(method_orig) << ", json=" << data << L::EL;
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            if (err != ESP_OK){
                logg << L::SL << Color::RED << "[HTTP] Task timeout." << L::EL;
                return false;
            }

            if (!data.empty())
            {
                if(esp_http_client_write(client, data.c_str(), data.length()) == ESP_FAIL) {
                    logg << L::SL << Color::RED << "[HTTP] Failed to write request." << L::EL;
                    return false;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(30));

            //esp_http_client_fetch_headers(client);
            
            esp_http_client_perform(client); // remaining tasks?

            return true;// assume true (res >= 200 && res < 300);
        }
        
        bool HttpConnection::has_task_ended() const
        {
            return async_r.taskend;
        }
        
        int HttpConnection::response_code() const
        {
            if (!async_r.taskend) return 0;

            return esp_http_client_get_status_code(client);
        }
        
        std::string HttpConnection::get_header(const std::string& key) const
        {   
            if (!async_r.taskend) return "";

            const auto it = async_r.headers.find(key);
            if (it != async_r.headers.end()) return it->second;
            return "";
        }
        
        const std::unordered_map<std::string, std::string>& HttpConnection::get_headers()
        {
            return async_r.headers;
        }
        
        MemoryFile HttpConnection::get_data() const
        {   
            if (!async_r.taskend) return MemoryFile();

            return std::move(async_r.data);
        }
        
        void HttpConnection::close()
        {
            //good_to_read = false;
            http_response = 0;
            if (client){
                esp_http_client_close(client);
                esp_http_client_cleanup(client);
                client = nullptr;
            }
            async_r.taskend = true;
        }

        esp_http_client_method_t http_request_to_esp_method(const http_request& me)
        {
            switch(me){                
            case http_request::GET:
                return HTTP_METHOD_GET;
            case http_request::POST:
                return HTTP_METHOD_POST;
            case http_request::PUT:
                return HTTP_METHOD_PUT;
            case http_request::PATCH:
                return HTTP_METHOD_PATCH;
            case http_request::DELETE:
                return HTTP_METHOD_DELETE;
            case http_request::HEAD:
                return HTTP_METHOD_HEAD;
            case http_request::NOTIFY:
                return HTTP_METHOD_NOTIFY;
            case http_request::SUBSCRIBE:
                return HTTP_METHOD_SUBSCRIBE;
            case http_request::UNSUBSCRIBE:
                return HTTP_METHOD_UNSUBSCRIBE;
            case http_request::OPTIONS:
                return HTTP_METHOD_OPTIONS;
            default:
                return HTTP_METHOD_GET; // wtf
            }
        }
        
        std::string handle_utf16(std::string wstr)
        {
            static std::wstring_convert<mycodecvt<char16_t, char, std::mbstate_t>, char16_t> convert; // known to have memory leak, so do fixed for now lmao
    
            std::u16string tempcombo;
            size_t pos = 0, old_pos = 0;
            do {
                if ((pos = wstr.find("\\u")) == std::string::npos) break;

                if (pos != old_pos && tempcombo.size()) {
                    wstr.insert(old_pos, convert.to_bytes(tempcombo));
                    tempcombo.clear();
                }

                if ((pos == 0 || wstr[pos - 1] != '\\') && (pos + sizeof("\\u") - 1 + 4) <= wstr.length())
                {
                    int dataa = 0;
                    sscanf(wstr.data() + pos + 2, "%4x", &dataa);
                    wstr.erase(pos, 6);
                    tempcombo += { static_cast<char16_t>(dataa) };
                    old_pos = pos;
                }

            } while (pos != std::string::npos);
            
            if (!tempcombo.empty()) wstr.insert(old_pos, convert.to_bytes(tempcombo));
            
            return wstr;
        }

    }
}