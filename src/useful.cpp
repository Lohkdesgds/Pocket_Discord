#include "useful.h"

#include "esp_heap_caps.h"

uint64_t get_time_ms() {
    //struct timespec ts;
    //if (timespec_get(&ts, TIME_UTC) == 0) return 0;
    //return static_cast<uint64_t>(ts.tv_sec) * 1000llu + static_cast<uint64_t>(ts.tv_nsec / 1000000);
    struct timespec tv_now;
    clock_gettime(CLOCK_REALTIME, &tv_now);
    return static_cast<uint64_t>(tv_now.tv_sec) * 1000ULL + static_cast<uint64_t>(tv_now.tv_nsec) / 1000000ULL;
}

ram_info::ram_info() :
    mem_total(heap_caps_get_total_size(MALLOC_CAP_8BIT)),
    mem_free(heap_caps_get_free_size(MALLOC_CAP_8BIT)),
    memex_total(heap_caps_get_total_size(MALLOC_CAP_EXEC)),
    memex_free(heap_caps_get_free_size(MALLOC_CAP_EXEC)),
    mem32_total(heap_caps_get_total_size(MALLOC_CAP_32BIT)),
    mem32_free(heap_caps_get_free_size(MALLOC_CAP_32BIT)),
    memi_total(heap_caps_get_total_size(MALLOC_CAP_IRAM_8BIT)),
    memi_free(heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT))
{
    nvs_get_stats(NULL, &nvs_stats);
}