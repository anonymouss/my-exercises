Simple Logger imitates Android ALOG

usage:

just set `log.level` property in file `props`

```cpp
#include "MyLog.h"

void func() {
    // use like printf()
    LOGI("this is an info log");
    LOGE("this is an error log at line %d", __LINE__);
}
```

output:

```
2018-12-21 | 18:00:13:792 | 10644 10644 | I: Core : updateLogLevel: 15 | update log level to : info(2)
2018-12-21 | 18:00:14:794 | 10644 10644 | I: LoggerTest : main: 21 | This is info log
2018-12-21 | 18:00:14:794 | 10644 10644 | W: LoggerTest : main: 22 | This is warning log
2018-12-21 | 18:00:14:794 | 10644 10644 | E: LoggerTest : main: 23 | This is error log
2018-12-21 | 18:00:14:794 | 10644 10647 | E: LoggerTest : foo: 10 | This is a test thread : 1
2018-12-21 | 18:00:14:795 | 10644 10648 | E: LoggerTest : foo: 10 | This is a test thread : 2
```

**TODO**: imitate Android `logcat`
