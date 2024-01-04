#include "discord/https.hpp"

#include "esp_log.h"
#include "filehandler.h"

#include <stdexcept>

namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "HTTPS";        

        esp_http_client_method_t http_request2esp(const http_request& me)
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
                return HTTP_METHOD_MAX; // wtf
            }
        }

        // STRing EQual Case Insensitive
        bool streqci(const char* s1, const char* s2, const size_t l)
        {
            if (!s1 || !s2) return false;
            for(size_t p = 0; p < l; ++p) {
                if (tolower(s1[p]) != tolower(s2[p])) return false;
            }
            return true;
        }
        
        void HTTPS_request::handle_https_events(esp_http_client_event_t* e)
        {
            switch(e->event_id) {
            case HTTP_EVENT_ERROR:
                ESP_LOGE(TAG, "HTTP_EVENT_ERROR!");
                break;
            case HTTP_EVENT_ON_CONNECTED:
                ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED!");
                break;
            case HTTP_EVENT_HEADERS_SENT: // all headers were sent
                ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT!");
                break;
            case HTTP_EVENT_ON_HEADER:
                ESP_LOGW(TAG, "New header: %s -> %s", e->header_key, e->header_value);
                if (streqci(e->header_key, "Content-Length", 14)) {
                    const auto ll = atoll(e->header_value);
                    ESP_LOGI(TAG, "Preparing for content of size %lli", ll);
                    m_was_prepared = true;
                    m_data_write_off = 0;
                    //m_data.prepare(ll + 1);
                    if (m_data) delete[] m_data; // Just to be sure u know
                    m_data = new char[ll + 1];
                    memset(m_data, '\0', ll + 1);
                }
                break;
            case HTTP_EVENT_ON_DATA:
                ESP_LOGW(TAG, "New data: %.*s [%i]", e->data_len, (char*)e->data, e->data_len);
                if (m_was_prepared) {
                    memcpy(m_data + m_data_write_off, (const char*)e->data, e->data_len);
                    m_data_write_off += e->data_len;
                }
                else {                    
                    ESP_LOGE(TAG, "DYNAMIC DATA BEING USED BECAUSE CONTENT-LENGTH HEADER WAS MISSING.");

                    char* old = m_data;
                    m_data = new char[m_data_write_off + e->data_len + 1];
                    if (old) {
                        memcpy(m_data, old, m_data_write_off);
                        delete[] old;
                    }
                    memcpy(m_data + m_data_write_off, e->data, e->data_len);
                    m_data_write_off += e->data_len;
                    m_data[m_data_write_off] = '\0';                    

                    //m_data.append((const char*)e->data, e->data_len);
                    //m_data_write_off += e->data_len;
                }
                break;
            case HTTP_EVENT_ON_FINISH:
                ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH!");
                {
                    if (m_data) {
                        j = new JSON(m_data, m_data_write_off);
                    }
                    m_finished = true;
                }
                break;
            case HTTP_EVENT_DISCONNECTED:
                ESP_LOGE(TAG, "HTTP_EVENT_DISCONNECTED!");
                break;
            case HTTP_EVENT_REDIRECT:
                ESP_LOGE(TAG, "HTTP_EVENT_REDIRECT!");
                break;
            default:
                break;
            }
        }

        HTTPS_request::HTTPS_request(const http_request r, const char* path, const char* data, const size_t data_len, const HeapString& m_https_cert_perm, const HeapString& m_bot_header)
        {
            esp_http_client_config_t config = {
                .url = http_url,
                .cert_pem = m_https_cert_perm.c_str(),
                .timeout_ms = http_max_timeout,
                .event_handler = [](esp_http_client_event_t* t){ ((HTTPS_request*)t->user_data)->handle_https_events(t); return ESP_OK; },
                .user_data = this,
                //.keep_alive_enable = true
                //.is_async = true
            };

            m_ref = esp_http_client_init(&config);

            {
                char* buf = nullptr;
                size_t buf_len = 0;
                saprintf(buf, &buf_len, "%s%s", http_url, path);
                
                esp_http_client_set_url(m_ref, buf);
                delete[] buf;
            }

            esp_http_client_set_method(m_ref, http_request2esp(r));
            
            esp_http_client_set_header(m_ref, "User-Agent", http_user_agent);
            // these are for Discord thingy:
            esp_http_client_set_header(m_ref, "Authorization", m_bot_header.c_str());
            esp_http_client_set_header(m_ref, "Content-Type", "application/json");

            ESP_LOGI(TAG, "Headers set:");
            ESP_LOGI(TAG, "- User-Agent -> %s", http_user_agent);
            ESP_LOGI(TAG, "- Authorization -> %s", m_bot_header.c_str());
            ESP_LOGI(TAG, "- Content-Type -> %s", "application/json");

            if (data && data_len > 0) esp_http_client_set_post_field(m_ref, data, data_len);

            esp_err_t err = ESP_OK;
            while (1) {
                err = esp_http_client_perform(m_ref);
                if (err != ESP_ERR_HTTP_EAGAIN) {
                    break;
                }
            }

            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
                esp_http_client_cleanup(m_ref);
                m_ref= nullptr;
                return;
            }

            m_status = esp_http_client_get_status_code(m_ref);
        }

        HTTPS_request::~HTTPS_request()
        {
            if (m_ref) esp_http_client_cleanup(m_ref);
            DEL_IT(j);
            DEL_EM(m_data);
        }

        const JSON* HTTPS_request::json() const
        {
            return j;
        }

        bool HTTPS_request::has_finished() const
        {
            return m_finished;
        }        

        int HTTPS_request::get_status() const
        {
            return m_status;
        }

        const char* HTTPS_request::get_content_raw() const
        {
            return m_data;
        }
        
        size_t HTTPS_request::get_content_raw_size() const
        {
            return m_data_write_off;
        }
        
        HTTPS::HTTPS(const char* token)
            : m_token(token)
        {            
            ESP_LOGI(TAG, "Initializing HTTPS for the first time, version %s for %s...", app_version, target_app);
            ESP_LOGI(TAG, "TOKEN: %s", m_token.c_str());

            ESP_LOGI(TAG, "Loading certificates...");

            READFILE_FULLY_TO(m_https_cert_perm, cert_http_path, "Could not load https certificate.");
            
            ESP_LOGI(TAG, "Certificate file loaded:\n\n%s\n", m_https_cert_perm.c_str());
            
            m_bot_header = "Bot ";
            m_bot_header.append(m_token.c_str(), m_token.size());
            
            ESP_LOGI(TAG, "HTTPS ready.");

            //start_https();
        }

        HTTPS::~HTTPS()
        {            
            DEL_IT(m_last);
        }
        
        // following https://github.com/espressif/esp-idf/blob/ece73357caa6c766770136b82639964870e340ba/examples/protocols/esp_http_client/main/esp_http_client_example.c#L623 this time
        HTTPS_request& HTTPS::post(const http_request r, const char* path, const char* data, const size_t data_len)
        {
            if (m_last) delete m_last;
            return *(m_last = new HTTPS_request(r, path, data, data_len, m_https_cert_perm, m_bot_header));
        }
        


    }
}