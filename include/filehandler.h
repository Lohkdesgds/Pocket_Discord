// Tested, works as of 20231219
#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#include "eventhandler.h"

class File {
    FILE* fp = nullptr;
    mutable bool fp_w = true; // fp working
    size_t wrote{};
public:
    enum class seek_mode : int { CUR = SEEK_CUR, END = SEEK_END, SET = SEEK_SET };
    
    File() = default;
    File(const char* path, const char* mode);
    File(const File&) = delete;
    File(File&&) noexcept;

    void operator=(const File&) = delete;
    void operator=(File&&) noexcept;

    ~File();
    void close();

    void write(const char*, size_t);
    size_t get_written() const;
    size_t read(char*, const size_t);
    int getc();
    void putc(const int);

    void seek(const long, const seek_mode);
    void flush();

    long size() const;

    bool eof() const;

    bool good() const;
    bool bad() const;
    operator bool() const;
};

void __file_make_event_handler_on_correct_core_id();
void __file_kill_event_handler_on_correct_core_id();