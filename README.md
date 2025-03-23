# CLog

A simple thread safe logger made in C that has good header documentation.

### How to build

CLog does not have any dependenices. The example will only be built if the cmake command CL_BUILD_EXAMPLE.

### Example

```c 
#include <CLog.h>

int main(void) {
    // flags must always be 0. It is reserved.
    cl_init(stdout, 0);

    cl_log(CL_TYPE_INFO, "Hello CLogger!\n");
    cl_log(CL_TYPE_TRACE, "trace!\n");
    cl_log(CL_TYPE_WARNING, "a warning! %s\n", "CLogger");
    cl_log(CL_TYPE_ERROR, "Oh no! %d\n", 10);
    cl_log(CL_TYPE_FATAL_ERROR, "Oh no!\n");

    cl_terminate();

    return 0;
}
```
