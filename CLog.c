#include "CLog.h"

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

struct {
    FILE* stream;
    clog_attribute_flags_t flags;
    bool stream_is_file;
} s_context;

pthread_mutex_t s_mutex;
bool s_init = false;

const char* const s_cl_type_colors[] = {
    "\033[0;37m",
    "\033[0;32m",
    "\033[1;33m", // Bold
    "\033[1;31m", // Bold
    "\033[1;91m", // Bold
    "\033[0m"
};

void m_lock() { pthread_mutex_lock(&s_mutex); }
void m_unlock() { pthread_mutex_unlock(&s_mutex); }

clog_error_t clog_init(FILE* stream, clog_attribute_flags_t flags) {
    if (s_init)
        return CL_ERROR_ALREADY_INITIALIZED_CLOGGER;

    if (pthread_mutex_init(&s_mutex, NULL) != 0) {
        return CL_ERROR_FAILED_TO_CREATE_MUTEX;
    }

    m_lock();

    s_context.stream = stream ? stream : stdout; 
    s_context.flags = flags;
    s_context.stream_is_file = false;

    s_init = true;

    m_unlock();

    return CL_ERROR_SUCCESS;
}

clog_error_t clog_init_with_file(const char* filename, clog_attribute_flags_t flags) {
    FILE* file = fopen(filename, "w+");
    
    if (!file) {
        return CL_ERROR_FAILED_TO_OPEN_FILE;
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

void clog_log(clog_type_t type, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    clog_vlog(type, fmt, args);
    va_end(args);
}

void clog_vlog(clog_type_t type, const char* fmt, va_list args) {
    if (!check_init()) return; 

    const char* fmt_header;

    switch (type) {
    case CL_TYPE_INFO:
        fmt_header = "INFO: ";
        break;
    case CL_TYPE_TRACE:
        fmt_header = "TRACE: ";
        break;
    case CL_TYPE_WARNING:
        fmt_header = "WARNING: ";
        break;
    case CL_TYPE_ERROR:
        fmt_header = "ERROR: ";
        break;
    case CL_TYPE_FATAL_ERROR:
        fmt_header = "FATAL ERROR: ";
        break;
    case CL_TYPE_DEFAULT:
        fmt_header = "";
        break;
    case CL_TYPE_MAX:
        assert(0 && "This should never be hit.");
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
                1;                        // For space

    m_lock();

    // Colours do not apply to files so the special code for colours are just printed instead of the colour changing. So if we are logging to a file we must
    // not include the colour codes.
    if (!s_context.stream_is_file) {
        final_fmt_size += strlen(s_cl_type_colors[(size_t)type]) + strlen(s_cl_type_colors[(size_t)CL_TYPE_DEFAULT]);
    }

    char* final_fmt = malloc(final_fmt_size);

    if (!final_fmt) {
        fprintf(stderr, "CLOGGER ERROR: malloc failed.");
        return;
    }

    if (s_context.stream_is_file) {
        sprintf(final_fmt, "[%s] %s%s", 
                time_str,
                fmt_header, 
                fmt
        );
    } else {
        sprintf(final_fmt, "[%s] %s%s%s%s", 
                time_str,
                s_cl_type_colors[(size_t)type], 
                fmt_header, 
                fmt, 
                s_cl_type_colors[(size_t)CL_TYPE_DEFAULT]
        );

    }

    vfprintf(s_context.stream, final_fmt, args);

    m_unlock();

    free(final_fmt);
}

void clog_set_log_output_stream(FILE* stream) {
    if (!check_init()) return; 

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
    if (!check_init()) return CL_ERROR_NOT_INITIALIZED; 

    FILE* file = fopen(filename, "w+");
    
    if (!file) {
        return CL_ERROR_FAILED_TO_OPEN_FILE;
    }

    clog_set_log_output_stream(file);


    m_lock();
    s_context.stream_is_file = true;
    m_unlock();
    
    return CL_ERROR_SUCCESS;
}

void clog_terminate() {
    if (!check_init()) return; 

    m_lock();
     
    if (s_context.stream != NULL   && 
        s_context.stream != stdout && 
        s_context.stream != stderr && 
        s_context.stream != stdin) {
        fclose(s_context.stream);
    }

    m_unlock();

    pthread_mutex_destroy(&s_mutex);
}
