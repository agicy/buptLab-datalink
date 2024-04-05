#include "lprintf.h"

#define TIME_PREFIX_LENGTH 16
#define BUFFER_SIZE 256

static inline void write(const char *const buf, const size_t len) {
    if (log_file)
        fwrite(buf, sizeof(char), len, log_file);
    fwrite(buf, sizeof(char), len, stdout);
    return;
}

static inline void output_line_by_line(const char *const buffer, const size_t length) {
    char time_stamp_buffer[TIME_PREFIX_LENGTH];
    uint32_t time_stamp = get_ms();
    size_t time_stamp_length = sprintf(time_stamp_buffer, "%5d.%03d ", time_stamp / 1000, time_stamp % 1000);

    for (size_t lptr = 0, rptr; lptr < length; lptr = rptr + 1) {
        rptr = lptr;
        while (rptr < length && buffer[rptr] != '\n')
            ++rptr;

        write(time_stamp_buffer, time_stamp_length);
        write(buffer + lptr, rptr - lptr);
        write("\n", 1);
    }
    return;
}

int vlprintf(const char *format, va_list arg_ptr) {
    char format_buffer[BUFFER_SIZE];
    int n = vsnprintf(format_buffer, BUFFER_SIZE, format, arg_ptr);
    output_line_by_line(format_buffer, n);
    return n;
}

int lprintf(const char *format, ...) {
    va_list arg_ptr;

    va_start(arg_ptr, format);
    int n = vlprintf(format, arg_ptr);
    va_end(arg_ptr);

    return n;
}
