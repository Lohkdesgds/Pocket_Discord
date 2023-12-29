#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"

#include "functionwrapper.h"

constexpr size_t event_handler_queue_size_default = 5;

enum event_result {
    NO_ERROR,   // Good!
    TIMEOUT,    // Timeout, took longer than expected.
    ERROR       // other errors.
};
class EventHandler {
    esp_event_loop_handle_t m_handle = nullptr;
    volatile unsigned* m_queue_size;
    static volatile size_t m_static_id;
    size_t m_id;

    void destroy();
public:
    EventHandler(const EventHandler&) = delete;
    void operator=(const EventHandler&) = delete;
    EventHandler(EventHandler&&) noexcept;
    void operator=(EventHandler&&) noexcept;

    // Event loop name, queue size, stack size, queue priority
    EventHandler(const char*, const size_t = 3, const size_t = 4096, const unsigned = 4);

    ~EventHandler();

    // function (w or w/o args), timeout max
    event_result post(FunctionWrapper, const unsigned = 200);

    unsigned queue_size_now() const;
};

// Won't create default by itself unless explicit
// It won't destroy the default event loop unless explicit called.

class EventHandlerDefault {
    static volatile bool has_registered_global;
public:
    // create if not existant
    EventHandlerDefault(const bool = false);

    static void start_global();
    static void destroy_global();
    
    // function (w or w/o args), timeout max
    event_result post(FunctionWrapper, const unsigned = 200);
};