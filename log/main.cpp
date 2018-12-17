#include "MyLog.h"

#undef LOG_TAG
#define LOG_TAG "LoggerTest"

int main() {
    updateLogLevel();

    LOGV("This is verbos log");
    LOGD("This is debug log");
    LOGI("This is info log");
    LOGW("This is warning log");
    LOGE("This is error log");
}