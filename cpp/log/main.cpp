#include <cstdlib>
#include <thread>

#include "MyLog.h"

#undef LOG_TAG
#define LOG_TAG "LoggerTest"

void foo(int id) {
    LOGE("This is a test thread : %d", id);
}

int main() {
    updateLogLevel();

    // wait 1 sec
    system("sleep 1");

    LOGV("This is verbose log");
    LOGD("This is debug log");
    LOGI("This is info log");
    LOGW("This is warning log");
    LOGE("This is error log");

    std::thread t1(foo, 1);
    std::thread t2(foo, 2);
    t1.join();
    t2.join();
}
