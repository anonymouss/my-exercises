#include <cstdlib>

#include "MyLog.h"

#undef LOG_TAG
#define LOG_TAG "LoggerTest"

int main() {
    updateLogLevel();

    // wait 1 sec
    system("sleep 1");

    LOGV("This is verbose log");
    LOGD("This is debug log");
    LOGI("This is info log");
    LOGW("This is warning log");
    LOGE("This is error log");
}