#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_http_client.h"

#include "defaults.h"
#include "heapstring.h"
#include "LJSON/json.h"

#include <unordered_map>

namespace Lunaris {
    namespace PocketDiscord {
        
        const char http_url[] = "https://discord.com/api/v10";
        const char http_user_agent[] = "Lunaris HTTPS embedded agent";
        const int http_max_timeout = 2000;
        
        const char cert_http_path[] = "cert/api.pem";

        enum class http_request {
            GET,
            POST,
            PUT,
            PATCH,
            DELETE,
            HEAD,
            NOTIFY,
            SUBSCRIBE,
            UNSUBSCRIBE,
            OPTIONS
        };

        class HTTPS_request{
            esp_http_client_handle_t m_ref = nullptr;            
            //std::unordered_map<HeapString, HeapString> m_headers;
            char* m_data = nullptr; // expect Content-Length from headers
            size_t m_data_write_off = 0;

            JSON* j = nullptr;
            int m_status = -1;
            bool m_was_prepared = false;
            bool m_finished = false;
            
            void handle_https_events(esp_http_client_event_t*);
        public:
            HTTPS_request(const HTTPS_request&) = delete;
            void operator=(const HTTPS_request&) = delete;
            void operator=(HTTPS_request&&) = delete;
            
            // req, path, data, len, cert, bot_header
            HTTPS_request(const http_request, const char*, const char*, const size_t, const HeapString&, const HeapString&);            
            HTTPS_request(HTTPS_request&&) noexcept;
            ~HTTPS_request();

            const JSON* json() const;
            bool has_finished() const;
            int get_status() const;
            const char* get_content_raw() const;
            size_t get_content_raw_size() const;
        };

        class HTTPS {
            //esp_http_client_handle_t m_client = nullptr;
            HeapString m_token;             // from constructor
            HeapString m_https_cert_perm;
            HeapString m_bot_header;
            HTTPS_request* m_last = nullptr;
        public:
            HTTPS(const char* token);
            ~HTTPS();

            // only one open at a time.
            HTTPS_request& post(const http_request, const char* path, const char* data = nullptr, const size_t data_len = 0);
        };

    }
}