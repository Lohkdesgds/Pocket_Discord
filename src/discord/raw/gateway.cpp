#include "discord/raw/gateway.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        bool gateway_has_issues(const gateway_status& e)
        {
            return e == gateway_status::CLOSED || e == gateway_status::DISCONNECTED || e == gateway_status::ERROR || e == gateway_status::UNKNOWN;
        }
        
        gateway_commands string_to_gateway_commands(const std::string& str)
        {
            if      (str == "IDENTIFY")                 return gateway_commands::IDENTIFY;
            else if (str == "RESUME")                   return gateway_commands::RESUME;
            else if (str == "HEARTBEAT")                return gateway_commands::HEARTBEAT;
            else if (str == "REQUEST_GUILD_MEMBERS")    return gateway_commands::REQUEST_GUILD_MEMBERS;
            else if (str == "UPDATE_VOICE_STATE")       return gateway_commands::UPDATE_VOICE_STATE;
            else if (str == "UPDATE_PRESENCE")          return gateway_commands::UPDATE_PRESENCE;
            return gateway_commands::UNKNOWN; // never here, but you know.
        }

        std::string gateway_commands_to_string(const gateway_commands& cod)
        {
            switch(cod){
            case gateway_commands::IDENTIFY:
                return "IDENTIFY";
            case gateway_commands::RESUME:
                return "RESUME";
            case gateway_commands::HEARTBEAT:
                return "HEARTBEAT";
            case gateway_commands::REQUEST_GUILD_MEMBERS:
                return "REQUEST_GUILD_MEMBERS";
            case gateway_commands::UPDATE_VOICE_STATE:
                return "UPDATE_VOICE_STATE";
            case gateway_commands::UPDATE_PRESENCE:
                return "UPDATE_PRESENCE";
            default:
                return ""; // empty
            }
        }

        gateway_events string_to_gateway_events(const std::string& str)
        {
            if      (str == "HELLO")                          return gateway_events::HELLO;
            else if (str == "READY")                          return gateway_events::READY;
            else if (str == "RESUMED")                        return gateway_events::RESUMED;
            else if (str == "RECONNECT")                      return gateway_events::RECONNECT;
            else if (str == "INVALID_SESSION")                return gateway_events::INVALID_SESSION;
            else if (str == "APPLICATION_COMMAND_CREATE")     return gateway_events::APPLICATION_COMMAND_CREATE;
            else if (str == "APPLICATION_COMMAND_UPDATE")     return gateway_events::APPLICATION_COMMAND_UPDATE;
            else if (str == "APPLICATION_COMMAND_DELETE")     return gateway_events::APPLICATION_COMMAND_DELETE;
            else if (str == "CHANNEL_CREATE")                 return gateway_events::CHANNEL_CREATE;
            else if (str == "CHANNEL_UPDATE")                 return gateway_events::CHANNEL_UPDATE;
            else if (str == "CHANNEL_DELETE")                 return gateway_events::CHANNEL_DELETE;
            else if (str == "CHANNEL_PINS_UPDATE")            return gateway_events::CHANNEL_PINS_UPDATE;
            else if (str == "GUILD_CREATE")                   return gateway_events::GUILD_CREATE;
            else if (str == "GUILD_UPDATE")                   return gateway_events::GUILD_UPDATE;
            else if (str == "GUILD_DELETE")                   return gateway_events::GUILD_DELETE;
            else if (str == "GUILD_BAN_ADD")                  return gateway_events::GUILD_BAN_ADD;
            else if (str == "GUILD_BAN_REMOVE")               return gateway_events::GUILD_BAN_REMOVE;
            else if (str == "GUILD_EMOJIS_UPDATE")            return gateway_events::GUILD_EMOJIS_UPDATE;
            else if (str == "GUILD_INTEGRATIONS_UPDATE")      return gateway_events::GUILD_INTEGRATIONS_UPDATE;
            else if (str == "GUILD_MEMBER_ADD")               return gateway_events::GUILD_MEMBER_ADD;
            else if (str == "GUILD_MEMBER_REMOVE")            return gateway_events::GUILD_MEMBER_REMOVE;
            else if (str == "GUILD_MEMBER_UPDATE")            return gateway_events::GUILD_MEMBER_UPDATE;
            else if (str == "GUILD_MEMBERS_CHUNK")            return gateway_events::GUILD_MEMBERS_CHUNK;
            else if (str == "GUILD_ROLE_CREATE")              return gateway_events::GUILD_ROLE_CREATE;
            else if (str == "GUILD_ROLE_UPDATE")              return gateway_events::GUILD_ROLE_UPDATE;
            else if (str == "GUILD_ROLE_DELETE")              return gateway_events::GUILD_ROLE_DELETE;
            else if (str == "INTEGRATION_CREATE")             return gateway_events::INTEGRATION_CREATE;
            else if (str == "INTEGRATION_UPDATE")             return gateway_events::INTEGRATION_UPDATE;
            else if (str == "INTEGRATION_DELETE")             return gateway_events::INTEGRATION_DELETE;
            else if (str == "INTERACTION_CREATE")             return gateway_events::INTERACTION_CREATE;
            else if (str == "INVITE_CREATE")                  return gateway_events::INVITE_CREATE;
            else if (str == "INVITE_DELETE")                  return gateway_events::INVITE_DELETE;
            else if (str == "MESSAGE_CREATE")                 return gateway_events::MESSAGE_CREATE;
            else if (str == "MESSAGE_UPDATE")                 return gateway_events::MESSAGE_UPDATE;
            else if (str == "MESSAGE_DELETE")                 return gateway_events::MESSAGE_DELETE;
            else if (str == "MESSAGE_DELETE_BULK")            return gateway_events::MESSAGE_DELETE_BULK;
            else if (str == "MESSAGE_REACTION_ADD")           return gateway_events::MESSAGE_REACTION_ADD;
            else if (str == "MESSAGE_REACTION_REMOVE")        return gateway_events::MESSAGE_REACTION_REMOVE;
            else if (str == "MESSAGE_REACTION_REMOVE_ALL")    return gateway_events::MESSAGE_REACTION_REMOVE_ALL;
            else if (str == "MESSAGE_REACTION_REMOVE_EMOJI")  return gateway_events::MESSAGE_REACTION_REMOVE_EMOJI;
            else if (str == "PRESENCE_UPDATE")                return gateway_events::PRESENCE_UPDATE;
            else if (str == "TYPING_START")                   return gateway_events::TYPING_START;
            else if (str == "USER_UPDATE")                    return gateway_events::USER_UPDATE;
            else if (str == "VOICE_STATE_UPDATE")             return gateway_events::VOICE_STATE_UPDATE;
            else if (str == "VOICE_SERVER_UPDATE")            return gateway_events::VOICE_SERVER_UPDATE;
            else if (str == "WEBHOOKS_UPDATE")                return gateway_events::WEBHOOKS_UPDATE;
            return gateway_events::UNKNOWN;
        }

        std::string gateway_events_to_string(const gateway_events& cod)
        {
            switch(cod){
            case gateway_events::HELLO:                        
                return "HELLO";
            case gateway_events::READY:                        
                return "READY";
            case gateway_events::RESUMED:                      
                return "RESUMED";
            case gateway_events::RECONNECT:                    
                return "RECONNECT";
            case gateway_events::INVALID_SESSION:              
                return "INVALID_SESSION";
            case gateway_events::APPLICATION_COMMAND_CREATE:   
                return "APPLICATION_COMMAND_CREATE";
            case gateway_events::APPLICATION_COMMAND_UPDATE:   
                return "APPLICATION_COMMAND_UPDATE";
            case gateway_events::APPLICATION_COMMAND_DELETE:   
                return "APPLICATION_COMMAND_DELETE";
            case gateway_events::CHANNEL_CREATE:               
                return "CHANNEL_CREATE";
            case gateway_events::CHANNEL_UPDATE:               
                return "CHANNEL_UPDATE";
            case gateway_events::CHANNEL_DELETE:               
                return "CHANNEL_DELETE";
            case gateway_events::CHANNEL_PINS_UPDATE:          
                return "CHANNEL_PINS_UPDATE";
            case gateway_events::GUILD_CREATE:                 
                return "GUILD_CREATE";
            case gateway_events::GUILD_UPDATE:                 
                return "GUILD_UPDATE";
            case gateway_events::GUILD_DELETE:                 
                return "GUILD_DELETE";
            case gateway_events::GUILD_BAN_ADD:                
                return "GUILD_BAN_ADD";
            case gateway_events::GUILD_BAN_REMOVE:             
                return "GUILD_BAN_REMOVE";
            case gateway_events::GUILD_EMOJIS_UPDATE:          
                return "GUILD_EMOJIS_UPDATE";
            case gateway_events::GUILD_INTEGRATIONS_UPDATE:    
                return "GUILD_INTEGRATIONS_UPDATE";
            case gateway_events::GUILD_MEMBER_ADD:             
                return "GUILD_MEMBER_ADD";
            case gateway_events::GUILD_MEMBER_REMOVE:          
                return "GUILD_MEMBER_REMOVE";
            case gateway_events::GUILD_MEMBER_UPDATE:          
                return "GUILD_MEMBER_UPDATE";
            case gateway_events::GUILD_MEMBERS_CHUNK:          
                return "GUILD_MEMBERS_CHUNK";
            case gateway_events::GUILD_ROLE_CREATE:            
                return "GUILD_ROLE_CREATE";
            case gateway_events::GUILD_ROLE_UPDATE:            
                return "GUILD_ROLE_UPDATE";
            case gateway_events::GUILD_ROLE_DELETE:            
                return "GUILD_ROLE_DELETE";
            case gateway_events::INTEGRATION_CREATE:           
                return "INTEGRATION_CREATE";
            case gateway_events::INTEGRATION_UPDATE:           
                return "INTEGRATION_UPDATE";
            case gateway_events::INTEGRATION_DELETE:           
                return "INTEGRATION_DELETE";
            case gateway_events::INTERACTION_CREATE:           
                return "INTERACTION_CREATE";
            case gateway_events::INVITE_CREATE:                
                return "INVITE_CREATE";
            case gateway_events::INVITE_DELETE:                
                return "INVITE_DELETE";
            case gateway_events::MESSAGE_CREATE:               
                return "MESSAGE_CREATE";
            case gateway_events::MESSAGE_UPDATE:               
                return "MESSAGE_UPDATE";
            case gateway_events::MESSAGE_DELETE:               
                return "MESSAGE_DELETE";
            case gateway_events::MESSAGE_DELETE_BULK:          
                return "MESSAGE_DELETE_BULK";
            case gateway_events::MESSAGE_REACTION_ADD:         
                return "MESSAGE_REACTION_ADD";
            case gateway_events::MESSAGE_REACTION_REMOVE:      
                return "MESSAGE_REACTION_REMOVE";
            case gateway_events::MESSAGE_REACTION_REMOVE_ALL:  
                return "MESSAGE_REACTION_REMOVE_ALL";
            case gateway_events::MESSAGE_REACTION_REMOVE_EMOJI:
                return "MESSAGE_REACTION_REMOVE_EMOJI";
            case gateway_events::PRESENCE_UPDATE:              
                return "PRESENCE_UPDATE";
            case gateway_events::TYPING_START:                 
                return "TYPING_START";
            case gateway_events::USER_UPDATE:                  
                return "USER_UPDATE";
            case gateway_events::VOICE_STATE_UPDATE:           
                return "VOICE_STATE_UPDATE";
            case gateway_events::VOICE_SERVER_UPDATE:          
                return "VOICE_SERVER_UPDATE";
            case gateway_events::WEBHOOKS_UPDATE:              
                return "WEBHOOKS_UPDATE";
            default:
                return ""; // empty
            }
        }

        gateway_payload_structure::gateway_payload_structure()
            : json(__fporig)
        {            
        }

        gateway_payload_structure::gateway_payload_structure(gateway_payload_structure&& oth)
            : json(__fporig)
        {
            __fporig = std::move(oth.__fporig);

            op = oth.op;
            s = oth.s;
            t = oth.t;
        }

        void gateway_payload_structure::operator=(gateway_payload_structure&& oth)
        {
            __fporig = std::move(oth.__fporig);

            op = oth.op;
            s = oth.s;
            t = oth.t;
        }

        gateway_payload_structure::~gateway_payload_structure()
        {
            reset();
        }

        void gateway_payload_structure::reset()
        {
            __fporig.clear();
        }

        gateway_payload_structure::gateway_payload_structure(MemoryFile&& oth)
            : __fporig(std::move(oth)), json(__fporig, false)
        {            
            for(size_t reparse = 0; reparse < 3; reparse++) {
                if (reparse > 0) { // reparse?
                    logg << L::SL << Color::YELLOW << "[GW] PAYLOAD is trying to reset and re-parse [" << (reparse + 1) << "/3]" << L::EL;
                }
                json.reset_and_reparse();

                if (__fporig.size() == 0) {
                    logg << L::SL << Color::RED << "[GW] PAYLOAD failed! Empty MemoryFile?" << L::EL;
                    all_good = false;
                    return;
                }

                all_good = true;

                // - - - - - - - OP CODE - - - - - - - //

 #ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) Parsing OP code..." << L::EL;
 #endif 

                if (/*MemoryFileJSON*/ const auto it = json["op"]; !it.is_null()) {
                    int op_raw = it.as_int();
                    if (op_raw < 0 || op_raw > static_cast<int>(gateway_opcodes::_MAX)) {
                        op = gateway_opcodes::UNKNOWN;
                        all_good = false;
                        logg << L::SL << Color::RED << "[GW] PAYLOAD failed! Invalid OP code: " << op_raw << "." << L::EL;
                        continue;
                    }
                    else{
                        op = static_cast<gateway_opcodes>(op_raw);
                    }
                }
                if (op == gateway_opcodes::UNKNOWN){
                    logg << L::SL << Color::RED << "[GW] OP code is UNKNOWN?! Restarting ESP32..." << L::EL;
                    __c_gateway_got_unhandled_disconnect_so_restart("Invalid OP code");
                    continue;
                } 
 #ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) * OP=" << static_cast<int>(op) << L::EL;
 #endif 

            // - - - - - - - SEQ NUMBER - - - - - - - //


 #ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) Parsing SEQ NUMBER..." << L::EL;
#endif 

                if (/*MemoryFileJSON*/ const auto it = json["s"]; !it.is_null()) {
                    s = static_cast<long long>(it.as_llu());
                }

            // - - - - - - - evenT NUMBER - - - - - - - //


#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) Parsing evenT..." << L::EL;
#endif 

                if (/*MemoryFileJSON*/ const auto it = json["t"]; !it.is_null()) {
                    t = string_to_gateway_events(it.as_string());

                    if (t == gateway_events::UNKNOWN){
                        all_good = false;
                        logg << L::SL << Color::RED << "[GW] PAYLOAD failed! Invalid EVENT code (" << it.as_string() << ")." << L::EL;
                    }
                }
#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) * EVENTNUM=" << static_cast<int>(t) << L::EL;
#endif 
            
/*
#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                logg << L::SL << Color::BRIGHT_GRAY << "[GW](gateway_payload_structure) Good? " << (all_good ? "Y" : "N") << L::EL;
#endif 
    */
                return;
            }
        }

        void GatewayControl::pure_restart()
        {
            logg << L::SL << Color::YELLOW << "[GW] PureRestart is restarting all possible gateway modules right now..." << L::EL;
            unique_cfg.status = gateway_status::ERROR;

            if (unique_cfg.tasking_hint > 0){
                logg << L::SL << Color::YELLOW << "[GW] It looks like there are " << unique_cfg.tasking_hint << " tasks being done right now. Waiting up to 30 seconds..." << L::EL;
                for(size_t p = 0; p < 30; p++) {
                    if (unique_cfg.tasking_hint > 0) {
                        vTaskDelay(1000 / portTICK_PERIOD_MS); // no need to go fast.
                    }
                    else break;
                }
                logg << L::SL << Color::YELLOW << "[GW] PureRestart is restarting with " << unique_cfg.tasking_hint << " tasks yet to end..." << L::EL;
            }

            unique_cfg.ready_event_got = false;
            unique_cfg.resumed_successfully = false;

            if (get_time_now_ms() - last_pure_restart < 60000) {
                if (++pure_restart_short_intervals_count > 5){
                    logg << L::SL << Color::YELLOW << "[GW] PureRestart detected looping. Restarting ESP32." << L::EL;
                    __c_gateway_got_unhandled_disconnect_so_restart("PURE_RESTART detected looping."); // this is bad.
                }
                else {
                    close_gateway();
                    logg << L::SL << Color::YELLOW << "[GW] PureRestart is closing early and holding for some seconds, because things are messy..." << L::EL;
                    vTaskDelay(pure_restart_short_intervals_count * 3000 / portTICK_PERIOD_MS); // no need to go fast.
                    unique_cfg.session_id.clear(); // fresh start
                }
            }
            else pure_restart_short_intervals_count = 0;

            logg << L::SL << Color::YELLOW << "[GW] PureRestart is trying to reconnect..." << L::EL;

            if (!reconnect()){
                logg << L::SL << Color::RED << "[GW] Can't restart connection! Restarting ESP32..." << L::EL;
                __c_gateway_got_unhandled_disconnect_so_restart("PURE_RESTART couldn't reconnect. This is bad."); // this is bad.
            }
            
            logg << L::SL << Color::GREEN << "[GW] PureRestart done!" << L::EL;
            last_pure_restart = get_time_now_ms();
            unique_cfg.status = gateway_status::UNKNOWN;
        }

        void GatewayControl::gateway_handling()
        {
            logg << L::SL << Color::GREEN << "[GW] Async handling is running!" << L::EL;

            bool was_on_once = false;
            int fails_count = 0;

            while (gateway_thr_run)
            {
                try{
                    if (fails_count > 15){
                        logg << L::SL << Color::RED << "[GW] Fail count is 15+. Restarting ESP32..." << L::EL;
                        __c_gateway_got_unhandled_disconnect_so_restart("Too many fails on gateway handling."); // this is bad.
                    }
                    vTaskDelay(25 / portTICK_PERIOD_MS); // no need to go fast.

                    switch (unique_cfg.status) { // discord.cpp -> dc_task
                    case gateway_status::RECONNECT:
                    case gateway_status::ERROR:
                    case gateway_status::CLOSED:
                    case gateway_status::DISCONNECTED:
                    {
                        pure_restart();
                        /*if (!reconnect()){
                            logg << L::SL << Color::RED << "[GW] Handling can't reconnect. Restarting ESP32!" << L::EL;
                            __c_gateway_got_unhandled_disconnect_so_restart("RECONNECT couldn't reconnect. This is bad."); // this is bad.
                        }*/
                    }
                        break;
                    case gateway_status::STARTUP:            // Started thread right now, first step.
                    {
                        logg << L::SL << Color::GREEN << "[GW] Identifying itself..." << L::EL;

                        std::string json_to_send;

                        { // safe
                            std::shared_lock<std::shared_mutex> safe_lock(unique_cfg.data_mtx);

                            if (!was_on_once || !unique_cfg.resumed_successfully || unique_cfg.session_id.empty()){
                                logg << L::SL << Color::GREEN << "[GW] Session ID is null or invalid, assuming new connection..." << L::EL;

                                json_to_send = 
                                "{"
                                    "\"op\":" + std::to_string(static_cast<int>(gateway_commands::IDENTIFY)) + ","
                                    "\"d\":{"
                                        "\"token\":\"" + unique_cfg.token + "\","
                                        "\"intents\":" + std::to_string(unique_cfg.intents) + ","
                                        "\"properties\":{"
                                            "\"$os\":\"esp-idf (" + std::string(esp_get_idf_version()) + ")\","
                                            "\"$browser\":\"LSW_DISCORD (" + app_version + ")\","
                                            "\"$device\":\"" + target_app + "\""
                                        "}"
                                    "}"
                                "}";
                            }
                            else {
                                logg << L::SL << Color::GREEN << "[GW] Session ID exists, trying to RESUME..." << L::EL;
                                unique_cfg.resumed_successfully = false;
                                // do reconnect thingy
                                json_to_send = 
                                "{"
                                    "\"op\":" + std::to_string(static_cast<int>(gateway_commands::RESUME)) + ","
                                    "\"d\":{"
                                        "\"token\":\"" + unique_cfg.token + "\","
                                        "\"session_id\":\"" + unique_cfg.session_id + "\","
                                        "\"seq\":" + (unique_cfg.sequence_number != -1 ? std::to_string(unique_cfg.sequence_number) : "null") +
                                    "}"
                                "}";
                            }
                        }

                        if (!send_raw_json(json_to_send)) {
                            logg << L::SL << Color::RED << "[GW] Can't RESUME! Restarting ESP32..." << L::EL;
                            __c_gateway_got_unhandled_disconnect_so_restart("IDENTIFY/RESUME couldn't be sent. This is bad."); // this is really bad.
                        }
                        else{
                            logg << L::SL << Color::GREEN << "[GW] Identified itself! Bot is online!" << L::EL;
                            //unique_cfg.ack_heartbeat = get_time_now_ms(); // unbug
                            unique_cfg.ack_heartbeat = true; // unbug
                            unique_cfg.status = gateway_status::CONNECTED;
                            was_on_once = true;
                            unique_cfg.resumed_successfully = true;
                        }
                    }
                        break;
                    case gateway_status::CONNECTED:          // WEBSOCKET_EVENT_CONNECTED
                    {
                        const unsigned long long time_now = get_time_now_ms();
                        std::shared_lock<std::shared_mutex> safe_lock(unique_cfg.data_mtx);
                        //const bool heartbeat_close_enough = (time_now - unique_cfg.ack_heartbeat) <= (1.5 * unique_cfg.heartbeat_interval);

                        // top priority.
                        if (time_now >= unique_cfg.heartbeat_at) {
                            if (!unique_cfg.ack_heartbeat && unique_cfg.heartbeat_at != 0) {
                                logg << L::SL << Color::YELLOW << "[GW] ACK Heartbeat wasn't set! Invalid session?! Trying RESUME..." << L::EL;
                                unique_cfg.status = gateway_status::RECONNECT;
                                unique_cfg.ready_event_got = false;
                                fails_count++;
                                continue;
                            }
                            else unique_cfg.ack_heartbeat = false; // reset
                            
                            const std::string json_to_send = 
                            "{"
                                "\"op\":" + std::to_string(static_cast<int>(gateway_commands::HEARTBEAT)) + ","
                                "\"d\":" + (unique_cfg.sequence_number != -1 ? std::to_string(unique_cfg.sequence_number) : "null") +
                            "}";

                            if (!send_raw_json(json_to_send)) {
                                if (fails_count < 5){
                                    logg << L::SL << Color::RED << "[GW] Can't send HEARTBEAT. Trying again..." << L::EL;
                                    fails_count++;
                                    continue;
                                }
                                else{
                                    logg << L::SL << Color::RED << "[GW] Can't send HEARTBEAT. Trying RESUME this time..." << L::EL;
                                    unique_cfg.status = gateway_status::RECONNECT;
                                    continue;
                                }
                            }
#ifdef LSW_SHOW_DETAILED_EVENTS
                            logg << L::SL << Color::GREEN << "[GW] Heartbeated once!" << L::EL;
#endif

                            safe_lock.unlock(); // so we can lock as:
                            std::unique_lock<std::shared_mutex> safe_lock2(unique_cfg.data_mtx);

                            unique_cfg.heartbeat_at = time_now + unique_cfg.heartbeat_interval;
                        }


                    }
                        break;
                    /*case gateway_status::ERROR:              // WEBSOCKET_EVENT_ERROR
                        logg << L::SL << Color::YELLOW << "[GW] Detected error! Trying to recover by refresh..." << L::EL;
                        {
                            unique_cfg.ready_event_got = false;
                            unique_cfg.status = gateway_status::RECONNECT;
                            unique_cfg.resumed_successfully = false;
                            unique_cfg.session_id.clear();

                            vTaskDelay(1000 / portTICK_PERIOD_MS); // no need to go fast.

                            if (!reconnect()){
                                logg << L::SL << Color::RED << "[GW] Can't restart connection! Restarting ESP32..." << L::EL;
                                __c_gateway_got_unhandled_disconnect_so_restart("RECONNECT couldn't reconnect. This is bad."); // this is bad.
                            }
                        }
                        return;
                    case gateway_status::DISCONNECTED:       // WEBSOCKET_EVENT_DISCONNECTED
                        logg << L::SL << Color::YELLOW << "[GW] Disconnected?! Restarting ESP32!" << L::EL;
                        __c_gateway_got_unhandled_disconnect_so_restart("DISCONNECTED in handling thread."); // this is bad.
                        return;
                    case gateway_status::CLOSED:             // WEBSOCKET_EVENT_CLOSED
                        logg << L::SL << Color::YELLOW << "[GW] Closed?! Restarting ESP32!" << L::EL;
                        __c_gateway_got_unhandled_disconnect_so_restart("CLOSED in handling thread."); // this is bad.
                        return;*/
                    case gateway_status::UNKNOWN:            // other event not in list and not WEBSOCKET_EVENT_DATA (common data work)
                        break; // do nothing.
                    }
                }
                catch(const std::exception& e)
                {
                    logg << L::SL << Color::RED << "[GW] Exception! " << e.what() << ". Trying to RESUME in 3 seconds..." << L::EL;
                    vTaskDelay(3000 / portTICK_PERIOD_MS); // no need to go fast.
                    unique_cfg.status = gateway_status::RECONNECT;
                    fails_count++;
                    continue;
                }
                catch(...)
                {
                    logg << L::SL << Color::RED << "[GW] Exception! (UNCAUGHT). Trying to RESUME in 3 seconds..." << L::EL;
                    vTaskDelay(3000 / portTICK_PERIOD_MS); // no need to go fast.
                    unique_cfg.status = gateway_status::RECONNECT;
                    fails_count++;
                    continue;
                }
                fails_count = 0;
            }
        }

        void GatewayControl::close_gateway()
        {
            unique_cfg.no_gateway_please = true;
            logg << L::SL << Color::YELLOW << "[GW] Closing gateway..." << L::EL;
            
            if (client_handle) {
                // 1001 @ 7.4.1: https://tools.ietf.org/html/rfc6455#section-7.4, Discord no 1000 or 1001, Discord API recommended 1012 (service restarting) close ID @ https://discord.com/developers/docs/topics/gateway#disconnections

                bool good = true;
                for(int tries = 0; tries < 3; tries++){
                    if (esp_websocket_client_close_with_code(client_handle, 1012, nullptr, 0, gateway_max_timeout / portTICK_PERIOD_MS) != ESP_OK) { 
                        if (esp_websocket_client_stop(client_handle) != ESP_OK) {
                            logg << L::SL << Color::RED << "[GW] Can't close current connection [" << (tries + 1) << "/3]." << L::EL;
                            vTaskDelay(200 / portTICK_PERIOD_MS); // no rush
                            good = false;
                        }
                        else good = true;
                    }
                    else good = true;

                    if (unique_cfg.confirm_close) {
                        logg << L::SL << Color::YELLOW << "[GW] CLOSE confirmation was set, assuming it's closed..." << L::EL;
                        vTaskDelay(100 / portTICK_PERIOD_MS); // no rush
                        good = true;
                        break;
                    }
                }

                if (!good) {
                    logg << L::SL << Color::RED << "[GW] CAN'T CLOSE GATEWAY! Assuming client was not started (somehow)." << L::EL;
                    unique_cfg.confirm_close = true; // forced
                    //__c_gateway_got_unhandled_disconnect_so_restart("GatewayControl::reconnect could not close gateway properly.");
                }
                
                if (client_handle) esp_websocket_client_destroy(client_handle);

                client_handle = nullptr;
                if (good) {
                    logg << L::SL << Color::GREEN << "[GW] Waiting final close event..." << L::EL;
                    while(!unique_cfg.confirm_close) vTaskDelay(250 / portTICK_PERIOD_MS); // no rush
                }
            }

            //unique_cfg.status = gateway_status::UNKNOWN; // reset
            unique_cfg.no_gateway_please = false; // good to go.
            
            logg << L::SL << Color::GREEN << "[GW] Closed succesfully!" << L::EL;
        }

        bool GatewayControl::reconnect()
        {
            logg << L::SL << Color::GREEN << "[GW] Working on reconnect..." << L::EL;

            close_gateway();

#ifndef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
            extern const uint8_t gateway_crt[] asm("_binary_gateway_pem_start");
#endif
            
            esp_websocket_client_config_t ws_cfg = {
                    .uri = (const char*) gateway_url,
                    .disable_auto_reconnect = true,
                    .task_prio = gateway_self_priority,
                    .task_stack = gateway_stack_size,
                    .buffer_size = gateway_buffer_size,
#ifndef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
                    .cert_pem = (const char*)gateway_crt
#endif
            };

            logg << L::SL << Color::GREEN << "[GW] Initializing client..." << L::EL;

            if(!(client_handle = esp_websocket_client_init(&ws_cfg))) {
                logg << L::SL << Color::RED << "[GW] Can't initialize client!" << L::EL;
                return false;
            }

            logg << L::SL << Color::GREEN << "[GW] Registering event poll..." << L::EL;

            if (!unique_cfg.async_task.setup("Gateway Events", gateway_queue_size_default, 0, gateway_poll_stack_size, gateway_poll_event_priority)){
                logg << L::SL << Color::RED << "[GW] Can't initialize poll queue!" << L::EL;
                esp_websocket_client_destroy(client_handle);
                client_handle = nullptr;
                return false;                
            }


            if (esp_websocket_register_events(client_handle, WEBSOCKET_EVENT_ANY, &__c_gateway_redirect_event, (void*)&unique_cfg) != ESP_OK) {
                logg << L::SL << Color::RED << "[GW] Can't register event redirect!" << L::EL;
                esp_websocket_client_destroy(client_handle);
                client_handle = nullptr;
                unique_cfg.async_task.reset();
                return false;
            }

            if (esp_websocket_client_start(client_handle) != ESP_OK) { // start this
                logg << L::SL << Color::RED << "[GW] Can't start gateway task.!" << L::EL;
                esp_websocket_client_destroy(client_handle);
                client_handle = nullptr;
                unique_cfg.async_task.reset();
                return false;
            }
            
            logg << L::SL << Color::GREEN << "[GW] Gateway (re)started successfully!" << L::EL;
            return true;
        }

        bool GatewayControl::send_raw_json(const std::string& json_raw)
        {
            if (json_raw.empty()) return false;

            //std::lock_guard<std::mutex> luck(gateway_send_mtx);

            int sent_bytes = esp_websocket_client_send_text(client_handle, json_raw.c_str(), json_raw.size(), gateway_max_timeout / portTICK_PERIOD_MS);            

            if (sent_bytes != ESP_FAIL){
                const unsigned long long time_now = get_time_now_ms();
                
                if (!last_gateway_send) {
                    last_gateway_send = get_time_now_ms();
                }
                else if (time_now - last_gateway_send < 500) {
                    int time_more = 500 - static_cast<int>(time_now - last_gateway_send);
                    if (time_more > 0) vTaskDelay(time_more / portTICK_PERIOD_MS); // 2 events per second, gateway limits. This should be safe. // https://discord.com/developers/docs/topics/gateway#rate-limiting
                }

                last_gateway_send = time_now;

                return true;
            }

            logg << L::SL << Color::RED << "[GW] Send JSON failed with result #" << sent_bytes << L::EL;
            return false; // if failed, why care about gateway limits? ;P
        }

        bool GatewayControl::setup(const std::string& token, const int intents, std::function<void(const gateway_events&, const MemoryFileJSON&)> event_handlr)
        {
            logg << L::SL << Color::GREEN << "[GW] Starting..." << L::EL;
            
            {
                std::unique_lock<std::shared_mutex> safe_lock(unique_cfg.data_mtx);
                unique_cfg.token = token;
                unique_cfg.intents = intents;
            }

            {
                std::lock_guard<std::mutex> lucky(unique_cfg.func_mtx);
                unique_cfg.func = event_handlr;
            }

            if (!reconnect()){
                logg << L::SL << Color::RED << "[GW] Failed to start." << L::EL;
                return false;
            }

            gateway_thr_run = true;
            gateway_thr = std::thread([&]{gateway_handling();});

            // gateway ready.
            logg << L::SL << Color::GREEN << "[GW] Gateway is set up!" << L::EL;
            return true;
        }

        void GatewayControl::stop()
        {
            logg << L::SL << Color::YELLOW << "[GW] Closing..." << L::EL;
            
            if (gateway_thr_run){
                gateway_thr_run = false;
                close_gateway();
                if (gateway_thr.joinable()) gateway_thr.join();
            }            

            logg << L::SL << Color::GREEN << "[GW] Closed the gateway." << L::EL;
        }
        
        bool GatewayControl::is_connected_and_ready() const
        {
            return unique_cfg.ready_event_got;
        }

