#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_http_client.h"

#include "memory_file.hpp"
#include "printer.hpp"

#include <string>
#include <unordered_map>
#include <codecvt>
#include <locale>

template<class I, class E, class S>
struct mycodecvt : std::codecvt<I, E, S> { ~mycodecvt(){} }; // what? @ https://stackoverflow.com/questions/41744559/is-this-a-bug-of-gcc

namespace LSW {
    namespace PocketDiscord {

        const std::string http_url = "https://discord.com/api/v9";
        const int http_max_timeout = 10000;

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

        struct download_to {
            volatile bool taskend = false;
            volatile bool is_alive = false;
            std::unordered_map<std::string, std::string> headers;
            MemoryFile data;
        };

        class HttpConnection {
            esp_http_client_handle_t client = nullptr;
            std::string token_copy;
            int http_response = 0;

            download_to async_r;
            int _work(esp_http_client_event_t*);

            bool reconnect(const bool);
        public:
            ~HttpConnection();
            // token, no messages?
            bool setup(const std::string&, const bool = false);

            // URL, mode, json
            bool task(const std::string&, const http_request&, const std::string&);
            
            // if async download is good.
            bool has_task_ended() const;

            // http response
            int response_code() const;

            // get key header value
            std::string get_header(const std::string&) const;

            const std::unordered_map<std::string, std::string>& get_headers();

            // after task, read (MOVE).
            MemoryFile get_data() const;

            // after read, close
            void close();    
        };

        esp_http_client_method_t http_request_to_esp_method(const http_request&);

        // some stuff from HTTP goes like \uABCD, aka UTF16, so this handles that
        std::string handle_utf16(std::string);

    }
}