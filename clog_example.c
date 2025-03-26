#include "clog.h"


int main(int argc, char** argv) {
    clog_init(stdout, CLOG_ATTRIB_FLAG_NONE);
    // clog_init_with_path("filename", 0) // Init with filename instead of stream.

    #if NDEBUG 

    clog_ignore_log_types(CLOG_TYPE_DEBUG);
    fprintf(stderr, "NOT DEBUG\n");

    #endif
    
    clog_log(CLOG_TYPE_INFO, "Hello CLog!\n");

    for (int i = 0; i < argc; i++) {
        clog_log(CLOG_TYPE_TRACE, "%s\n", argv[i]);
    }

    clog_log(CLOG_TYPE_DEBUG, "Something that should only be printed in debug mode.\n");

    clog_set_log_output_stream(stderr);

    clog_log(CLOG_TYPE_WARNING, "a warning! %s\n", "CLog");
    clog_log(CLOG_TYPE_ERROR, "Oh no! %d\n", 10);
    clog_log(CLOG_TYPE_FATAL_ERROR, "Oh no!\n");

    clog_set_log_output_file("example.log");

    clog_log(CLOG_TYPE_TRACE, "Tracey things... %d\n", 10);
    clog_log(CLOG_TYPE_INFO, "Need to report some info... %d\n", 86);

    clog_terminate();

    return 0;
}
