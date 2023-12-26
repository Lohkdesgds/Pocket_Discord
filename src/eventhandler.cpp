#include "eventhandler.h"
#include "esp_log.h"

ESP_EVENT_DEFINE_BASE(EVENT_LSW_HANDLER);

static const char TAG[] = "EVH";

volatile size_t EventHandler::m_static_id = 0;
volatile bool EventHandlerDefault::has_registered_global = false;

static void __event_poll_handler(void* queue_ptr, esp_event_base_t ev_type, int32_t ev_id, void* fcn_ptr)
{
    volatile unsigned* vol = (volatile unsigned*)queue_ptr;
    
    FunctionWrapper* f_wrap = (FunctionWrapper*)(*(uintptr_t*)fcn_ptr);
    
    --(*vol);

    try {
        f_wrap->run();
    }
    catch(...) {
        ESP_LOGE(TAG, "Event %ld got exception at core %d!", ev_id, xPortGetCoreID());
    }

    try {        
        delete f_wrap;
    }
    catch(...) {
        ESP_LOGE(TAG, "Event %ld got exception trying to free memory at core %d!", ev_id, xPortGetCoreID());
    }
}

void EventHandler::destroy()
{
    if (m_handle) {
        delete m_queue_size;
        esp_event_loop_delete(m_handle);
        m_handle = nullptr;
    }
}

EventHandler::EventHandler(EventHandler&& e) noexcept
    : m_handle(e.m_handle), m_queue_size(e.m_queue_size), m_id(e.m_id)
{
    e.m_handle = nullptr;
    e.m_queue_size = nullptr;
    e.m_id = 0;
}

void EventHandler::operator=(EventHandler&& e) noexcept
{
    destroy();
    m_handle = e.m_handle;
    m_queue_size = e.m_queue_size;
    m_id = e.m_id;
    e.m_handle = nullptr;
    e.m_queue_size = nullptr;
    e.m_id = 0;
}

EventHandler::EventHandler(const char* task_name, const size_t queue_size, const size_t stack_size, const unsigned priority)
    : m_queue_size(new volatile unsigned(0)), m_id(++m_static_id)
{
    esp_event_loop_args_t event_loop_args = {
        .queue_size = static_cast<int32_t>(queue_size),
        .task_name = task_name,
        .task_priority = static_cast<UBaseType_t>(priority),
        .task_stack_size = stack_size,
        .task_core_id = tskNO_AFFINITY
    };

    if (esp_event_loop_create(&event_loop_args, &m_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Can't create event loop %s with queue sized %zu.", task_name, queue_size);
        throw Exception("Failed to create event loop");
    }
    
    if (esp_event_handler_register_with(m_handle, EVENT_LSW_HANDLER, ESP_EVENT_ANY_ID, &__event_poll_handler, (void*)m_queue_size) != ESP_OK) {
        ESP_LOGE(TAG, "Can't register event loop %s with queue sized %zu.", task_name, queue_size);
        throw Exception("Failed to register event loop");
    }
}

EventHandler::~EventHandler()
{
    destroy();
}

event_result EventHandler::post(FunctionWrapper fcn, const unsigned to)
{
    uintptr_t mov = (uintptr_t)new FunctionWrapper((FunctionWrapper&&)(fcn));
    ++(*m_queue_size);
    esp_err_t err = esp_event_post_to(m_handle, EVENT_LSW_HANDLER, 0, (void*)&mov, sizeof(mov), pdMS_TO_TICKS(to)); // mov is just the number, its value is the ptr value

    switch(err) {
    case ESP_ERR_TIMEOUT:
        delete (FunctionWrapper*)mov;
        --(*m_queue_size);
        return event_result::TIMEOUT;
    case ESP_OK:
        return event_result::NO_ERROR;
    default:
        delete (FunctionWrapper*)mov;
        --(*m_queue_size);
        return event_result::ERROR;
    }
}

unsigned EventHandler::queue_size_now() const
{
    return m_queue_size ? *m_queue_size : 0;
}

EventHandlerDefault::EventHandlerDefault(const bool start)
{
    if (start) {
        start_global();
    }
}

void EventHandlerDefault::start_global()
{
    const auto err = esp_event_loop_create_default();

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) throw Exception("Cannot create default event loop"); // invalid state == already created

    if (!has_registered_global) {
        has_registered_global = true;
        if (esp_event_handler_register(EVENT_LSW_HANDLER, ESP_EVENT_ANY_ID, &__event_poll_handler, nullptr) != ESP_OK) {
            ESP_LOGE(TAG, "Can't register on default event loop.");
            throw Exception("Failed to register event loop");
        }
    }
}

void EventHandlerDefault::destroy_global()
{
    esp_event_handler_unregister(EVENT_LSW_HANDLER, ESP_EVENT_ANY_ID, &__event_poll_handler);
    has_registered_global = false;
    esp_event_loop_delete_default();
}

event_result EventHandlerDefault::post(FunctionWrapper fcn, const unsigned to)
{
    if (!has_registered_global) return event_result::ERROR;

    uintptr_t mov = (uintptr_t)new FunctionWrapper((FunctionWrapper&&)(fcn));
    
    esp_err_t err = esp_event_post(EVENT_LSW_HANDLER, 0, (void*)&mov, sizeof(mov), pdMS_TO_TICKS(to)); // mov is just the number, its value is the ptr value

    switch(err) {
    case ESP_ERR_TIMEOUT:
        delete (FunctionWrapper*)mov;
        return event_result::TIMEOUT;
    case ESP_OK:
        return event_result::NO_ERROR;
    default:
        delete (FunctionWrapper*)mov;
        return event_result::ERROR;
    }
}