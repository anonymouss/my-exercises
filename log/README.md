Simple Logger imitates Android ALOG

just set `log.level` property in file `props`

```
2018-12-18 | 13:18:49:028 | I: Core : updateLogLevel: 15 | update log level to : 2
2018-12-18 | 13:18:50:030 | I: LoggerTest : main: 15 | This is info log
2018-12-18 | 13:18:50:030 | W: LoggerTest : main: 16 | This is warning log
2018-12-18 | 13:18:50:030 | E: LoggerTest : main: 17 | This is error log
```

**TODO**: imitate Android `logcat`