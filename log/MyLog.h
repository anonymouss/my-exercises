#ifndef __LOG_MY_LOG_H__
#define __LOG_MY_LOG_H__

#include <cstdint>
#include <chrono>
#include <cstdio>

void updateLogLevel();
int getProperty(const char *key, char *value, const char *defaultValue);

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

#define TS_PRINTF(fmt, x...) {                                                              \
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());       \
    struct tm *ptm = localtime(&tt);                                                        \
    fprintf(stdout, "%d-%02d-%02d | %02d:%02d:%02d | " fmt "\n",                            \
        (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,                  \
        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec,                              \
        ##x);                                                                               \
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