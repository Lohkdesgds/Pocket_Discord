#include "memory_json.hpp"

namespace LSW {
    namespace PocketDiscord {

        std::string MemoryFileJSON::json_pair::as_string(const MemoryFile& fp) const
        {
            return fp.substr(m_beg, m_end - m_beg);
        }

        std::string MemoryFileJSON::json_unique::as_string(const MemoryFile& fp) const
        {
            return fp.substr(m_beg, m_end - m_beg);
        }

        MemoryFileJSON::json_both::json_both(const json_pair& oth, const MemoryFile& fp)
            : memfp(fp), m_beg(oth.m_beg), m_end(oth.m_end), type(oth.type), index(0), key(oth.key)
        {
        }

        MemoryFileJSON::json_both::json_both(const json_unique& oth, const MemoryFile& fp)
            : memfp(fp), m_beg(oth.m_beg), m_end(oth.m_end), type(oth.type), index(oth.index), key()
        {
        }

        json_type MemoryFileJSON::json_both::get_type() const
        {
            return type;
        }

        const std::string& MemoryFileJSON::json_both::get_key() const
        {
            return key;
        }

        size_t MemoryFileJSON::json_both::get_index() const
        {
            return index;
        }

        std::string MemoryFileJSON::json_both::get_value_str() const
        {
            return memfp.substr(m_beg, m_end - m_beg);
        }

        bool MemoryFileJSON::json_both::is_from_array() const
        {
            return key.empty();
        }

        bool MemoryFileJSON::json_both::is_string() const
        {
            return type == json_type::STRING;
        }

        bool MemoryFileJSON::json_both::is_bool() const
        {
            return type == json_type::BOOL;
        }

        bool MemoryFileJSON::json_both::is_number() const
        {
            return type == json_type::NUMBER;
        }

        bool MemoryFileJSON::json_both::is_object() const
        {
            return type == json_type::OBJECT;
        }

        bool MemoryFileJSON::json_both::is_array() const
        {
            return type == json_type::ARRAY;
        }

        bool MemoryFileJSON::json_both::is_null() const
        {
            return type == json_type::EMPTY;
        }

        bool MemoryFileJSON::json_both::is_empty() const
        {
            return type == json_type::EMPTY;
        }

        std::string MemoryFileJSON::json_both::as_string() const
        {
            std::string temp = get_value_str();
            if (temp.size() && temp.front() == '\"') temp.erase(temp.begin());
            if (temp.size() && temp.back() == '\"') temp.pop_back();
            return temp;
        }

