#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_http_client.h"

#include "defaults.h"
#include "useful.h"
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

        class ChunkableChar : public IterateableJSONRef{
            struct chunk {
                char* data = nullptr;
                size_t data_len = 0;
                chunk* next = nullptr;
                ~chunk();
            };

            chunk* m_c = nullptr;
            chunk* m_last = nullptr;
            size_t m_total = 0;
        public:
            ~ChunkableChar();
            void append_chunk(const char*, size_t);

            char get(const size_t at) const;
            void read(char* ptr, const size_t len, const size_t at) const;
            size_t max_off() const;
        };

        class HTTPS_request{

            struct rate_limit {
                uint64_t triggered_at = 0; // ms, use get_time_ms()
                double wait_seconds = 0;
                bool got_limited = false;
            };

            esp_http_client_handle_t m_ref = nullptr;
            ChunkableChar* m_data = nullptr;
            size_t m_data_write_off = 0;
            JSON* j = nullptr;
            rate_limit m_curr_rl;
            int m_status = -1;
            bool m_was_prepared = false;
            bool m_finished = false;
            
            static rate_limit m_rl;
            
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
        };

        class HTTPS {
            //esp_http_client_handle_t m_client = nullptr;
            HeapString m_token;             // from constructor
            HeapString m_https_cert_perm;
            HeapString m_bot_header;
        public:
            HTTPS(const char* token);

            HTTPS_request post(const http_request, const char* path, const char* data = nullptr, const size_t data_len = 0);
        };

    }
}