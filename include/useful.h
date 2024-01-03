#pragma once

#include <ctime>
#include "nvs.h"
#include "nvs_flash.h"

uint64_t get_time_ms();


struct ram_info {
    nvs_stats_t nvs_stats;
    const size_t mem_total;
    const size_t mem_free;
    const size_t memex_total;
    const size_t memex_free;
    const size_t mem32_total;
    const size_t mem32_free;
    const size_t memi_total;
    const size_t memi_free;

    ram_info();
};