        unsigned long long MemoryFileJSON::json_both::as_llu() const
        {
            std::string temp = as_string();
            try {
                return std::stoull(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as LLU '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing LLU '" + temp + "'!"));
            }
            return 0;
        }

        long long MemoryFileJSON::json_both::as_lld() const
        {
            std::string temp = as_string();
            try {
                return std::stoll(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as LLD '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing LLD '" + temp + "'!"));
            }
            return 0;
        }

        int MemoryFileJSON::json_both::as_int() const
        {
            std::string temp = as_string();
            try {
                return std::stoi(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as INT '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing INT '" + temp + "'!"));
            }
            return 0;
        }

        bool MemoryFileJSON::json_both::as_bool() const
        {
            return as_string().find("true") == 0;
        }

        MemoryFileJSON MemoryFileJSON::json_both::as_json() const
        {
            return MemoryFileJSON(memfp, m_beg, m_end, type);
        }

#ifdef LSW_EXPERIMENTAL_JSON_AUTOCAST_ENABLE
        MemoryFileJSON::json_both::operator std::string() const
        {
            return as_string();
        }

        MemoryFileJSON::json_both::operator unsigned long long() const
        {
            return as_llu();
        }

        MemoryFileJSON::json_both::operator long long() const
        {
            return as_lld();
        }

        MemoryFileJSON::json_both::operator int() const
        {
            return as_int();
        }

        MemoryFileJSON::json_both::operator bool() const
        {
            return as_bool();
        }

        MemoryFileJSON::json_both::operator MemoryFileJSON() const
        {
            return as_json();
        }
#endif
        const MemoryFile& MemoryFileJSON::json_both::get_memfp() const
        {
            return memfp;
        }

        MemoryFileJSON MemoryFileJSON::json_both::operator[](const std::string& key) const
        {
            return as_json()[key];
        }

        MemoryFileJSON MemoryFileJSON::json_both::operator[](const size_t& index) const
        {
            return as_json()[index];
        }

        MemoryFileJSON::json_any_vec_iterator::json_any_vec_iterator(const MemoryFileJSON::json_any_vec_iterator& oth)
            : memfp(oth.memfp), pair_it(oth.pair_it), unique_it(oth.unique_it), iterator_type(oth.iterator_type)
        {
        }

        MemoryFileJSON::json_any_vec_iterator::json_any_vec_iterator(MemoryFileJSON::json_any_vec_iterator&& oth) noexcept
            : memfp(oth.memfp), pair_it(std::move(oth.pair_it)), unique_it(std::move(oth.unique_it)), iterator_type(oth.iterator_type)
        {
        }

        MemoryFileJSON::json_any_vec_iterator::json_any_vec_iterator(const std::vector<json_pair>& vec, const bool as_end, const MemoryFile& fp)
            : memfp(fp), pair_it(as_end ? vec.end() : vec.begin()), unique_it(), iterator_type(json_iterator_type::OBJECT_PAIR)
        {
        }

        MemoryFileJSON::json_any_vec_iterator::json_any_vec_iterator(const std::vector<json_unique>& vec, const bool as_end, const MemoryFile& fp)
            : memfp(fp), pair_it(), unique_it(as_end ? vec.end() : vec.begin()), iterator_type(json_iterator_type::ARRAY_INDEX)
        {
        }

        MemoryFileJSON::json_any_vec_iterator MemoryFileJSON::json_any_vec_iterator::operator++()
        {
            switch (iterator_type) {
            case json_iterator_type::ARRAY_INDEX:
                ++unique_it;
                break;
            case json_iterator_type::OBJECT_PAIR:
                ++pair_it;
                break;
            }
            return *this;
        }

        MemoryFileJSON::json_any_vec_iterator MemoryFileJSON::json_any_vec_iterator::operator++(int)
        {
            MemoryFileJSON::json_any_vec_iterator cpy = *this;
            ++(*this);
            return cpy;
        }

        bool MemoryFileJSON::json_any_vec_iterator::operator==(const json_any_vec_iterator& oth) const
        {
            return memfp == oth.memfp && iterator_type == oth.iterator_type && (iterator_type == json_iterator_type::OBJECT_PAIR ? (pair_it == oth.pair_it) : (unique_it == oth.unique_it));
        }

        bool MemoryFileJSON::json_any_vec_iterator::operator!=(const json_any_vec_iterator& oth) const
        {
            return !((*this) == oth);
        }

        MemoryFileJSON::json_both MemoryFileJSON::json_any_vec_iterator::operator*() const
        {
            switch (iterator_type) {
            case json_iterator_type::ARRAY_INDEX:
                return MemoryFileJSON::json_both(*unique_it, memfp);
            case json_iterator_type::OBJECT_PAIR:
                return MemoryFileJSON::json_both(*pair_it, memfp);
            }

            logg << L::SL << Color::RED << "[MemoryJSON] Failed: invalid JSON_BOTH object." << L::EL;
            throw std::runtime_error("invalid json_both object");
            return MemoryFileJSON::json_both(json_pair(), memfp);
        }

        size_t MemoryFileJSON::get_current_limit() const
        {
            if (m_lim_end == static_cast<size_t>(-1)) return memfp.size();
            return m_lim_end;
        }

        json_type MemoryFileJSON::get_type_on(const MemoryFile& fp, const size_t at_left, const size_t at_right) const
        {
            if (at_left > at_right) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed: invalid JSON range." << L::EL;
                throw std::runtime_error("invalid range.");
            }
            const char left = fp[at_left], right = fp[at_right - 1];

            if (left == '\"' && right == '\"') {
                return json_type::STRING;
            }
            else if (at_right - at_left > 5) {
                if (left == '{' && right == '}') return json_type::OBJECT;
                else if (left == '[' && right == ']') return json_type::ARRAY;
                else return json_type::NUMBER;
            }
            else {
                const std::string temp = fp.substr(at_left, at_right - at_left);
                if (temp == "true" || temp == "false") return json_type::BOOL;
                else if (temp == "{}" || temp == "[]" || temp == "null") return json_type::EMPTY;
                else return json_type::NUMBER;
            }
            logg << L::SL << Color::RED << "[MemoryJSON] Failed: can't determine json type." << L::EL;
            throw std::runtime_error("Invalid JSON: can't check what type it is.");
            return json_type::OBJECT;
        }

        void MemoryFileJSON::_parse_as_array()
        {
#ifdef LSW_DEBUG_JSON_PARSE_INFO
            logg << L::SL << Color::BRIGHT_GRAY << "[JSON] Parsing as array..." << L::EL;
#endif
            //json_unique
            const size_t curr_limit = get_current_limit();
            size_t counting = 0;

            size_t depth = 0;
            bool skip = false; // backslash
            bool in_quote = false;
            size_t last_object_starts[2] = { m_lim_beg, m_lim_beg };

            const auto object_add_marker = [&](const size_t val) {
                last_object_starts[1] = last_object_starts[0];
                last_object_starts[0] = val;
            };
            const auto fix_object_and_push = [&] {

                json_unique current_obj;

                current_obj.m_beg = last_object_starts[1] + 1;
                current_obj.m_end = last_object_starts[0];

                while (current_obj.m_beg < current_obj.m_end && (std::find(useless_for_json.begin(), useless_for_json.end(), memfp[current_obj.m_beg]) != useless_for_json.end()))
                    current_obj.m_beg++; // 1 is the left one
                while (current_obj.m_beg < current_obj.m_end && (std::find(useless_for_json.begin(), useless_for_json.end(), memfp[current_obj.m_end - 1]) != useless_for_json.end()))
                    current_obj.m_end--; // 0 is the right one

                current_obj.type = get_type_on(memfp, current_obj.m_beg, current_obj.m_end);
                current_obj.index = counting++;

                arrs.push_back(std::move(current_obj));

                /*std::cout
                    << "\nindex: '" << arrs.back().index << "'"
                    << "\nval:   '" << arrs.back().as_string(memfp) << "'"
                    << "\ntype:  '" << jsontype_to_string(arrs.back().type) << "'" << std::endl;*/
            };

            for (size_t curr = m_lim_beg; curr < curr_limit; curr++)
            {
                if (skip) { skip = false; continue; }

                const char ch = memfp[curr];

                switch (ch) {
                case '\\':
                    skip = true;
                    break;
                case '\"':
                    in_quote = !in_quote;
                    break;
                case ',':
                    if (depth == 1 && !in_quote) {
                        object_add_marker(curr);
                        fix_object_and_push();
                    }
                    break;
                case '{':
                case '[':
                    if (!in_quote) depth++;
                    break;
                case '}':
                case ']':
                    if (!in_quote) {
                        if (depth-- == 1) {
                            object_add_marker(curr);
                            fix_object_and_push();
                            curr = curr_limit;
                            continue;
                        }
                    }
                    break;
                }
            }
#ifdef LSW_DEBUG_JSON_PARSE_INFO
            logg << L::SL << Color::BRIGHT_GRAY << "[JSON] Parsed array of length: " << arrs.size() << "." << L::EL;
#endif
        }

        void MemoryFileJSON::_parse_as_block()
        {
#ifdef LSW_DEBUG_JSON_PARSE_INFO
            logg << L::SL << Color::BRIGHT_GRAY << "[JSON] Parsing as object..." << L::EL;
#endif
            const size_t curr_limit = get_current_limit();

            size_t depth = 0;
            bool skip = false; // backslash
            bool in_quote = false;
            bool has_key_pending = false;
            size_t last_quote_starts[2] = { m_lim_beg, m_lim_beg };
            size_t last_object_starts[2] = { m_lim_beg, m_lim_beg };

            json_pair current_obj;

            const auto quote_add_marker = [&](const size_t val) {
                last_quote_starts[1] = last_quote_starts[0];
                last_quote_starts[0] = val;
            };
            const auto object_add_marker = [&](const size_t val) {
                last_object_starts[1] = last_object_starts[0];
                last_object_starts[0] = val;
            };
            const auto fix_object_and_push = [&]() {

                if (current_obj.key.empty()) {
                    logg << L::SL << Color::RED << "[MemoryJSON] Failed: value with no key." << L::EL;
                    throw std::runtime_error("value with no key! Invalid JSON");
                }

                while (last_object_starts[1] < last_object_starts[0] && (std::find(useless_for_json.begin(), useless_for_json.end(), memfp[last_object_starts[1]]) != useless_for_json.end()))
                    last_object_starts[1]++; // 1 is the left one
                while (last_object_starts[1] < last_object_starts[0] && (std::find(useless_for_json.begin(), useless_for_json.end(), memfp[last_object_starts[0] - 1]) != useless_for_json.end()))
                    last_object_starts[0]--; // 0 is the right one

                current_obj.m_beg = last_object_starts[1];
                current_obj.m_end = last_object_starts[0];
                current_obj.type = get_type_on(memfp, current_obj.m_beg, current_obj.m_end);

                objs.push_back(std::move(current_obj));
                current_obj.key.clear();

                /*std::cout
                    << "\nkey:  '" << objs.back().key << "'"
                    << "\nval:  '" << objs.back().as_string(memfp) << "'"
                    << "\ntype: '" << jsontype_to_string(objs.back().type) << "'" << std::endl;*/
            };

            for (size_t curr = m_lim_beg; curr < curr_limit; curr++)
            {
                if (skip) { skip = false; continue; }

                const char ch = memfp[curr];

                switch (ch) {
                case '\\':
                    skip = true;
                    break;
                case '\"':
                    in_quote = !in_quote;
                    if (depth == 1) quote_add_marker(curr);
                    break;
                case ':':
                    if (depth == 1 && !in_quote) {
                        current_obj.key = memfp.substr(last_quote_starts[1] + 1, last_quote_starts[0] - last_quote_starts[1] - 1);
                        object_add_marker(curr + 1);
                        //std::cout << "\nkey: '" << current_obj.key << "'" << std::endl;
                        has_key_pending = true;
                    }
                    break;
                case ',':
                    if (depth == 1 && !in_quote) {
                        object_add_marker(curr);
                        fix_object_and_push();
                        //std::cout << "val: '" << memfp.substr(last_object_starts[1], last_object_starts[0] - last_object_starts[1]) << "'" << std::endl;
                        has_key_pending = false;
                    }
                    break;
                case '{':
                case '[':
                    if (!in_quote) depth++;
                    break;
                case '}':
                case ']':
                    if (!in_quote) {
                        if (depth-- == 0) {
                            curr = curr_limit;
                            has_key_pending = false;
                            continue;
                        }
                    }
                    break;
                }
            }

            if (has_key_pending) {
                object_add_marker(curr_limit - 1);
                fix_object_and_push();
            }
#ifdef LSW_DEBUG_JSON_PARSE_INFO
            logg << L::SL << Color::BRIGHT_GRAY << "[JSON] Parsed object with " << objs.size() << " pair(s)." << L::EL;
#endif
        }

        void MemoryFileJSON::parse()
        {
            objs.clear();

            switch (type) {
            case json_type::ARRAY:
                _parse_as_array();
                break;
            case json_type::OBJECT:
                _parse_as_block();
                break;
            default:
                break;
            /*default: // not an error lmao
                logg << L::SL << Color::RED << "[MemoryJSON] Failed: invalid type to parse as object." << L::EL;
                throw std::runtime_error("Invalid JSON type to parse");
                break;*/
            }
        }

        MemoryFileJSON::MemoryFileJSON(const MemoryFile& fp, const size_t s, const size_t e, const json_type t)
            : m_lim_beg(s), m_lim_end(e), type(t), memfp(fp)
        {
            parse();
        }

        MemoryFileJSON::MemoryFileJSON(const MemoryFile& fp)
            : m_lim_beg(0), m_lim_end(static_cast<size_t>(-1)), type(get_type_on(fp, m_lim_beg, (m_lim_end == static_cast<size_t>(-1) ? fp.size() : m_lim_end))), memfp(fp)
        {
            parse();
        }

        MemoryFileJSON::MemoryFileJSON(MemoryFileJSON&& oth)
            : m_lim_beg(oth.m_lim_beg), m_lim_end(oth.m_lim_end), type(oth.type), memfp(oth.memfp), objs(std::move(oth.objs)), arrs(std::move(oth.arrs))
        {
        }

        MemoryFileJSON MemoryFileJSON::operator[](const std::string& key) const
        {
            for (auto& i : objs) {
                if (i.key == key) {
                    return MemoryFileJSON(memfp, i.m_beg, i.m_end, i.type);
                }
            }
            return MemoryFileJSON(memfp, 0, 0, json_type::EMPTY);
        }

        MemoryFileJSON MemoryFileJSON::operator[](const size_t& index) const
        {
            for (auto& i : arrs) {
                if (i.index == index) {
                    return MemoryFileJSON(memfp, i.m_beg, i.m_end, i.type);
                }
            }
            return MemoryFileJSON(memfp, 0, 0, json_type::EMPTY);
        }

        MemoryFileJSON::json_any_vec_iterator MemoryFileJSON::begin() const
        {
            if (objs.size()) return MemoryFileJSON::json_any_vec_iterator(objs, false, memfp);
            if (arrs.size()) return MemoryFileJSON::json_any_vec_iterator(arrs, false, memfp);
            return MemoryFileJSON::json_any_vec_iterator(arrs, true, memfp); // worst case: end() of arrs (chosen here)
        }

        MemoryFileJSON::json_any_vec_iterator MemoryFileJSON::end() const
        {
            if (objs.size()) return MemoryFileJSON::json_any_vec_iterator(objs, true, memfp);
            if (arrs.size()) return MemoryFileJSON::json_any_vec_iterator(arrs, true, memfp);
            return MemoryFileJSON::json_any_vec_iterator(arrs, true, memfp); // worst case: end() of arrs (chosen here)
        }

        json_type MemoryFileJSON::get_type() const
        {
            return type;
        }

        std::string MemoryFileJSON::get_value_str() const
        {
            return memfp.substr(m_lim_beg, get_current_limit() - m_lim_beg);
        }

        size_t MemoryFileJSON::get_array_length() const
        {
            return arrs.size();
        }

        bool MemoryFileJSON::is_string() const
        {
            return type == json_type::STRING;
        }

        bool MemoryFileJSON::is_bool() const
        {
            return type == json_type::BOOL;
        }

        bool MemoryFileJSON::is_number() const
        {
            return type == json_type::NUMBER;
        }

        bool MemoryFileJSON::is_object() const
        {
            return type == json_type::OBJECT;
        }

        bool MemoryFileJSON::is_array() const
        {
            return type == json_type::ARRAY;
        }

        bool MemoryFileJSON::is_null() const
        {
            return type == json_type::EMPTY;
        }

        bool MemoryFileJSON::is_empty() const
        {
            return type == json_type::EMPTY;
        }

        std::string MemoryFileJSON::as_string() const
        {
            std::string temp = get_value_str();
            if (temp.size() && temp.front() == '\"') temp.erase(temp.begin());
            if (temp.size() && temp.back() == '\"') temp.pop_back();
            return temp;
        }

        unsigned long long MemoryFileJSON::as_llu() const
        {
            std::string temp = as_string();
            try {
                return std::stoull(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as LLU '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing LLU '" + temp + "'!"));
            }
            return 0;
        }

        long long MemoryFileJSON::as_lld() const
        {
            std::string temp = as_string();
            try {
                return std::stoll(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as LLD '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing LLD '" + temp + "'!"));
            }
            return 0;
        }

        int MemoryFileJSON::as_int() const
        {
            std::string temp = as_string();
            try {
                return std::stoi(temp);
            }
            catch (...) {
                logg << L::SL << Color::RED << "[MemoryJSON] Failed parsing as INT '" << temp << "'" << L::EL;
                throw std::runtime_error(("Failed parsing INT '" + temp + "'!"));
            }
            return 0;
        }

        bool MemoryFileJSON::as_bool() const
        {
            return as_string().find("true") == 0;
        }

#ifdef LSW_EXPERIMENTAL_JSON_AUTOCAST_ENABLE
        MemoryFileJSON::operator std::string() const
        {
            return as_string();
        }

        MemoryFileJSON::operator unsigned long long() const
        {
            return as_llu();
        }

        MemoryFileJSON::operator long long() const
        {
            return as_lld();
        }

        MemoryFileJSON::operator int() const
        {
            return as_int();
        }

        MemoryFileJSON::operator bool() const
        {
            return as_bool();
        }
#endif
        const MemoryFile& MemoryFileJSON::get_memfp() const
        {
            return memfp;
        }

        std::ostream& operator<<(std::ostream& out, MemoryFileJSON const& me)
        {
            return out << me.get_value_str();
        }

        std::string jsontype_to_string(const json_type& t)
        {
            switch (t) {
            case json_type::STRING:
                return "STRING";
            case json_type::BOOL:
                return "BOOL";
            case json_type::NUMBER:
                return "NUMBER";
            case json_type::OBJECT:
                return "OBJECT";
            case json_type::ARRAY:
                return "ARRAY";
            default: // EMPTY or other case?
                return "EMPTY";
            }
        }

        std::string fix_quotes_string_for_json(const std::string& str)
        {
            std::string out;
            for (auto& i : str) {
                switch (i) {
                case '\b':
                    out += "\\b";
                    break;
                case '\f':
                    out += "\\f";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                case '\"':
                    out += "\\\"";
                    break;
                case '\\':
                    out += "\\\\";
                    break;
                default:
                    out += i;
                    break;
                }
            }
            return out;
        }
    }
}
