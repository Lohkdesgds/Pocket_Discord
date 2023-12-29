#include "useful.h"

uint64_t get_time_ms() {
    //struct timespec ts;
    //if (timespec_get(&ts, TIME_UTC) == 0) return 0;
    //return static_cast<uint64_t>(ts.tv_sec) * 1000llu + static_cast<uint64_t>(ts.tv_nsec / 1000000);
    struct timespec tv_now;
    clock_gettime(CLOCK_REALTIME, &tv_now);
    return static_cast<uint64_t>(tv_now.tv_sec) * 1000ULL + static_cast<uint64_t>(tv_now.tv_nsec) / 1000000ULL;
}