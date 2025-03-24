#include "CLog.h"

int main(int argc, char** argv) {
    clog_init(stdout, 0);
    // clog_init_with_path("filename", 0) // Init with filename instead of stream.

    clog_log(CL_TYPE_INFO, "Hello CLog!\n");

    for (int i = 0; i < argc; i++) {
        clog_log(CL_TYPE_TRACE, "%s\n", argv[i]);
    }

    clog_set_log_output_stream(stderr);

    clog_log(CL_TYPE_WARNING, "a warning! %s\n", "CLog");
    clog_log(CL_TYPE_ERROR, "Oh no! %d\n", 10);
    clog_log(CL_TYPE_FATAL_ERROR, "Oh no!\n");

    clog_set_log_output_file("example.log");

    clog_log(CL_TYPE_TRACE, "Tracey things... %d\n", 10);
    clog_log(CL_TYPE_INFO, "Need to report some info... %d\n", 86);

    clog_terminate();

    return 0;
}
