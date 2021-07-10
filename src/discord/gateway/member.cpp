#include "discord/gateway/member.hpp"

namespace LSW {
    namespace PocketDiscord {

        std::string Member::gen_roles_str() const
        {
            std::string str;
            for(auto& i : roles){
                str += "\"" + std::to_string(i) + "\",";
            }
            if (str.length() > 0) str.pop_back();
            return str;
        }

        Member::Member(BotCore& othcore)
            : core(othcore)
        {
        }
        
        Member::Member(const Member& oth)
            : core(oth.core), user(oth.user), nick(oth.nick), roles(oth.roles), joined_at(oth.joined_at), pending(oth.pending), permissions(oth.permissions)
        {

        }

        bool Member::load_from_json(const MemoryFileJSON& json, const bool supress_user_err)
        {
            if (json.is_empty()) {
                ESP_LOGE(MemberTAG, "Member::load_from_json parsing failed! JSON was null!");
                return false;
            }
            
            if (/*MemoryFileJSON*/ const auto it = json["nick"]; !it.is_null())            nick = handle_utf16(it.as_string());
            if (/*MemoryFileJSON*/ const auto it = json["joined_at"]; !it.is_null())       joined_at = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["pending"]; !it.is_null())         pending = it.as_bool();
            if (/*MemoryFileJSON*/ const auto it = json["permissions"]; !it.is_null())     permissions = it.as_string();
            if (/*MemoryFileJSON*/ const auto it = json["guild_id"]; !it.is_null())        guild_id = it.as_llu();

            if (/*MemoryFileJSON*/ const auto it = json["user"]; !it.is_null()) {
                if (!user.load_from_json(it) && !supress_user_err) ESP_LOGW(MemberTAG, "Member::load_from_json parsing failed! User failed somewhere while parsing.");
            }

            if (/*MemoryFileJSON*/ const auto it = json["roles"]; !it.is_null() && it.is_array()) {
                /*const size_t arr_len = it.array_length();
                for (size_t p = 0; p < arr_len; p++) {*/
                for(const auto& inn : it) {
                    //MemoryFileJSON inn = it[p];
                    //if (inn.is_null()) break;
                    unsigned long long plusid = inn.as_llu();
                    if (!plusid) continue;
                    roles.push_back(plusid);
                }
            }

            return supress_user_err ? true : user.get_id() != 0;
        }

        const User& Member::get_user() const
        {
            return user;
        }

        const std::string& Member::get_nick() const
        {
            return nick;
        }
        
        const std::vector<unsigned long long>& Member::get_roles() const
        {
            return roles;
        }

        bool Member::get_roles_one(const unsigned long long& roleid) const
        {
            return std::find(roles.begin(), roles.end(), roleid) != roles.end();
        }
        
        const std::string& Member::get_joined_at() const
        {
            return joined_at;
        }
        
        bool Member::get_pending() const
        {
            return pending;
        }
        
        const std::string& Member::get_permissions() const
        {
            return permissions;
        }
        
        unsigned long long Member::get_guild_id() const
        {
            return guild_id;
        }
        
        User& Member::get_user_ref()
        {
            return user;
        }
        
        void Member::set_nick(const std::string& arg)
        {
            nick = arg;
        }

        std::vector<unsigned long long>& Member::get_roles_ref()
        {
            return roles;
        }
        
        void Member::set_permissions(const std::string& arg)
        {
            permissions = arg;
        }
        

        // setup a member, guild & user id, (load full member (true) or just set these vars (false)?)
        bool Member::load_member(const unsigned long long nid, const unsigned long long gid, const bool fullload)
        {
            if (nid == 0 || (fullload && gid == 0)) return false;

            user.set_id(nid);

            if (!fullload){
                return true;
            }

            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(nid);
            const http_request method = http_request::GET;
            const std::string json = "";

            std::future<request_response> req = core.post_task(request, method, json);

            req.wait();

            request_response res = req.get();

            return load_from_json(res.mj) && user.get_id();
        }

        // updates member with defined variables (at this guild)
        std::future<request_response> Member::update_member_at_guild(const unsigned long long gid)
        {
            if (!gid || !user.get_id()) return fake_future<request_response>();

            const std::string rolestr = gen_roles_str();
            
            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(user.get_id());
            const http_request method = http_request::PATCH;
            const std::string json = 
            "{"
                "\"nick\":\"" + fix_quotes_string_for_json(nick) + "\"" +
                (rolestr.empty() ? "" : (",\"roles\":[" + rolestr + "]")) +
                // later add support for voice things?
            "}";

            return core.post_task(request, method, json);
        }

        // changes their nick in guild. If string is null, updates nick to current set
        std::future<request_response> Member::modify_member_nick(const unsigned long long gid, const std::string& newnick)
        {
            if (!gid || !user.get_id()) return fake_future<request_response>();

            nick = newnick;
            
            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(user.get_id());
            const http_request method = http_request::PATCH;
            const std::string json = 
            "{"
                "\"nick\":\"" + fix_quotes_string_for_json(nick) + "\""
            "}";

            return core.post_task(request, method, json);
        }
        
        // adds role to member in guild
        std::future<request_response> Member::add_member_role(const unsigned long long gid, const unsigned long long roleid)
        {
            if (!gid || !user.get_id() || !roleid) return fake_future<request_response>();

            const auto it = std::find(roles.begin(), roles.end(), roleid);
            if (it != roles.end()) return fake_future<request_response>(); // already have this role!

            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(user.get_id()) + "/roles/" + std::to_string(roleid);
            const http_request method = http_request::PUT;
            const std::string json = "";

            roles.push_back(roleid);

            return core.post_task(request, method, json);
        }
        
        // remove role to member in guild
        std::future<request_response> Member::remove_member_role(const unsigned long long gid, const unsigned long long roleid)
        {
            if (!gid || !user.get_id() || !roleid) return fake_future<request_response>();

            const auto it = std::find(roles.begin(), roles.end(), roleid);
            if (it == roles.end()) return fake_future<request_response>(); // has no role!

            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(user.get_id()) + "/roles/" + std::to_string(roleid);
            const http_request method = http_request::DELETE;
            const std::string json = "";

            roles.erase(it);

            return core.post_task(request, method, json);
        }
        
        // remove member from guild (kick)
        std::future<request_response> Member::kick_member(const unsigned long long gid)
        {
            if (!gid || !user.get_id()) return fake_future<request_response>();

            const std::string request = "/guilds/" + std::to_string(gid) + "/members/" + std::to_string(user.get_id());
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        // ban member from guild (+ reason, delete how many days of messages)
        std::future<request_response> Member::ban_member(const unsigned long long gid, const std::string& reason, const int days_delete)
        {
            if (!gid || !user.get_id() || reason.empty() || (days_delete < 0 || days_delete > 7)) return fake_future<request_response>();

            const std::string request = "/guilds/" + std::to_string(gid) + "/bans/" + std::to_string(user.get_id());
            const http_request method = http_request::PUT;
            const std::string json = 
            "{"
                "\"delete_message_days\":" + std::to_string(days_delete) + ","
                "\"reason\":\"" + fix_quotes_string_for_json(reason) + "\""
            "}";

            return core.post_task(request, method, json);
        }
        
        // unban member
        std::future<request_response> Member::unban_member(const unsigned long long gid)
        {
            if (!gid || !user.get_id()) return fake_future<request_response>();

            const std::string request = "/guilds/" + std::to_string(gid) + "/bans/" + std::to_string(user.get_id());
            const http_request method = http_request::DELETE;
            const std::string json = "";

            return core.post_task(request, method, json);
        }
        
        
    }
}