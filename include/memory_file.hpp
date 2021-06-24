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
#include <memory>
#include <algorithm>

#include "printer.hpp"
#include "sd_card.hpp"

//#define LSW_MEMORYFILE_NOBUFFER // POOR PERFORMANCE, but less memory usage (-1024 bytes per file)
//#define LSW_DISCARD_READWRITE_ERRORS

namespace LSW {
    namespace PocketDiscord {

        const std::string memory_path = mount_point + std::string("/memory/");
        constexpr size_t memfile_tempbuf_size = 4096; // this is used in other places...
        constexpr size_t memfile_blocks_copy = 256; // bytes
        constexpr size_t read_block_chunk_size = alloc_size_each; // from SD card
        constexpr unsigned memfile_max_file_number = 200;


        class MemoryFile {
            struct _shared {
                unsigned m_count = 0;
                std::mutex mtx;

                unsigned request();
            };

            static _shared s_shared;

            std::unique_ptr<FILE, std::function<void(FILE*)>> m_fp;
            size_t m_size_now = 0;
            size_t m_ram_size = memfile_tempbuf_size;
            std::vector<char> m_buf;

            void set_size(const size_t);
            void write_noalloc(const char*, size_t, const size_t); // should not alloc. (array, size, offset)
            std::string read_noalloc(size_t, const size_t) const; // (size, offset)

            size_t max_ram() const;
        public:
            MemoryFile(const size_t = memfile_tempbuf_size);
            MemoryFile(const MemoryFile&);
            MemoryFile(MemoryFile&&) noexcept;
            MemoryFile(const std::string&);
            void operator=(const MemoryFile&);
            void operator=(MemoryFile&&) noexcept;

            bool operator==(const MemoryFile&) const;
            bool operator!=(const MemoryFile&) const;

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

            std::string substr(const size_t = 0, const size_t = static_cast<size_t>(-1)) const;
        };

    }
}