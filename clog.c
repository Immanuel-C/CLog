#include "clog.h"

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <stdbool.h>

#define IGNORE_RETVAL (void)

struct {
    FILE* stream;
    clog_attribute_flags_t flags;
    clog_type_flags_t ignore_types;
    bool stream_is_file;
} s_context;

mtx_t s_mtx;
bool s_init = false;

const char* s_cl_type_colors[] = {
    "\033[0;37m",
    "\033[0;32m",
    "\033[1;33m", // Bold
    "\033[1;31m", // Bold
    "\033[1;91m", // Bold
    "\033[0m",
    "\033[1;32m", // Bold
};

void m_lock() { IGNORE_RETVAL mtx_lock(&s_mtx); }
void m_unlock() { IGNORE_RETVAL mtx_unlock(&s_mtx); }

clog_error_t clog_init(FILE* stream, clog_attribute_flags_t flags) {
    if (s_init) {
        return CLOG_ERROR_ALREADY_INITIALIZED_CLOGGER;
    }

    if (mtx_init(&s_mtx, mtx_plain) != thrd_success) {
        return CLOG_ERROR_FAILED_TO_CREATE_MUTEX;
    }

    m_lock();

    s_context.stream = stream ? stream : stdout; 
    s_context.flags = flags;
    s_context.ignore_types  = 0;
    s_context.stream_is_file = false;

    s_init = true;

    m_unlock();

    return CLOG_ERROR_SUCCESS;
}

clog_error_t clog_init_with_file(const char* filename, clog_attribute_flags_t flags) {
    FILE* file = fopen(filename, "w+");
    
    if (!file) {
        return CLOG_ERROR_FAILED_TO_OPEN_FILE;
    }

    clog_error_t result = clog_init(file, flags);
    s_context.stream_is_file = true;

    return result;
}

bool check_init() {
    if (s_init) {
        return true;
    }

    fprintf(stderr, "CLOG FATAL ERROR: CLogger has not been initialized");

    return false;
}

clog_error_t clog_log(clog_type_flags_t type, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    clog_error_t res = clog_vlog(type, fmt, args);
    
    va_end(args);

    return res;
}

clog_error_t clog_vlog(clog_type_flags_t type, const char* fmt, va_list args) {
    if (!check_init()) { return CLOG_ERROR_NOT_INITIALIZED; }

    m_lock();

    if (s_context.ignore_types & type) { 
        m_unlock();
        return CLOG_ERROR_SUCCESS; 
    }

    m_unlock();


    const char* fmt_header;
    const char* type_color_str;

    switch (type) {
    case CLOG_TYPE_INFO:
        fmt_header = "INFO: ";
        type_color_str = s_cl_type_colors[0];
        break;
    case CLOG_TYPE_TRACE:
        fmt_header = "TRACE: ";
        type_color_str = s_cl_type_colors[1];
        break;
    case CLOG_TYPE_WARNING:
        fmt_header = "WARNING: ";
        type_color_str = s_cl_type_colors[2];
        break;
    case CLOG_TYPE_ERROR:
        fmt_header = "ERROR: ";
        type_color_str = s_cl_type_colors[3];
        break;
    case CLOG_TYPE_FATAL_ERROR:
        fmt_header = "FATAL ERROR: ";
        type_color_str = s_cl_type_colors[4];
        break;
    case CLOG_TYPE_DEFAULT:
        fmt_header = "";
        type_color_str = s_cl_type_colors[5];
        break;
    case CLOG_TYPE_DEBUG:
        fmt_header = "DEBUG: ";
        type_color_str = s_cl_type_colors[6];
        break;
    default:
        assert(!"Not a valid log type. Do not treat the log type in the log functions as a bitfield.");
    }

    time_t raw_time;
    time(&raw_time);
    struct tm* tm = localtime(&raw_time);

    char* time_str = asctime(tm);
    // Remove the new line at the end of the time_str.
    time_str[strlen(time_str) - 1] = 0;

    size_t final_fmt_size = 
                strlen(fmt)          + 
                strlen(fmt_header)   +
                strlen(time_str)     + 
                2                       + // For []
                1                       + // For space
                1;                        // For \0 

    m_lock();

    // Colours do not apply to files so the special code for colours are just printed instead of the colour changing. So if we are logging to a file we must
    // not include the colour codes.
    if (!s_context.stream_is_file && !(s_context.flags & CLOG_ATTRIB_FLAG_NO_COLOR)) {
        final_fmt_size += strlen(s_cl_type_colors[5]) + strlen(type_color_str);
    }

    char* final_fmt = malloc(final_fmt_size);

    if (!final_fmt) {
        return CLOG_ERROR_ALLOC_FAILED;
    }

    if (s_context.stream_is_file || s_context.flags & CLOG_ATTRIB_FLAG_NO_COLOR) {
        IGNORE_RETVAL snprintf(final_fmt, final_fmt_size, "[%s] %s%s", 
                time_str,
                fmt_header, 
                fmt
        );
    } else {
        IGNORE_RETVAL snprintf(final_fmt, final_fmt_size, "[%s] %s%s%s%s", 
                time_str,
                type_color_str,
                fmt_header, 
                fmt, 
                s_cl_type_colors[5]
        );

    }

    IGNORE_RETVAL vfprintf(s_context.stream, final_fmt, args);

    m_unlock();

    free(final_fmt);

    return CLOG_ERROR_SUCCESS;
}

void clog_set_log_output_stream(FILE* stream) {
    if (!check_init()) { return; }

    m_lock();

    if (stream != stdout &&
        stream != stderr &&
        stream != stdin  &&
        stream != NULL)
        fclose(s_context.stream);

    if (stream == NULL) {
        s_context.stream = stdout;
    } else {
        s_context.stream = stream;
    }

    s_context.stream_is_file = false;

    m_unlock();
}

clog_error_t clog_set_log_output_file(const char* filename) {
    if (!check_init()) { return CLOG_ERROR_NOT_INITIALIZED; }

    FILE* file = fopen(filename, "a+");
    
    if (!file) {
        return CLOG_ERROR_FAILED_TO_OPEN_FILE;
    }

    clog_set_log_output_stream(file);


    m_lock();
    s_context.stream_is_file = true;
    m_unlock();
    
    return CLOG_ERROR_SUCCESS;
}

void clog_ignore_log_types(clog_type_flags_t types) {
    if (!check_init()) { return; }
    m_lock();
    s_context.ignore_types = types;
    m_unlock();
}

void clog_terminate() {
    if (!check_init()) { return; }

    m_lock();
     
    if (s_context.stream != NULL   && 
        s_context.stream != stdout && 
        s_context.stream != stderr && 
        s_context.stream != stdin) {
        fclose(s_context.stream);
    }

    m_unlock();

    IGNORE_RETVAL mtx_destroy(&s_mtx);
}
