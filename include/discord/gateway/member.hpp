#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "discord/raw/http_interface.hpp"
#include "discord/core.hpp"
#include "discord/gateway/user.hpp"

namespace LSW {
    namespace PocketDiscord {

        inline const char MemberTAG[] = "Member";
        
        // based on Discord @ https://discord.com/developers/docs/resources/guild#guild-member-object-guild-member-structure
        class Member {
            BotCore& core;

            User user; // not included on MESSAGE_*
            std::string nick; // may not be set/send
            std::vector<unsigned long long> roles;
            std::string joined_at;
            bool pending = true; // accepted rules? false if accepted. Assume not accepted?!
            std::string permissions; // may not be set (optional)

            std::string gen_roles_str() const;
        public:
            Member(BotCore&);
            Member(const Member&);
            // bool: Messages doesn't have USER, but AUTHOR, so error check doesn't work! Set TRUE if SUPRESS UserID == 0
            bool load_from_json(const MemoryFileJSON&, const bool = false);
            
            const User& get_user() const;
            const std::string& get_nick() const;
            const std::vector<unsigned long long>& get_roles() const;
            bool get_roles_one(const unsigned long long&) const;
            const std::string& get_joined_at() const;
            bool get_pending() const;
            const std::string& get_permissions() const;

            User& get_user_ref();
            void set_nick(const std::string&);
            std::vector<unsigned long long>& get_roles_ref();
            void set_permissions(const std::string&);
            
            // based on Discord message related calls

            // setup a member, guild & user id, (load full member (true) or just set these vars (false)?)
            bool load_member(const unsigned long long, const unsigned long long, const bool = true);

            // updates member with defined variables (at this guild)
            std::future<request_response> update_member_at_guild(const unsigned long long);
            // changes their nick in guild. If string is null, updates nick to current set
            std::future<request_response> modify_member_nick(const unsigned long long, const std::string& = "");
            // adds role to member in guild
            std::future<request_response> add_member_role(const unsigned long long, const unsigned long long);
            // remove role to member in guild
            std::future<request_response> remove_member_role(const unsigned long long, const unsigned long long);
            // remove member from guild (kick)
            std::future<request_response> kick_member(const unsigned long long);
            // ban member from guild (+ reason, delete how many days of messages)
            std::future<request_response> ban_member(const unsigned long long, const std::string& = "The hammer has spoken", const int = 0);
            // unban member
            std::future<request_response> unban_member(const unsigned long long);

        };

    }
}