#ifdef LSW_ENABLE_AGGRESSIVE_ERROR
        void GatewayControl::force_restart_agressive_error()
        {
            unique_cfg.status = gateway_status::ERROR;
        }
#endif

        const std::string& GatewayControl::get_token() const
        {
            return unique_cfg.token;
        }

        unsigned long long GatewayControl::get_bot_id() const
        {
            return unique_cfg.bot_id;
        }

        const std::string& GatewayControl::get_bot_name() const
        {
            return unique_cfg.bot_string;
        }
        
        bool GatewayControl::send_command(const gateway_commands& cmd, const std::string& json_raw)
        {
            if (unique_cfg.status != gateway_status::CONNECTED) return false; // offline, reconnecting, something.

            const std::string json_to_send = 
            "{"
                "\"op\":" + std::to_string(static_cast<int>(cmd)) + ","
                "\"d\":" + json_raw +
            "}";

            if (!send_raw_json(json_to_send)) {
                logg << L::SL << Color::GREEN << "[GW] Failed to send command (custom command)!" << L::EL;
                return false;
            }
#ifdef LSW_SHOW_JSON_SENT_CONFIRMATION_SUCCESS
            logg << L::SL << Color::GREEN << "[GW] Sent JSON command successfully! (" << json_to_send.substr(0, 2048) << ")" << L::EL;
#endif
            return true;
        }

        
        void __c_gateway_redirect_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
        {
            unique_configuration* unique_cfg = (unique_configuration*)event_handler_arg;
            esp_websocket_event_data_t* data = (esp_websocket_event_data_t*)event_data;

            try{

 #ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
            logg << L::SL << Color::BRIGHT_GRAY << "[GW] Raw event: (event=" << event_id 
                                                                        << ", op_code=" << static_cast<int>(data->op_code)
                                                                        << ", payload_len=" << data->payload_len 
                                                                        << ", data_len=" << data->data_len
                                                                        << ", payload_offset=" << data->payload_offset << ")" << L::EL;
 #endif      

                if (unique_cfg->no_gateway_please){
                    switch(event_id){
                    case WEBSOCKET_EVENT_CLOSED:
                        unique_cfg->confirm_close = true;
                        unique_cfg->ready_event_got = false;
                        logg << L::SL << Color::GREEN << "[GW] (no_gateway_please) Got CLOSE event." << L::EL;
                        break;
                    case WEBSOCKET_EVENT_DISCONNECTED:
                    case WEBSOCKET_EVENT_ERROR:
                        unique_cfg->ready_event_got = false;
                        logg << L::SL << Color::YELLOW << "[GW] (no_gateway_please) Got ERROR event. Ignoring..." << L::EL;
                        break;
                    default:
                        logg << L::SL << Color::GREEN << "[GW] (no_gateway_please) Skipped a common event." << L::EL;
                        break;
                    }
                    return;
                }
                
                switch (event_id) {
                case WEBSOCKET_EVENT_CONNECTED:
                    unique_cfg->confirm_close = false;
                    logg << L::SL << Color::GREEN << "[GW] Confirmed CONNECTED Gateway event!" << L::EL;
                    return;
                case WEBSOCKET_EVENT_DATA:
                    if(data->op_code == WS_TRANSPORT_OPCODES_TEXT || data->op_code == WS_TRANSPORT_OPCODES_CLOSE) 
                    {
                        if (data->payload_offset == 0) unique_cfg->event_buffer_fp.clear();
                        const bool theend = data->payload_len == data->payload_offset + data->data_len;
                        unique_cfg->event_buffer_fp.append(data->data_ptr, data->data_len);

#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                        if (theend) {
                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] EVENT is COMPLETE. Tasking Event..." << L::EL;
                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON size match [" << unique_cfg->event_buffer_fp.size() << " == " << data->payload_len << "]? (no testing, just report)" << L::EL;
                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON:\n" << unique_cfg->event_buffer_fp.substr(0, 2048) << (unique_cfg->event_buffer_fp.size() > 2048 ? "..." : "") << L::EL;
                        }
#endif

                        if (theend) {
#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] EVENTID: " << static_cast<int>(event_id) << "; JSON: " << (unique_cfg->event_buffer_fp.empty() ? "NULL" : (unique_cfg->event_buffer_fp.substr(0, 4096) + ((unique_cfg->event_buffer_fp.size() > 4096) ? "..." : ""))) << L::EL;
#endif 
                            unique_cfg->gw_data_temp = std::make_shared<gateway_payload_structure>(std::move(unique_cfg->event_buffer_fp));

                            // only moves if successful.
                            if (!unique_cfg->gw_data_temp->all_good) {
                                //logg << L::SL << Color::RED << "[GW] WEBSOCKET_EVENT_DATA can't handle JSON! Restarting ESP32 soon! (" << (unique_cfg->gw_data_temp->__fporig.empty() ? "EMPTY|NULL" : (unique_cfg->gw_data_temp->__fporig.substr(0, 2048) + (unique_cfg->gw_data_temp->__fporig.size() > 2048 ? "..." : ""))) << ")" << L::EL;
                                logg << L::SL << Color::RED << "[GW] WEBSOCKET_EVENT_DATA can't handle JSON! Trying to recover via pure restart method!" << L::EL;
                                
                                unique_cfg->status = gateway_status::ERROR;
                                unique_cfg->confirm_close = true;
                                unique_cfg->ready_event_got = false;

                                return;

                                /*vTaskDelay(2000 / portTICK_PERIOD_MS); // no rush
                                __c_gateway_got_unhandled_disconnect_so_restart("__c_gateway_redirect_event could not handle JSON."); // sorry ma'am*/
                            }
                            unique_cfg->event_buffer_fp.clear();
                            // good.
                            
#ifdef LSW_GATEWAY_DEBUGGING_AS_INFO
                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON PARSED SUCCESSFULLY (op:" << static_cast<int>(unique_cfg->gw_data_temp->op) << ")" << L::EL;
#endif 

                            if (unique_cfg->gw_data_temp->s >= 0) {
                                std::unique_lock<std::shared_mutex> safe_lock(unique_cfg->data_mtx);
                                unique_cfg->sequence_number = unique_cfg->gw_data_temp->s;
                            }

                            switch(unique_cfg->gw_data_temp->op) {
                            case gateway_opcodes::UNKNOWN:                // -1 [MINE]           // Default creation, not related to Discord.
                                logg << L::SL << Color::GREEN << "[GW] Got invalid OP code?!. Skipping... (" << (unique_cfg->gw_data_temp->__fporig.empty() ? "EMPTY|NULL" : (unique_cfg->gw_data_temp->__fporig.substr(0, 2048) + (unique_cfg->gw_data_temp->__fporig.size() > 2048 ? "..." : ""))) << ")" << L::EL;
                                return;
                            case gateway_opcodes::DISPATCH:               //  0 [RECEIVE]        // An event was dispatched.
                            {
                                // THIS IS IT BOIS!
                                switch(unique_cfg->gw_data_temp->t) {
                                case gateway_events::HELLO:                          // Defines the heartbeat interval
                                    logg << L::SL << Color::GREEN << "[GW] Discord sent a HELLO 'DISPATCH' event!" << L::EL;          // [!!!!!] don't manage here. Manage the OPCode one.
                                    break;
                                case gateway_events::READY:                          // Contains the initial state information
                                    logg << L::SL << Color::GREEN << "[GW] Ready event received!" << L::EL;
                                    {
                                        std::unique_lock<std::shared_mutex> safe_lock(unique_cfg->data_mtx);
                                        bool all_good = true;

                                        {
                                            if (/*MemoryFileJSON*/ const auto it = unique_cfg->gw_data_temp->json["d"]["session_id"]; !it.is_null()) unique_cfg->session_id = it.as_string();
                                            else {
                                                logg << L::SL << Color::RED << "[GW] READY's \"d\"/\"session_id\" was null?!" << L::EL;
                                                all_good = false;
                                            }
                                            if (/*MemoryFileJSON*/ const auto it = unique_cfg->gw_data_temp->json["d"]["user"]["id"]; !it.is_null()) unique_cfg->bot_id = it.as_llu();
                                            else {
                                                logg << L::SL << Color::RED << "[GW] READY's \"d\"/\"user\"/\"id\" was null?!" << L::EL;
                                                all_good = false;
                                            }

                                            /*MemoryFileJSON*/ const auto j_nam = unique_cfg->gw_data_temp->json["d"]["user"]["username"];
                                            /*MemoryFileJSON*/ const auto j_dsc = unique_cfg->gw_data_temp->json["d"]["user"]["discriminator"];

                                            if (!j_nam.is_null() && !j_dsc.is_null()) unique_cfg->bot_string = j_nam.as_string() + "#" + j_dsc.as_string();
                                            else {
                                                logg << L::SL << Color::RED << "[GW] READY's \"d\"/\"user\"/\"username\" or \"d\"/\"user\"/\"discriminator\" was null?!" << L::EL;
                                                all_good = false;
                                            }
                                        }

                                        if (!all_good){
                                            logg << L::SL << Color::RED << "[GW] Ready event had errors! Restarting ESP32..." << L::EL;
                                            logg << L::SL << Color::RED << "[GW] JSON was: " << unique_cfg->gw_data_temp->__fporig.substr() << L::EL;
                                            __c_gateway_got_unhandled_disconnect_so_restart("__c_gateway_redirect_event got invalid READY data."); // what if there is a bug, for real?
                                        }
                                        else{
                                            logg << L::SL << Color::GREEN << "[GW] Connected successfully with information!" << L::EL;
                                            logg << L::SL << Color::GREEN << "[GW] Bot: " << unique_cfg->bot_string << "; BotID: " << std::to_string(unique_cfg->bot_id) << L::EL;// << "; SessionID: " << unique_cfg->session_id << L::EL;
                                            unique_cfg->ready_event_got = true;
                                        }
                                    }
                                    break;
                                case gateway_events::RESUMED:                        // Response to Resume (if good)
                                    logg << L::SL << Color::GREEN << "[GW] Dispatch got RESUMED successfully." << L::EL;
                                    unique_cfg->resumed_successfully = true;
                                    break;
                                case gateway_events::RECONNECT:                      // Server is going away, client should reconnect to gateway and resume
                                    logg << L::SL << Color::YELLOW << "[GW] Dispatch got RECONNECT." << L::EL;          // [!!!!!] don't manage here. Manage the OPCode one.
                                    unique_cfg->status = gateway_status::RECONNECT; // update, reconnect, resume
                                    break;
                                case gateway_events::INVALID_SESSION:                // Failure response to Identify or Resume or invalid active session
                                    logg << L::SL << Color::YELLOW << "[GW] Dispatch got INVALID_SESSION! Trying RESUME..." << L::EL;
                                    if (/*MemoryFileJSON*/ const auto it = unique_cfg->gw_data_temp->json["d"]; !it.is_null()) unique_cfg->resumed_successfully = it.as_bool(); // d is true if RESUMEable
                                    unique_cfg->status = gateway_status::RECONNECT; // reconnect (resume)
                                    break;
                                
                                default:
                                    break;
                                }
                                
                                {
                                    std::lock_guard<std::mutex> lucky(unique_cfg->func_mtx);
                                    if (unique_cfg->func) {
                                        
#ifdef LSW_SHOW_DETAILED_EVENTS
                                        logg << L::SL << Color::BLUE << "[GW] Adding one async task..." << L::EL;
#endif

#ifdef LSW_SHOW_JSON_BEING_SENT
                                        logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON being posted:\n" << (unique_cfg->gw_data_temp->__fporig.empty() ? "EMPTY|NULL" : (unique_cfg->gw_data_temp->__fporig.substr(0, 2048) + (unique_cfg->gw_data_temp->__fporig.size() > 2048 ? "..." : ""))) << L::EL;
#endif

                                        unique_cfg->async_task.post([&tasking_hint = unique_cfg->tasking_hint, &func = unique_cfg->func, gww = unique_cfg->gw_data_temp]{
#ifdef LSW_SHOW_JSON_BEING_SENT
                                            logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON got posted:\n" << (gww->__fporig.empty() ? "EMPTY|NULL" : (gww->__fporig.substr(0, 2048) + (gww->__fporig.size() > 2048 ? "..." : ""))) << L::EL;
                                            const auto jj = gww->json["d"];
                                            //logg << L::SL << Color::BRIGHT_GRAY << "[GW] JSON [\"d\"]:\n" << (jj.empty() ? "EMPTY|NULL" : jj.get_memfp().substr(0, 2048)) << L::EL;
#endif                                      
                                            tasking_hint++;
                                            func(gww->t, gww->json["d"]);
                                            if (tasking_hint > 0) tasking_hint--;
                                        });
                                        
#ifdef LSW_SHOW_DETAILED_EVENTS
                                        logg << L::SL << Color::BLUE << "[GW] Added one async task. Tasks to do: " << unique_cfg->async_task.queue_size_now() << L::EL;
#endif
                                    }
                                    else {
                                        logg << L::SL << Color::YELLOW << "[GW] No function set to handle events. Skipped event." << L::EL;
                                    }
                                }
                            }
                                break;
                            case gateway_opcodes::HEARTBEAT:              //  1 [SEND/RECEIVE]   // Fired periodically by the client to keep the connection alive.
                            {
                                std::unique_lock<std::shared_mutex> safe_lock(unique_cfg->data_mtx);
                                unique_cfg->heartbeat_at = 0; // causes handling thread to send heartbeat (discord is asking for heartbeat)
                            }    
                                break;
                            case gateway_opcodes::RECONNECT:              //  6 [RECEIVE]        // You should attempt to reconnect and resume immediately.
                            {
                                logg << L::SL << Color::YELLOW << "[GW] Got RECONNECT event, trying to RESUME..." << L::EL;                                
                                unique_cfg->status = gateway_status::RECONNECT; // other thread should reconnect
                            }
                                break;
                            case gateway_opcodes::INVALID_SESSION:        //  8 [RECEIVE]        // The session has been invalidated. You should reconnect and identify/resume accordingly.
                            {
                                logg << L::SL << Color::YELLOW << "[GW] Got INVALID_SESSION event, trying to RESUME..." << L::EL;
                                unique_cfg->status = gateway_status::ERROR; // other thread should reconnect
                                unique_cfg->ready_event_got = false;
                            }
                                break;
                            case gateway_opcodes::HELLO:                  //  9 [RECEIVE]        // Sent immediately after connecting, contains the heartbeat_interval to use.
                            {
                                logg << L::SL << Color::YELLOW << "[GW] Got HELLO event, introducing itself..." << L::EL;
                                if (/*MemoryFileJSON*/ const auto it = unique_cfg->gw_data_temp->json["d"]["heartbeat_interval"]; !it.is_null()){
                                    std::unique_lock<std::shared_mutex> safe_lock(unique_cfg->data_mtx);

                                    unique_cfg->heartbeat_at = 0; // new
                                    unique_cfg->heartbeat_interval = it.as_int();
                                    //unique_cfg->ack_heartbeat = get_time_now_ms(); // no error please.
                                    
                                    if (unique_cfg->heartbeat_interval < 500) {
                                        logg << L::SL << Color::RED << "[GW] Heartbeat wasn't good (" << unique_cfg->heartbeat_interval << ")?! Restarting ESP32..." << L::EL;
                                        logg << L::SL << Color::RED << "[GW] JSON: " << (unique_cfg->gw_data_temp->__fporig.empty() ? "EMPTY|NULL" : (unique_cfg->gw_data_temp->__fporig.substr(0, 2048) + (unique_cfg->gw_data_temp->__fporig.size() > 2048 ? "..." : ""))) << L::EL;
                                        vTaskDelay(500 / portTICK_PERIOD_MS); // no rush
                                        __c_gateway_got_unhandled_disconnect_so_restart("Couldn't handle HELLO somehow."); // dies.
                                    }
                                    unique_cfg->status = gateway_status::STARTUP;
                                    logg << L::SL << Color::GREEN << "[GW] HELLO is GOOD with Heartbeat interval at " << unique_cfg->heartbeat_interval << "!" << L::EL;
                                }
                                else {
                                    logg << L::SL << Color::RED << "[GW] HELLO was corrupted?! Restarting ESP32..." << L::EL;
                                    logg << L::SL << Color::RED << "[GW] JSON: " << (unique_cfg->gw_data_temp->__fporig.empty() ? "EMPTY|NULL" : (unique_cfg->gw_data_temp->__fporig.substr(0, 2048) + (unique_cfg->gw_data_temp->__fporig.size() > 2048 ? "..." : ""))) << L::EL;
                                    vTaskDelay(1000 / portTICK_PERIOD_MS); // no rush
                                    __c_gateway_got_unhandled_disconnect_so_restart("Couldn't handle HELLO somehow."); // dies.
                                }

                            }
                                break;
                            case gateway_opcodes::HEARTBEAT_ACK:          // 10 [RECEIVE]        // Sent in response to receiving a heartbeat to acknowledge that it has been received.
#ifdef LSW_SHOW_DETAILED_EVENTS
                                logg << L::SL << Color::GREEN << "[GW] Heartbeat ACK got!" << L::EL;
#endif
                                unique_cfg->ack_heartbeat = true;// get_time_now_ms();
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    return;            
                case WEBSOCKET_EVENT_ERROR:
                    logg << L::SL << Color::RED << "[GW] CONNECTION ERROR! Trying the restart pure method." << L::EL;
                    unique_cfg->status = gateway_status::ERROR;
                    unique_cfg->confirm_close = true;
                    unique_cfg->ready_event_got = false;
                    /*unique_cfg->status = gateway_status::ERROR;
                    unique_cfg->ready_event_got = false;
                    __c_gateway_got_unhandled_disconnect_so_restart("Gateway got ERROR event"); // dies.*/
                    return;
                case WEBSOCKET_EVENT_DISCONNECTED:
                    logg << L::SL << Color::RED << "[GW] DISCONNECTED! Trying the restart pure method..." << L::EL;
                    unique_cfg->status = gateway_status::CLOSED;
                    unique_cfg->confirm_close = true;
                    unique_cfg->ready_event_got = false;
                    return;
                case WEBSOCKET_EVENT_CLOSED:
                    logg << L::SL << Color::RED << "[GW] CONNECTION CLOSED! Trying the restart pure method..." << L::EL;
                    unique_cfg->status = gateway_status::CLOSED;
                    unique_cfg->confirm_close = true;
                    unique_cfg->ready_event_got = false;
                    /*unique_cfg->status = gateway_status::RECONNECT;
                    unique_cfg->confirm_close = true;
                    unique_cfg->ready_event_got = false;*/
                    return;                
                default:
                    logg << L::SL << Color::YELLOW << "[GW] Unknown Gateway event. Skipping." << L::EL;
                    return;
                }
            }
            catch(const std::exception& e)
            {
                logg << L::SL << Color::RED << "[GW] Fatal exception: " << e.what() << ". Trying to recover..." << L::EL;
                //vTaskDelay(1000 / portTICK_PERIOD_MS); // no need to go fast.
                unique_cfg->status = gateway_status::ERROR;
            }
            catch(...)
            {
                logg << L::SL << Color::RED << "[GW] Fatal exception: (UNCAUGHT). Trying to recover..." << L::EL;
                //vTaskDelay(1000 / portTICK_PERIOD_MS); // no need to go fast.
                unique_cfg->status = gateway_status::ERROR;
            }
        }

        void __c_gateway_got_unhandled_disconnect_so_restart(const std::string& reason)
        {            
            logg << L::SL << Color::RED << "[GW] __c_gateway_got_unhandled_disconnect_so_restart called restart for this reason: \"" << reason << "\". Restarting ESP32..." << L::EL;
            vTaskDelay(500 / portTICK_PERIOD_MS); // no need to go fast.
            esp_restart(); // shutdown and reset.
        }

    }
}