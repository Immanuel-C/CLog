#if !defined(CLOG_H)
#define CLOG_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

typedef enum clog_error {
    CLOG_ERROR_SUCCESS,
    CLOG_ERROR_NOT_INITIALIZED,
    CLOG_ERROR_ALREADY_INITIALIZED_CLOGGER,
    CLOG_ERROR_FAILED_TO_OPEN_FILE,
    CLOG_ERROR_FAILED_TO_CREATE_MUTEX,
    CLOG_ERROR_ALLOC_FAILED,
} clog_error_t;

typedef uint64_t clog_type_flags_t;
enum clog_type_flag_bits {
    CLOG_TYPE_INFO = 1,
    CLOG_TYPE_TRACE = 2,
    CLOG_TYPE_WARNING = 4, 
    CLOG_TYPE_ERROR = 8,
    CLOG_TYPE_FATAL_ERROR = 16,
    // Defualt has no header and no colour in console output. Its supposed to act like a printf.
    CLOG_TYPE_DEFAULT = 32,
    // Debug type will only log when NDEBUG is not defined.
    CLOG_TYPE_DEBUG = 64,
};

typedef uint64_t clog_attribute_flags_t;
// No flag bits defined, reserved for future use.
enum clog_attribute_flag_bits {
    CLOG_ATTRIB_FLAG_NONE = 0,
    // Log without colour. 
    CLOG_ATTRIB_FLAG_NO_COLOR = 1,
    // Overwrite text in log file.
    CLOG_ATTRIB_FLAG_OVERWRITE_FILES = 2,
};

/**
 * @brief Initialize CLog with attribute flags. Do not pass a file stream (not stdout or stderr) into this function use cl_init_with_file instead. This function is not thread safe, call it from the main thread.
 * @param stream A reference to an output stream. Only use this function if you are outputting to stdout or stderr. Use cl_init_with_path if you want to initialize with a file instead.
 * @param flags a bitfield that can hold values from clog_attribute_flag_bits. 
 * @retval clog_error_t Returns CLOG_ERROR_SUCCESS if the function succeeded. if it didn't it can return CLOG_ERROR_ALREADY_INITIALIZED_CLOGGER if clog was already initialized and CLOG_ERROR_FAILED_TO_CREATE_MUTEX if creating a mutex failed. 
 */
clog_error_t clog_init(FILE* stream, clog_attribute_flags_t flags);
/**
 * @brief Same a cl_init but outputs to a file instead of a stream. This function is not thread safe, call it from the main thread.
 * @param filename The path to the file that will serve as CLog's output. If the file is not found then it will be created.
 * @param flags a bitfield that can hold values from clog_attribute_flag_bits. 
 * @retval clog_error_t Returns all the same values as cl_init but can also return CLOG_ERROR_FAILED_TO_OPEN_FILE if opening the file failed.
 */
clog_error_t clog_init_with_file(const char* filename, clog_attribute_flags_t flags);
/**
 * @brief Log the fmt to the output stream. The fmt will be filled by the variable args. This function is thread safe.
 * @param type The type of log can be CLOG_TYPE_INFO, CLOG_TYPE_TRACE, CLOG_TYPE_WARNING, CLOG_TYPE_ERROR, CLOG_TYPE_FATAL_ERROR, or CLOG_TYPE_DEFAULT (Use this if you want no colouring in the terminal). This is not a bitfield only pass one type.
 * @param fmt A null terminating string that format will be filled by variable args.
 * @param ... these will fill the format specifiers in the fmt.
 * @retval clog_error_t Returns CLOG_ERROR_SUCCESS if the function succeeded. If the function fails then CLOG_ERROR_ALLOC_FAILED can be returned or CLOG_ERROR_NOT_INITIALIZED can be returned if clog is not initialized.
 */
clog_error_t clog_log(clog_type_flags_t type, const char* fmt, ...);
/**
 * @brief Same as cl_log. Log the fmt to the output stream. The fmt will be filled by the variable args. This function is thread safe.
 * @param type The type of log can be CLOG_TYPE_INFO, CLOG_TYPE_TRACE, CLOG_TYPE_WARNING, CLOG_TYPE_ERROR, CLOG_TYPE_FATAL_ERROR, or CLOG_TYPE_DEFAULT (Use this if you want no colouring in the terminal). This is not a bitfield only pass one type.
 * @param fmt A null terminating string that format will be filled by variable args.
 * @param args A va_list that will fill the format specifiers in the fmt.
 * @retval clog_error_t Returns all the same values as clog_log. 
 */
clog_error_t clog_vlog(clog_type_flags_t type, const char* fmt, va_list args);
/**
 * @brief Set the output stream. Do not pass a file stream (not stdout or stderr) into this function. This function is thread safe.
 * @param stream The output stream must be stdout or stderr.
 */
void clog_set_log_output_stream(FILE* stream);
/**
 * @brief Set the output stream to a file. This function is thread safe.
 * @param filename The path to the file that will serve as CLog's output. If the file is not found then it will be created.
 * @retval clog_error_t Returns CLOG_ERROR_SUCCESS if the function succeeded otherwise it will return CLOG_ERROR_FAILED_TO_OPEN_FILE if it failed to open or create the file provided.
 */
clog_error_t clog_set_log_output_file(const char* filename);
/**
 * @brief Set clog to not log anything that is passed into this function. This function is thread safe.
 * @param types This is a bitfield and multiple types can be passed in.
 */
void clog_ignore_log_types(clog_type_flags_t types);
/**
 * @brief Terminate CLog. This function is no thread safe call it when all other threads are joined as it can cause undefined behaviour otherwise.
 */
void clog_terminate();

#if defined(__cplusplus)
}
#endif

#endif
