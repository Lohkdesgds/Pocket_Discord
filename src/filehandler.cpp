// Tested, works as of 20231219

#include "filehandler.h"
#include "defaults.h"

const char mount_point[] = "/sdcard";
const size_t alloc_size_each = 64;
static const char TAG[] = "FP";

File::File(const char* path, const char* mode)
{
    static const char mount_point[] = MOUNT_POINT;
	const size_t size_path = strlen(path);
	const size_t size_needed = size_path + sizeof(mount_point) + 1;
	char* buf = new char[size_needed];

	memcpy(buf, mount_point, sizeof(mount_point));
	buf[sizeof(mount_point) - 1] = '/';
	memcpy(buf + sizeof(mount_point), path, size_path);
	buf[size_needed - 1] = '\0';

    fp = fopen(buf, mode);

    if (!fp) ESP_LOGE(TAG, "Failed to open %s!", buf);
    else     ESP_LOGI(TAG, "Opened file %s!", buf);

    delete[] buf;
}

File::File(File&& o) noexcept
  : fp(o.fp)
{
    o.fp = nullptr;
}

void File::operator=(File&& o) noexcept
{
    close();
    EXC_NULL(fp, o.fp);
}

File::~File()
{
    close();
}

void File::close()
{
    if (fp) fclose(fp);
    fp = nullptr;
}

size_t File::write(const char* buf, size_t siz)
{
    if (!fp) return 0;

    if (siz >= alloc_size_each) {
        size_t total_write = 0;
        while(siz > 0) {
            const size_t wrote = fwrite(buf, sizeof(char), siz > alloc_size_each ? alloc_size_each : siz, fp);
            fflush(fp);
            if (wrote == 0) return total_write;
            buf += wrote;
            total_write += wrote;
            siz -= wrote;
        }
        return total_write;
    }

    const size_t wrote = fwrite(buf, sizeof(char), siz, fp);
    fflush(fp);
    return wrote;
}

size_t File::read(char* buf, const size_t siz)
{
    return fp ? fread(buf, sizeof(char), siz, fp) : 0;
}

int File::getc()
{
    return fp ? fgetc(fp) : -1;
}

int File::putc(const int ch)
{
    return fp ? fputc(ch, fp) : -1;
}

bool File::seek(const long off, const seek_mode mode)
{
    return fp ? fseek(fp, off, static_cast<int>(mode)) : false;
}

bool File::flush()
{
    return fp ? fflush(fp) : true;
}

bool File::eof() const
{
    return fp ? feof(fp) : true;
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
