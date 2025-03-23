#include "CLog.h"

int main(int argc, char** argv) {
    cl_init(stdout, 0);
    // cl_init_with_path("filename", 0) // Init with filename instead of stream.

    cl_log(CL_TYPE_INFO, "Hello CLogger!\n");

    for (int i = 0; i < argc; i++) {
        cl_log(CL_TYPE_TRACE, "%s\n", argv[i]);
    }

    cl_set_log_output_stream(stderr);

    cl_log(CL_TYPE_WARNING, "a warning! %s\n", "CLogger");
    cl_log(CL_TYPE_ERROR, "Oh no! %d\n", 10);
    cl_log(CL_TYPE_FATAL_ERROR, "Oh no!\n");

    cl_set_log_output_file("example.log");

    cl_log(CL_TYPE_TRACE, "Tracey things... %d\n", 10);
    cl_log(CL_TYPE_INFO, "Need to report some info... %d\n", 86);

    cl_terminate();

    return 0;
}
