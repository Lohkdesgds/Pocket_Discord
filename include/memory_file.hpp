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
#include <vector>
#include <mutex>
#include <algorithm>

#include "printer.hpp"
#include "sd_card.hpp"

//#define LSW_MEMORYFILE_NOBUFFER // POOR PERFORMANCE, but less memory usage (-1024 bytes per file)
//#define LSW_DISCARD_READWRITE_ERRORS

namespace LSW {
    namespace PocketDiscord {

        const std::string memory_path = mount_point + std::string("/memory/");
        constexpr size_t memfile_tempbuf_size = 4096; // this is used in other places...
        constexpr size_t read_block_chunk_size = alloc_size_each; // from SD card


        class MemoryFile {
            static unsigned fpcount;
            static unsigned fpopened;
            static std::mutex safemtx;

            FILE* fp = nullptr;
            size_t size_now_valid = 0;
#ifndef LSW_MEMORYFILE_NOBUFFER
            //char tempbuf[memfile_tempbuf_size]{};
            std::vector<char> tempbuf;
#endif

            void gen_new();
            void close_fp();
            void check_gen_new(); // if null, gen_new
        public:
            MemoryFile();
            ~MemoryFile();

            MemoryFile(const MemoryFile&);
            MemoryFile(MemoryFile&&) noexcept;
            void operator=(const MemoryFile&);
            void operator=(MemoryFile&&) noexcept;

            bool operator==(const MemoryFile&) const;
            bool operator!=(const MemoryFile&) const;

            unsigned files_open() const;

            size_t size() const;
            size_t length() const;
            void resize(const size_t); // fseek
            void reserve(const size_t); // fseek
            void clear();
            bool empty() const;
            bool valid() const;

            char operator[](const size_t) const;
            char at(const size_t) const;
            char back() const;
            char front() const;

            void operator=(const std::string&);
            void operator+=(const std::string&);
            void append(const std::string&);
            void append(const char*, const size_t);
            void push_back(const char);
            void assign(const std::string&);
            void assign(const char*, const size_t);
            void pop_back();
            void swap(MemoryFile&&);

            size_t find_after(const char, const size_t) const;
            size_t find_before(const char, const size_t) const;
            bool find_block(size_t&, size_t&, const size_t, const size_t, const char, const char) const;
            // min, max (if all, set this as size()), '{', '}' (example)

            std::string substr(const size_t = 0, const size_t = static_cast<size_t>(-1)) const;
        };        

    }
}