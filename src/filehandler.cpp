// Tested, works as of 20231219

#include "filehandler.h"
#include "defaults.h"
#include "functionwrapper.h"
#include <stdexcept>

const size_t alloc_size_each = 64;
static const char TAG[] = "FP";

struct __file_core_ctl {
    EventHandler* evh = nullptr;
    BaseType_t core_id = tskNO_AFFINITY;
    bool ready = false;

    void work_right(FunctionWrapper);
};

__file_core_ctl __file_core;

#define FPWRK(FW) __file_core.work_right(FW)


FunctionWrapper fw_fopen(volatile bool* don, FILE** to, const char* path, const char* mode)
{
    static const char mount_point[] = MOUNT_POINT;
    const size_t size_path = strlen(path);
    const size_t size_needed = size_path + sizeof(mount_point) + 1;
    char* buf = new char[size_needed];

    memcpy(buf, mount_point, sizeof(mount_point));
    buf[sizeof(mount_point) - 1] = '/';
    memcpy(buf + sizeof(mount_point), path, size_path);
    buf[size_needed - 1] = '\0';

    struct __tmp{
        FILE** to; // static ref (hopefully)
        char* buf; // allocated! NEED DEALLOC
        const char* mode;
        volatile bool* done;
    };

    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;

        *(wk->to) = fopen(wk->buf, wk->mode); // if opened, it won't even enter the for loop
        for (size_t try_pls = 0; !*(wk->to) && try_pls++ < 15; *(wk->to) = fopen(wk->buf, wk->mode)) {
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!*(wk->to)) ESP_LOGE(TAG, "Failed to open %s!", wk->buf);
        else            ESP_LOGI(TAG, "Opened file %s @ %p!", wk->buf, *wk->to);

        *(wk->done) = true;
        delete[] wk->buf;
    }, new __tmp{to, buf, mode, don}, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fclose(volatile bool* don, FILE* take)
{
    if (!take) return {};
    
    struct __tmp{
        FILE* fp; // static ref (hopefully)
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;

        void* ptrval = (void*)wk->fp;
        fclose(wk->fp);
        
        ESP_LOGI(TAG, "Closed file %p!", ptrval);

        *(wk->done) = true;

    }, new __tmp{ take, don }, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fwrite(volatile bool* don, FILE* to, const char* buf, size_t siz, size_t* tw)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        const char* buf;
        size_t siz;
        volatile bool* done;
        size_t* total;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;

        if (wk->siz >= alloc_size_each) {
            size_t total_write = 0;
            while(wk->siz > 0) {
                const size_t wrote = fwrite(wk->buf, sizeof(char), wk->siz > alloc_size_each ? alloc_size_each : wk->siz, wk->fp);
                fflush(wk->fp);

                if (wrote == 0) {
                    *wk->total = total_write;
                    *(wk->done) = true;
                    return;
                }

                wk->buf += wrote;
                total_write += wrote;
                wk->siz -= wrote;
            }
            *wk->total = total_write;
            *(wk->done) = true;
            return;
        }

        const size_t wrote = fwrite(wk->buf, sizeof(char), wk->siz, wk->fp);
        fflush(wk->fp);
        *wk->total = wrote;
        *(wk->done) = true;

    }, new __tmp{to, buf, siz, don, tw}, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fread(volatile bool* don, FILE* to, char* buf, size_t siz, size_t* tr)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        char* buf;
        size_t siz;
        volatile bool* done;
        size_t* total;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        *wk->total = fread(wk->buf, sizeof(char), wk->siz, wk->fp);

        *(wk->done) = true;

    }, new __tmp{to, buf, siz, don, tr}, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fgetc(volatile bool* don, FILE* to, int* buf)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        int* buf;
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        *wk->buf = fgetc(wk->fp);
        
        *(wk->done) = true;

    }, new __tmp{to, buf, don }, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fputc(volatile bool* don, FILE* to, int val)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        int val;
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        fputc(wk->val, wk->fp);

        *(wk->done) = true;

    }, new __tmp{to, val, don }, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fseek(volatile bool* don, FILE* to, long dst, int mod)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        long dst;
        int mod;
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        fseek(wk->fp, wk->dst, wk->mod);

        *(wk->done) = true;
    }, new __tmp{to, dst, mod, don }, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_fflush(volatile bool* don, FILE* to)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        fflush(wk->fp);

        *(wk->done) = true;
    }, new __tmp{to, don }, [](void* p){ delete (__tmp*)p; });
}

