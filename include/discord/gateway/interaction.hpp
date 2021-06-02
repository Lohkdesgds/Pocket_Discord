#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <optional>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/member.hpp"
#include "discord/gateway/user.hpp"
#include "discord/gateway/message.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        // Things based on Discord @ (somewhere there) https://discord.com/developers/docs/interactions/slash-commands#interaction

        enum class interation_type {
            PING                        = 1, // 
            APPLICATION_COMMAND         = 2, // Main /command thingy
            MESSAGE_COMPONENT           = 3  // 
        };

        enum class interaction_type_response {
            PONG                                    = 1, // ACK a Ping
            CHANNEL_MESSAGE_WITH_RESPONSE           = 4, // Respond to an interaction with a Message
            DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE    = 5, // ACK an interaction and edit a response later, the user sees a loading state
            DEFERRED_UPDATE_MESSAGE                 = 6, // for components, ACK an interaction and edit the original message later; the user does not see a loading state
            UPDATE_MESSAGE                          = 7  // for components, edit the message the component was attached to
        };

        // user creating only
        class InteractionApplicationCommandCallbackData { // like Message
            bool tts = false;
            std::string content;
            std::vector<Embed> embeds;
            int flags = 0;
        public:
            std::string to_json() const;

            InteractionApplicationCommandCallbackData& set_tts(const bool);
            InteractionApplicationCommandCallbackData& set_content(const std::string&);
            InteractionApplicationCommandCallbackData& set_flags(const int);
            InteractionApplicationCommandCallbackData& add_embed(Embed&&);

            bool get_tts() const;
            const std::string& get_content() const;
            const std::vector<Embed>& get_embed() const;
            int get_flags() const;
            std::vector<Embed>& get_embed_ref();
        };

        // user creating only
        class InteractionResponse {
            interaction_type_response type = interaction_type_response::CHANNEL_MESSAGE_WITH_RESPONSE;
            std::optional<InteractionApplicationCommandCallbackData> data;
        public:
            std::string to_json() const;

            void set_type(const interaction_type_response);
            InteractionApplicationCommandCallbackData& generate_data();
        };

        class Interaction {
            BotCore& core;

            unsigned long long id = 0; // necessary
            std::string token; // necessary
            unsigned long long application_id = 0; // hmm
            interation_type type = interation_type::PING;
            std::shared_ptr<MemoryFileJSON> data; // just do it manually.
            unsigned long long guild_id = 0;
            unsigned long long channel_id = 0;
            Member member; // who triggered
            Message message; // for components
            bool was_command = false; // adapted, for interaction_name
            std::string interation_name; // if "name" is available, name, else "custom_id" (adapted)
        public:
            Interaction(BotCore&);
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            const std::string& get_token() const;
            unsigned long long get_application_id() const;
            interation_type get_type() const;
            const MemoryFileJSON& get_data() const;
            unsigned long long get_guild_id() const;
            unsigned long long get_channel_id() const;
            const Member& get_member() const;
            const User& get_user() const;
            const Message& get_message() const;
            bool get_was_interaction_command() const; // adapted, if /command
            const std::string& get_interaction_name() const; // adapted, /command or [command] (button)

            // based on Discord interaction related calls

            std::future<request_response> create_response(const InteractionResponse&) const;
            // deletes message trigger (I think this only works with components)
            std::future<request_response> delete_trigger() const;
        };

    }
}