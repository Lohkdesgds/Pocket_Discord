#include "tasker.hpp"

ESP_EVENT_DEFINE_BASE(EVENT_LSW_HANDLER);

namespace LSW {
    namespace PocketDiscord {
        
        void data_pass::copy(std::function<void(void)> ff, unsigned* cc)
        {
            if (!func) func = new std::function<void(void)>();
            *func = ff;
            counterr = cc;
        }

        void data_pass::freeup()
        {
            if (func) {
                delete func;
                func = nullptr;
            }
        }
        
        void data_pass::run_if()
        {
            if (func) (*func)();
        }

        void __eventpoll_static_task(void* idcnull, esp_event_base_t idcevbase, int32_t idcid, void* funcc)
        {
            data_pass& dp = *(data_pass*)funcc;
            (*dp.counterr)--;

            //if (show_debug) logg << L::SL << Color::BRIGHT_GRAY << "[EventPoll] Tasking one task right now on CORE #" << xPortGetCoreID() << "!" << L::EL;
            
            try {
                dp.run_if();
            }
            catch(const std::exception& e) {
                logg << L::SL << Color::RED << "[EventPoll] Event task on core #" << xPortGetCoreID() << " got an exception: " << e.what() << "." << L::EL;
            }
            catch(...) {
                logg << L::SL << Color::RED << "[EventPoll] Event task on core #" << xPortGetCoreID() << " got an exception: UNCAUGHT." << L::EL;
            }

            try {
                dp.freeup();
            }
            catch(const std::exception& e) {
                logg << L::SL << Color::RED << "[EventPoll] Event task on core #" << xPortGetCoreID() << " got an exception while cleaning up memory: " << e.what() << "." << L::EL;
            }
            catch(...) {
                logg << L::SL << Color::RED << "[EventPoll] Event task on core #" << xPortGetCoreID() << " got an exception while cleaning up memory: UNCAUGHT." << L::EL;
            }

            //if (show_debug) logg << L::SL << Color::BRIGHT_GRAY << "[EventPoll] Task on CORE #" << xPortGetCoreID() << " ended!" << L::EL;

            return;
        }

        EventPoll::~EventPoll()
        {
            reset();
        }

        bool EventPoll::setup(const std::string& eventname, const int32_t queuesiz, const unsigned max_time, const size_t buffer_size, const unsigned priority)
        {
            if (eventname.empty()) {
                logg << L::SL << Color::RED << "[EventPoll] Setup has invalid null name!" << L::EL;
                return false; // needs a name!
            }
            reset();

            if (max_time != 0) max_timeout = max_time;

            esp_event_loop_args_t event_loop_args = {
                .queue_size = queuesiz, // just queue buffer size, not threads number
                .task_name = eventname.c_str(),
                .task_priority = static_cast<UBaseType_t>(priority),
                .task_stack_size = buffer_size,
                .task_core_id = tskNO_AFFINITY
            };

            if (esp_event_loop_create(&event_loop_args, &loop_handle) != ESP_OK) {
                logg << L::SL << Color::RED << "[EventPoll] Can't create loop!" << L::EL;
                return false; // no good
            }

            is_setup = true;

            if (esp_event_handler_register_with(loop_handle, EVENT_LSW_HANDLER, ESP_EVENT_ANY_ID, &__eventpoll_static_task, nullptr) != ESP_OK) {
                logg << L::SL << Color::RED << "[EventPoll] Can't set function properly!" << L::EL;
                reset();
                return false;
            }

            return true;
        }

        event_result EventPoll::post(const std::function<void(void)> ff)
        {
            if (!is_setup) return event_result::ERROR;

            queue_siz_inn++;

            data_pass dp;
            dp.copy(ff, &queue_siz_inn);

            esp_err_t err = esp_event_post_to(loop_handle, EVENT_LSW_HANDLER, 0, (void*)&dp, sizeof(data_pass), pdMS_TO_TICKS(max_timeout));

            if (err == ESP_ERR_TIMEOUT) {
                dp.freeup();
                return event_result::TIMEOUT;
            }
            if (err != ESP_OK) {
                dp.freeup();
                return event_result::ERROR;
            }
            
            return event_result::NO_ERROR;
        }
        
        void EventPoll::reset()
        {
            if (is_setup) {
                esp_event_loop_delete(loop_handle);
                loop_handle = nullptr;
                is_setup = false;
            }
        }

        unsigned EventPoll::queue_size_now() const
        {
            return queue_siz_inn;
        }

    }
}