FunctionWrapper fw_feof(volatile bool* don, FILE* to, bool* eof)
{    
    if (!to) return {};

    struct __tmp{
        FILE* fp; // static ref (hopefully)
        bool* eof;
        volatile bool* done;
    };
    
    return FunctionWrapper([](void* d){
        __tmp* wk = (__tmp*)d;
                
        *wk->eof = feof(wk->fp);

        *(wk->done) = true;
    }, new __tmp{to, eof, don }, [](void* p){ delete (__tmp*)p; });
}



File::File(const char* path, const char* mode)
{
    fp_w = false;
    FPWRK(fw_fopen(&fp_w, &fp, path, mode));
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // not ready
}

File::File(File&& o) noexcept
{
    while (!o.fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // not ready
    EXC_NULL(fp, o.fp);
    EXCHANGE(wrote, o.wrote, 0);
}

void File::operator=(File&& o) noexcept
{
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // not ready
    while (!o.fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // not ready
    close();
    EXC_NULL(fp, o.fp);
    EXCHANGE(wrote, o.wrote, 0);
}

File::~File()
{
    close();
}

void File::close()
{
    fp_w = false;
    if (fp) {
        FPWRK(fw_fclose(&fp_w, fp));
        while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // not ready
        fp = nullptr;
    }
}

void File::write(const char* buf, size_t siz)
{
    fp_w = false;
    FPWRK(fw_fwrite(&fp_w, fp, buf, siz, &wrote));
}

size_t File::get_written() const
{
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // wait
    return wrote;
}

size_t File::read(char* buf, const size_t siz)
{
    size_t readd = 0;
    fp_w = false;
    FPWRK(fw_fread(&fp_w, fp, buf, siz, &readd));
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // wait
    return readd;
}

int File::getc()
{    
    int g = -1;
    fp_w = false;
    FPWRK(fw_fgetc(&fp_w, fp, &g));
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // wait

    return g;
}

void File::putc(const int ch)
{    
    fp_w = false;
    FPWRK(fw_fputc(&fp_w, fp, ch));
}

void File::seek(const long off, const seek_mode mode)
{
    fp_w = false;
    FPWRK(fw_fseek(&fp_w, fp, off, static_cast<int>(mode)));
}

void File::flush()
{
    fp_w = false;
    FPWRK(fw_fflush(&fp_w, fp));
}

bool File::eof() const
{
    bool e = true;
    fp_w = false;
    FPWRK(fw_feof(&fp_w, fp, &e));
    while (!fp_w) vTaskDelay(pdMS_TO_TICKS(10)); // wait
    return e;
}

bool File::good() const
{
    return fp != nullptr;
}

bool File::bad() const
{
    return fp == nullptr;
}

File::operator bool() const
{
    return fp != nullptr;
}


void __file_core_ctl::work_right(FunctionWrapper f)
{
    if (!__file_core.ready) throw std::runtime_error("FILESYSTEM Not initialized properly");
    if (__file_core.core_id != xPortGetCoreID()) {
        __file_core.evh->post((FunctionWrapper&&)f, static_cast<unsigned>(-1));
    }
    else {
        f.run();
    }
}

void __file_make_event_handler_on_correct_core_id()
{
    if (!__file_core.ready) {
        __file_core.core_id = xPortGetCoreID();
        __file_core.evh = new EventHandler("FILEWA", 1, 3072, 2, __file_core.core_id);
        __file_core.ready = true;
    }
}

void __file_kill_event_handler_on_correct_core_id()
{
    if (__file_core.ready) {
        __file_core.core_id = tskNO_AFFINITY;
        if (__file_core.evh) delete __file_core.evh;
        __file_core.ready = false;
    }
}