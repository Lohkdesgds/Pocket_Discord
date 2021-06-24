#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/member.hpp"
#include "discord/gateway/user.hpp"
#include "discord/gateway/attachment.hpp"
#include "discord/gateway/embed.hpp"
#include "discord/gateway/emoji.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char MessageTAG[] = "Message";

        enum class message_type {
            DEFAULT                                       = 0,  // Discord don't have description, I'll put something here later lmao
            RECIPIENT_ADD                                 = 1,  // Discord don't have description, I'll put something here later lmao
            RECIPIENT_REMOVE                              = 2,  // Discord don't have description, I'll put something here later lmao
            CALL                                          = 3,  // Discord don't have description, I'll put something here later lmao
            CHANNEL_NAME_CHANGE                           = 4,  // Discord don't have description, I'll put something here later lmao
            CHANNEL_ICON_CHANGE                           = 5,  // Discord don't have description, I'll put something here later lmao
            CHANNEL_PINNED_MESSAGE                        = 6,  // Discord don't have description, I'll put something here later lmao
            GUILD_MEMBER_JOIN                             = 7,  // Discord don't have description, I'll put something here later lmao
            USER_PREMIUM_GUILD_SUBSCRIPTION               = 8,  // Discord don't have description, I'll put something here later lmao
            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1        = 9,  // Discord don't have description, I'll put something here later lmao
            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2        = 10, // Discord don't have description, I'll put something here later lmao
            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3        = 11, // Discord don't have description, I'll put something here later lmao
            CHANNEL_FOLLOW_ADD                            = 12, // Discord don't have description, I'll put something here later lmao
            GUILD_DISCOVERY_DISQUALIFIED                  = 14, // Discord don't have description, I'll put something here later lmao
            GUILD_DISCOVERY_REQUALIFIED                   = 15, // Discord don't have description, I'll put something here later lmao
            GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING  = 16, // Discord don't have description, I'll put something here later lmao
            GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING    = 17, // Discord don't have description, I'll put something here later lmao
            THREAD_CREATED                                = 18, // Discord don't have description, I'll put something here later lmao
            REPLY                                         = 19, // Discord don't have description, I'll put something here later lmao
            APPLICATION_COMMAND                           = 20, // Discord don't have description, I'll put something here later lmao
            THREAD_STARTER_MESSAGE                        = 21, // Discord don't have description, I'll put something here later lmao
            GUILD_INVITE_REMINDER                         = 22  // Discord don't have description, I'll put something here later lmao
        };

        enum class component_type {
            ACTION_ROW      = 1, // can have one or many buttons
            BUTTON          = 2  // a button
        };

        enum class button_style {
            PRIMARY         = 1, // blurple                     REQUIRE: custom_id
            SECONDARY       = 2, // grey                        REQUIRE: custom_id
            SUCCESS         = 3, // green                       REQUIRE: custom_id
            DANGER          = 4, // red                         REQUIRE: custom_id
            LINK            = 5  // grey, navigates to a URL    REQUIRE: url
        };

        class GenericComponentObject {
            component_type type = component_type::BUTTON;
        protected:
            void set_type(const component_type&);
            GenericComponentObject(const component_type&);
        public:
            component_type get_type() const;
        };

        // based on Discord @ https://discord.com/developers/docs/interactions/message-components#component-object
        class ButtonObject : public GenericComponentObject {
            button_style style = button_style::PRIMARY;
            std::string label; // optional
            Emoji emoji; // optional
            std::string custom_id; // sometimes optional (no url)
            std::string url; // sometimes optional (no custom_id)
            bool disabled = false;
        public:
            ButtonObject();
            ButtonObject(const ButtonObject&);
            ButtonObject(ButtonObject&&);
            void operator=(const ButtonObject&);
            void operator=(ButtonObject&&);

            bool load_from_json(const MemoryFileJSON&);
            std::string to_json() const;

            button_style get_style() const;
            const std::string& get_label() const;
            const Emoji& get_emoji() const;
            const std::string& get_custom_id() const;
            const std::string& get_url() const;
            bool get_disabled() const;

            ButtonObject& set_style(const button_style);
            ButtonObject& set_label(const std::string&);
            ButtonObject& set_emoji(const Emoji&);
            ButtonObject& set_custom_id(const std::string&);
            ButtonObject& set_url(const std::string&);
            ButtonObject& set_disabled(const bool);
        };

        // based on Discord @ https://discord.com/developers/docs/interactions/message-components#actionrow
        class ActionRow : public GenericComponentObject {
            std::vector<ButtonObject> components;
        public:
            ActionRow();
            ActionRow(const ActionRow&);
            ActionRow(ActionRow&&);
            void operator=(const ActionRow&);
            void operator=(ActionRow&&);

            bool load_from_json(const MemoryFileJSON&);
            std::string to_json() const;

            bool add(const ButtonObject&);
            const std::vector<ButtonObject>& get() const;
            std::vector<ButtonObject>& get_ref();
        };

        class GenericComponentObjectPTR {
            GenericComponentObject* generic = nullptr;
            void destroy();
        public:
            ~GenericComponentObjectPTR();
            GenericComponentObjectPTR() = default;
            GenericComponentObjectPTR(GenericComponentObject*);
            GenericComponentObjectPTR(const GenericComponentObjectPTR&) = delete;
            GenericComponentObjectPTR(GenericComponentObjectPTR&&);

            bool gen_button();
            bool gen_actionrow();

            GenericComponentObject* as_generic();

            ButtonObject* as_button();
            ActionRow* as_actionrow();

            const ButtonObject* as_button() const;
            const ActionRow* as_actionrow() const;

            component_type get_type() const;
        };
        
        // based on Discord @ https://discord.com/developers/docs/resources/channel#message-object
        class Message {
            BotCore& core;

            unsigned long long id = 0;
            unsigned long long channel_id = 0;
            unsigned long long guild_id = 0; // optional
            Member member; // author is used here.
            std::string content;
            std::string timestamp;
            std::string edited_timestamp; // optional
            bool mentions_everyone = false;
            std::vector<unsigned long long> mentions; // (user) -> id
            std::vector<unsigned long long> mention_roles; // direct
            std::vector<unsigned long long> mention_channels; // (channel mention) -> id
            std::vector<Attachment> attachments;
            std::vector<Embed> embeds;
            std::vector<GenericComponentObjectPTR> components; // slash thingy new stuff, may change in the future
            std::shared_ptr<Message> referenced_message; // valid on threads or reply
            message_type type = message_type::DEFAULT;
        public:
            Message(BotCore&);
            bool load_from_json(const MemoryFileJSON&);

            unsigned long long get_id() const;
            unsigned long long get_channel_id() const;
            unsigned long long get_guild_id() const;
            const User& get_author() const;
            const Member& get_member() const;
            const std::string& get_content() const;
            const std::string& get_timestamp() const;
            const std::string& get_edited_timestamp() const;
            bool get_mentions_everyone() const;
            const std::vector<unsigned long long>& get_mentions() const;
            bool get_mentions_one(const unsigned long long&);
            const std::vector<unsigned long long>& get_mention_roles() const;
            bool get_mention_roles_one(const unsigned long long&);
            const std::vector<unsigned long long>& get_mention_channels() const;
            bool get_mention_channels_one(const unsigned long long&);
            const std::vector<Attachment>& get_attachments() const;
            const std::vector<Embed>& get_embeds() const;
            const std::vector<GenericComponentObjectPTR>& get_components() const;
            const std::shared_ptr<Message> get_referenced_message() const;
            message_type get_type() const;

            Message& set_id(const unsigned long long);
            Message& set_channel_id(const unsigned long long);
            Message& set_guild_id(const unsigned long long);
            Message& set_content(const std::string&);
            std::vector<Attachment> get_attachments_ref();
            std::vector<Embed>& get_embeds_ref();
            std::vector<GenericComponentObjectPTR>& get_components_ref();


            // based on Discord message related calls

            // deletes THIS message:
            std::future<request_response> delete_message();
            // create a answer to THIS message:
            std::future<request_response> create_message_answer(const std::string&);
            // create a answer to THIS message: (with embed)
            std::future<request_response> create_message_answer(const std::string&, const Embed&);
            // create a message in this same channel
            std::future<request_response> create_message_same_channel(const std::string&);
            // create a message in this same channel (with embed)
            std::future<request_response> create_message_same_channel(const std::string&, const Embed&);
            // send an exact copy of this message in the same channel in the same guild with embeds and components.
            std::future<request_response> create_this_message();
            // edit a message with this ID with current data in it
            std::future<request_response> edit_this_message_as_is();
            
            // to add.
            /*// create a reaction to the message (what emoji)
            std::future<request_response> create_reaction(const std::string&);
            // delete own reaction (what emoji)
            std::future<request_response> delete_own_reaction(const std::string&);
            // delete someone's reaction (who, what emoji)
            std::future<request_response> delete_user_reaction(const unsigned long long&, const std::string&);
            // get users that reacted this (opt: after what user id, opt: max reactions list size)
            std::future<request_response> get_reactions(const unsigned long long& = 0, const int& = 25);
            // remove all reactions in message
            std::future<request_response> delete_all_reactions();
            // delete all reactions of that emoji. (emoji)
            std::future<request_response> delete_all_reactions_for_emoji(const std::string&);
            // edit message with this
            std::future<request_response> edit_message(const std::string&, const std::string& = "");*/
        };
    }
}