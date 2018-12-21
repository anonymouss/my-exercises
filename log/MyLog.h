#ifndef __LOG_MY_LOG_H__
#define __LOG_MY_LOG_H__

#include <cstdint>
#include <chrono>
#include <cstdio>
#include <thread>

// #define USE_POSIX // don't use posix tid by default, it's too large

#ifdef linux // linux platform
#include <unistd.h>
#ifdef USE_POSIX // use posix tid
#define gettid() std::this_thread::get_id()
#else // don't use posix tid
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif // end USE_POSIX
#define TID gettid()
#define PID getpid()
#else // non linux platform
// only support linux
#define PID = -1
#define TID = -1
#endif // end linux

void updateLogLevel();
int getProperty(const char *key, char *value, const char *defaultValue);
const char *toStr(const int level);

enum LogLevel : uint32_t {
    LOG_LEVEL_VERBOSE   = 0x0,
    LOG_LEVEL_DEBUG     = 0x1,
    LOG_LEVEL_INFO      = 0x2,
    LOG_LEVEL_WARNING   = 0x3,
    LOG_LEVEL_ERROR     = 0x4,
};

static const char *kFileName = "props";
static const char *kLogsLevelProperty = "log.level";
extern uint32_t gLogLevel;

#define LOG_TAG "Core"

#define TRIM_STR(n, str) str[n]

// XXX: RISK: is fprintf() thread safe ?
#define TS_PRINTF(fmt, x...) {                                                              \
    auto tp = std::chrono::system_clock::now();                                             \
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);                  \
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tp - seconds);\
    auto tt = std::chrono::system_clock::to_time_t(tp);                                     \
    struct tm *ptm = localtime(&tt);                                                        \
    fprintf(stdout, "%d-%02d-%02d | %02d:%02d:%02d:%03d | %d %ld | " fmt "\n",              \
        (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,                  \
        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec, milliseconds,                \
        PID, TID, ##x);                                                                     \
}

#define LOG(level, fmt, x...) {                                                                     \
    if (level >= gLogLevel)                                                                         \
        TS_PRINTF("%c: %s : %s: %d | " fmt, TRIM_STR(10, #level), LOG_TAG, __func__, __LINE__, ##x);\
}

#define LOGV(fmt, x...) LOG(LOG_LEVEL_VERBOSE, fmt, ##x)
#define LOGD(fmt, x...) LOG(LOG_LEVEL_DEBUG, fmt, ##x)
#define LOGI(fmt, x...) LOG(LOG_LEVEL_INFO, fmt, ##x)
#define LOGW(fmt, x...) LOG(LOG_LEVEL_WARNING, fmt, ##x)
#define LOGE(fmt, x...) LOG(LOG_LEVEL_ERROR, fmt, ##x)

#endif // __LOG_MY_LOG_H__
