/*
* # MEMORY_JSON
* Verified: < 20210520
*/

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#include "printer.hpp"
#include "memory_file.hpp"

//#define LSW_EXPERIMENTAL_JSON_AUTOCAST_ENABLE // doesn't work well, maybe another day
//#define LSW_DEBUG_JSON_PARSE_INFO

namespace LSW {
    namespace PocketDiscord {

        const std::string useless_for_json = "\n\r\t ";

        enum class json_type {
            STRING,
            BOOL,
            NUMBER,
            OBJECT,
            ARRAY,
            EMPTY // null
        };
        enum class json_iterator_type {
            ARRAY_INDEX,
            OBJECT_PAIR
        };

        class MemoryFileJSON {
            struct json_pair {
                size_t m_beg = 0, m_end = 0;
                json_type type = json_type::STRING;
                std::string key;

                std::string as_string(const MemoryFile&) const;
            };
            struct json_unique {
                size_t m_beg = 0, m_end = 0;
                json_type type = json_type::STRING;
                size_t index = 0;

                std::string as_string(const MemoryFile&) const;
            };
            class json_both {
                const MemoryFile& memfp;
                const size_t m_beg, m_end;
                const json_type type;
                const size_t index;
                const std::string key;
            public:
                json_both(const json_pair&, const MemoryFile&);
                json_both(const json_unique&, const MemoryFile&);

                json_type get_type() const;
                const std::string& get_key() const;
                size_t get_index() const;
                std::string get_value_str() const;

                bool is_from_array() const; // basically no key

                bool is_string() const;
                bool is_bool() const;
                bool is_number() const;
                bool is_object() const;
                bool is_array() const;
                bool is_null() const; // same as empty
                bool is_empty() const;

                std::string as_string() const; // no \"\"
                unsigned long long as_llu() const;
                long long as_lld() const;
                int as_int() const;
                bool as_bool() const;
                MemoryFileJSON as_json() const;

#ifdef LSW_EXPERIMENTAL_JSON_AUTOCAST_ENABLE
                operator std::string() const;
                operator unsigned long long() const;
                operator long long() const;
                operator int() const;
                operator bool() const;
                operator MemoryFileJSON() const;
#endif

                MemoryFileJSON operator[](const std::string&) const;
                MemoryFileJSON operator[](const size_t&) const;

                const MemoryFile& get_memfp() const;
            };

            class json_any_vec_iterator {
                const MemoryFile& memfp;
                std::vector<json_pair>::const_iterator pair_it;
                std::vector<json_unique>::const_iterator unique_it;
                const json_iterator_type iterator_type;

                json_any_vec_iterator(const json_any_vec_iterator&);
                json_any_vec_iterator(json_any_vec_iterator&&) noexcept;
            public:
                json_any_vec_iterator(const std::vector<json_pair>&, const bool, const MemoryFile&); // end? true : false
                json_any_vec_iterator(const std::vector<json_unique>&, const bool, const MemoryFile&); // end? true : false

                json_any_vec_iterator operator++(); // ++this
                json_any_vec_iterator operator++(int); // this++

                bool operator==(const json_any_vec_iterator&) const;
                bool operator!=(const json_any_vec_iterator&) const;

                json_both operator*() const;
            };

            const size_t m_lim_beg, m_lim_end;
            const json_type type = json_type::OBJECT;
            const MemoryFile& memfp;
            std::vector<json_pair> objs;
            std::vector<json_unique> arrs;

            size_t get_current_limit() const;
            json_type get_type_on(const MemoryFile&, const size_t, const size_t) const;

            void _parse_as_array();
            void _parse_as_block();
            void parse();

            MemoryFileJSON(const MemoryFile&, const size_t, const size_t, const json_type);
        public:
            MemoryFileJSON(const MemoryFile&); // parse
            MemoryFileJSON(const MemoryFileJSON&) = delete;
            MemoryFileJSON(MemoryFileJSON&&);

            void reset_and_reparse();

            MemoryFileJSON operator[](const std::string&) const;
            MemoryFileJSON operator[](const size_t&) const;

            json_any_vec_iterator begin() const;
            json_any_vec_iterator end() const;

            json_type get_type() const;
            std::string get_value_str() const;
            size_t get_array_length() const;

            bool is_string() const;
            bool is_bool() const;
            bool is_number() const;
            bool is_object() const;
            bool is_array() const;
            bool is_null() const; // same as empty
            bool is_empty() const;

            std::string as_string() const; // no \"\"
            unsigned long long as_llu() const;
            long long as_lld() const;
            int as_int() const;
            bool as_bool() const;

#ifdef LSW_EXPERIMENTAL_JSON_AUTOCAST_ENABLE
            operator std::string() const;
            operator unsigned long long() const;
            operator long long() const;
            operator int() const;
            operator bool() const;
#endif

            const MemoryFile& get_memfp() const;
        };

        std::ostream& operator<<(std::ostream&, MemoryFileJSON const&);

        std::string jsontype_to_string(const json_type&);

        std::string fix_quotes_string_for_json(const std::string&);

    }
}