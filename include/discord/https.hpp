#pragma once

//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_http_client.h"
//
//#include "defaults.h"
//#include "heapstring.h"
//
//namespace Lunaris {
//    namespace PocketDiscord {
//        
//        const char http_url[] = "https://discord.com/api/v10";
//        const char http_user_agent[] = "";
//        const int http_max_timeout = 2000;
//        
//        const char cert_http_path[] = "cert/api.pem";
//
//        enum class http_request {
//            GET,
//            POST,
//            PUT,
//            PATCH,
//            DELETE,
//            HEAD,
//            NOTIFY,
//            SUBSCRIBE,
//            UNSUBSCRIBE,
//            OPTIONS
//        };
//
//        class HTTPS {
//            esp_http_client_handle_t client = nullptr;
//            HeapString m_token;             // from constructor
//            
//        public:
//            HTTPS(const char* token);
//        };
//
//    }
//}