// Tested, works as of 20231219
#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

class File {
    FILE* fp = nullptr;
public:
    enum class seek_mode : int { CUR = 1, END = 2, SET = 0 };
    
    File() = default;
    File(const char* path, const char* mode);
    File(const File&) = delete;
    File(File&&) noexcept;

    void operator=(const File&) = delete;
    void operator=(File&&) noexcept;

    ~File();
    void close();

    size_t write(const char*, size_t);
    size_t read(char*, const size_t);
    int getc();
    int putc(const int);

    bool seek(const long, const seek_mode);
    bool flush();

    bool eof() const;

    bool good() const;
    bool bad() const;
    operator bool() const;
};