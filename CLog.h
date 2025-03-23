#if !defined(CLOG_H)
#define CLOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

typedef enum CLogError {
    CL_ERROR_SUCCESS,
    CL_ERROR_NOT_INITIALIZED,
    CL_ERROR_ALREADY_INITIALIZED_CLOGGER,
    CL_ERROR_FAILED_TO_OPEN_FILE,
    CL_ERROR_FAILED_TO_CREATE_MUTEX,
    CL_ERROR_MALLOC_FAILED,
    CL_ERROR_MAX,
} CLogError;

typedef enum CLogType {
    CL_TYPE_INFO,
    CL_TYPE_TRACE,
    CL_TYPE_WARNING,
    CL_TYPE_ERROR,
    CL_TYPE_FATAL_ERROR,
    CL_TYPE_DEFAULT,
    CL_TYPE_MAX,
} CLogType;

typedef uint64_t CLogAttributeFlags;
typedef enum CLogAttributeFlagBits {
    // This flag is used internally do not pass this into an init function even cl_init_with_file as its handeled for you.
    CL_ATTRIB_STREAM_IS_FILE = 0x1,
} CLogAttributeFlagBits;

/**
 * @brief Initialize CLog with attribute flags. Do not pass a file stream (not stdout or stderr) into this function use cl_init_with_file instead. This function is not thread safe, call it from the main thread.
 * @param stream A reference to an output stream. Only use this function if you are outputting to stdout or stderr. Use cl_init_with_path if you want to initialize with a file instead.
 * @param flags Reserved. Never pass in anything other than 0.
 * @retval CLogError Returns CL_ERROR_SUCCESS if the function succeeded. if it didn't it can return CL_ERROR_ALREADY_INITIALIZED_CLOGGER if CLog was already initialized and CL_ERROR_FAILED_TO_CREATE_MUTEX if creating a mutex failed. 
 */
CLogError cl_init(FILE* stream, CLogAttributeFlags flags);
/**
 * @brief Same a cl_init but outputs to a file instead of a stream. This function is not thread safe, call it from the main thread.
 * @param filename The path to the file that will serve as CLog's output. If the file is not found then it will be created.
 * @param flags Reserved. Never pass in anything other than 0.
 * @retval CLogError Returns all the same values as cl_init but can also return CL_ERROR_FAILED_TO_OPEN_FILE if opening the file failed.
 */
CLogError cl_init_with_file(const char* filename, CLogAttributeFlags flags);
/**
 * @brief Log the fmt to the output stream. The fmt will be filled by the variable args. This function is thread safe.
 * @param type The type of Log can be CL_TYPE_INFO, CL_TYPE_TRACE, CL_TYPE_WARNING, CL_TYPE_ERROR, CL_TYPE_FATAL_ERROR, or CL_TYPE_DEFAULT (Use this if you want no colouring in the terminal).
 * @param fmt A null terminating string that format will be filled by variable args.
 * @param ... these will fill the format specifiers in the fmt.
 */
void cl_log(CLogType type, const char* fmt, ...);
/**
 * @brief Same as cl_log. Log the fmt to the output stream. The fmt will be filled by the variable args. This function is thread safe.
 * @param type The type of Log can be CL_TYPE_INFO, CL_TYPE_TRACE, CL_TYPE_WARNING, CL_TYPE_ERROR, CL_TYPE_FATAL_ERROR, or CL_TYPE_DEFAULT (Use this if you want no colouring in the terminal and no header).
 * @param fmt A null terminating string that format will be filled by variable args.
 * @param args A va_list that will fill the format specifiers in the fmt.
 */
void cl_vlog(CLogType type, const char* fmt, va_list args);
/**
 * @brief Set the output stream. Do not pass a file stream (not stdout or stderr) into this function. This function is thread safe.
 * @param stream The output stream must be stdout or stderr.
 */
void cl_set_log_output_stream(FILE* stream);
/**
 * @brief Set the output stream to a file. This function is thread safe.
 * @param filename The path to the file that will serve as CLog's output. If the file is not found then it will be created.
 * @retval CLogError Returns CL_ERROR_SUCCESS if the function succeeded otherwise it will return CL_ERROR_FAILED_TO_OPEN_FILE if it failed to open or create the file provided.
 */
CLogError cl_set_log_output_file(const char* filename);
/**
 * @brief Terminate CLog. This function is no thread safe call it when all other threads are joined as it can cause undefined behaviour otherwise.
 */
void cl_terminate();

#endif
