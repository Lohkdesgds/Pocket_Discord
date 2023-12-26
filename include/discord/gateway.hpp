#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_websocket_client.h"
#include "esp_transport_ws.h"

#include "../eventhandler.h"

namespace Lunaris {
    namespace PocketDiscord {

        const char gateway_url[] = "wss://gateway.discord.gg/?v=10&encoding=json";
        constexpr int gateway_max_timeout = 5000;
        constexpr size_t gateway_stack_size = 6 * 1024;
        constexpr size_t gateway_poll_stack_size = 16 * 1024;
        constexpr size_t gateway_buffer_size = 4096;
        constexpr unsigned gateway_self_priority = 1;
        constexpr unsigned gateway_queue_size_default = 30;
        constexpr unsigned gateway_poll_event_priority = 50;
        
    }
}