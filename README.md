# CLog

A simple thread safe logger made in C that has good header documentation.

### How to build

CLog does not have any dependenices. The example will only be built if the cmake command CL_BUILD_EXAMPLE is on.

### Example

```c 
#include <clog.h>

int main(void) {
    // flags must always be 0. It is reserved.
    clog_init(stdout, 0);

    clog_log(CL_TYPE_INFO, "Hello CLog!\n");
    clog_log(CL_TYPE_TRACE, "trace!\n");
    clog_log(CL_TYPE_WARNING, "a warning! %s\n", "CLog");
    clog_log(CL_TYPE_ERROR, "Oh no! %d\n", 10);
    clog_log(CL_TYPE_FATAL_ERROR, "Oh no!\n");

    clog_terminate();

    return 0;
}
```
