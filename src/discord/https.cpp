#include "discord/https.hpp"

#include "esp_log.h"
#include "filehandler.h"

#include <stdexcept>

namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "HTTPS";
        
        HTTPS_request::rate_limit HTTPS_request::m_rl;

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

        const char* http_request2str(const http_request& me)
        {
            switch(me){                
            case http_request::GET:
                return "GET";
            case http_request::POST:
                return "POST";
            case http_request::PUT:
                return "PUT";
            case http_request::PATCH:
                return "PATCH";
            case http_request::DELETE:
                return "DELETE";
            case http_request::HEAD:
                return "HEAD";
            case http_request::NOTIFY:
                return "NOTIFY";
            case http_request::SUBSCRIBE:
                return "SUBSCRIBE";
            case http_request::UNSUBSCRIBE:
                return "UNSUBSCRIBE";
            case http_request::OPTIONS:
                return "OPTIONS";
            default:
                return "NULL"; // wtf
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
        
        ChunkableChar::chunk::~chunk()
        {
            DEL_EM(data);
            DEL_IT(next);
        }

        ChunkableChar::~ChunkableChar()
        {
            DEL_IT(m_c);
        }

        void ChunkableChar::append_chunk(const char* s, size_t l)
        {
            if (!s || l == 0) return;

            chunk* m_new = new chunk();
            m_new->data = new char[l];
            memcpy(m_new->data, s, l);
            m_new->data_len = l;
            m_total += l;

            if (!m_c) {
                m_c = m_new;
                m_last = m_c;
            }
            else {
                m_last->next = m_new;
                m_last = m_last->next;
            }
        }

        char ChunkableChar::get(const size_t at) const
        {
            if (at >= m_total) return '\0';
            size_t p = at;
            for(chunk* c = m_c; c != nullptr; c = c->next)
            {
                if (p < c->data_len) return c->data[p];
                p -= c->data_len; // cannot result in less than 0 because of previous test
            }
            return '\0'; // somehow?
        }

        void ChunkableChar::read(char* ptr, const size_t len, const size_t at) const
        {
            memset(ptr, '\0', len);

            if (at >= m_total) return;
            
            size_t p = at; // offsetting on the way
            size_t l = len; // adjust on the way
            for(chunk* c = m_c; c != nullptr; c = c->next)
            {
                if (p < c->data_len) { // can read
                    const size_t max_read = c->data_len - p;
                    const size_t act_read = max_read < l ? max_read : l;
                    l -= act_read;
                    memcpy(ptr, c->data + p, act_read);
                    ptr += act_read; // offset pointer itself for easier run
                }
                else {
                    if (p >= c->data_len) p -= c->data_len;
                    else p = 0; // I know this shouldn't be necessary, but what if
                }
            }
        }

        size_t ChunkableChar::max_off() const
        {
            return m_total;
        }

        
        void HTTPS_request::handle_https_events(esp_http_client_event_t* e)
        {
            switch(e->event_id) {
            case HTTP_EVENT_ERROR:
                //ESP_LOGE(TAG, "HTTP_EVENT_ERROR!");
                break;
            case HTTP_EVENT_ON_CONNECTED:
                //ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED!");
                m_curr_rl.triggered_at = 0;
                m_curr_rl.wait_seconds = 0;
                m_curr_rl.got_limited = false;
                break;
            case HTTP_EVENT_HEADERS_SENT: // all headers were sent
                //ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT!");
                break;
            case HTTP_EVENT_ON_HEADER:
                //ESP_LOGW(TAG, "New header: %s -> %s", e->header_key, e->header_value);
                if (streqci(e->header_key, "X-RateLimit-Remaining", 21)) {
                    const int i = atoi(e->header_value);
                    m_curr_rl.got_limited |= (i <= 1);
                }
                else if (streqci(e->header_key, "X-RateLimit-Reset-After", 23)) {
                    m_curr_rl.wait_seconds = strtod(e->header_value, nullptr);
                    m_curr_rl.triggered_at = get_time_ms();
                }         
                break;
            case HTTP_EVENT_ON_DATA:
                //ESP_LOGW(TAG, "New data: %.*s [%i]", e->data_len, (char*)e->data, e->data_len);
                if (!m_data) m_data = new ChunkableChar();
                m_data->append_chunk((const char*)e->data, e->data_len);
                break;
            case HTTP_EVENT_ON_FINISH:
                //ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH!");
                {
                    if (m_curr_rl.got_limited) {
                        const double needed_delay_before = m_rl.got_limited ? (m_rl.wait_seconds - 0.001 * (get_time_ms() - m_rl.triggered_at)) : 0.0;
                        const double needed_delay_now    = m_curr_rl.wait_seconds - 0.001 * (get_time_ms() - m_curr_rl.triggered_at);
                        m_rl.triggered_at = get_time_ms();
                        m_rl.wait_seconds = needed_delay_now > needed_delay_before ? needed_delay_now : needed_delay_before;
                        m_rl.got_limited = true;
                    }

                    if (m_data) {
                        j = new JSON((ChunkableChar*&&)m_data);
                        m_data = nullptr;
                    }
                    m_finished = true;
                }
                break;
            case HTTP_EVENT_DISCONNECTED:
                //ESP_LOGE(TAG, "HTTP_EVENT_DISCONNECTED!");
                break;
            case HTTP_EVENT_REDIRECT:
                //ESP_LOGE(TAG, "HTTP_EVENT_REDIRECT!");
                break;
            default:
                break;
            }
        }

        HTTPS_request::HTTPS_request(const http_request r, const char* path, const char* data, const size_t data_len, const HeapString& m_https_cert_perm, const HeapString& m_bot_header)
        {
            if (m_rl.got_limited) {
                m_rl.got_limited = false;
                const double secs = (m_rl.wait_seconds - 0.001 * (get_time_ms() - m_rl.triggered_at));
                const uint64_t msecs = static_cast<uint64_t>(1000.0 * secs + 1);
                ESP_LOGW(TAG, "Got rate-limited, waiting %llu ms", msecs);
                vTaskDelay(pdMS_TO_TICKS(msecs));
            }

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
            ESP_LOGI(TAG, "Performed HTTP %s @ %s; Got status=%i.", http_request2str(r), path, m_status);
        }

        HTTPS_request::HTTPS_request(HTTPS_request&& o) noexcept
        {
            EXCHANGE(m_ref, o.m_ref, nullptr);
            EXCHANGE(m_data, o.m_data, nullptr);
            EXCHANGE(m_data_write_off, o.m_data_write_off, 0);
            EXCHANGE(j, o.j, nullptr);
            EXCHANGE(m_curr_rl, o.m_curr_rl, {});
            EXCHANGE(m_status, o.m_status, -1);
            EXCHANGE(m_was_prepared, o.m_was_prepared, false);
            EXCHANGE(m_finished, o.m_finished, false);
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
        }
        
        // following https://github.com/espressif/esp-idf/blob/ece73357caa6c766770136b82639964870e340ba/examples/protocols/esp_http_client/main/esp_http_client_example.c#L623 this time
        HTTPS_request HTTPS::post(const http_request r, const char* path, const char* data, const size_t data_len)
        {
            return HTTPS_request(r, path, data, data_len, m_https_cert_perm, m_bot_header);
        }
        


    }
}