#ifndef __SEL_BASE_H__
#define __SEL_BASE_H__

#include <cstdint>
#include <string>
#include <sys/time.h>
#include <sys/prctl.h>

namespace sel {

#define DISALLOW_EVIL_CONSTRUCTORS(className)           \
    className(const className &) = delete;              \
    className(const className &&) = delete;             \
    className &operator=(const className &) = delete;   \
    className &operator=(const className &&) = delete;  \

enum status_t : uint32_t {
    OK = 0,
    BAD_VALUE,
    NOT_FOUND,
    BAD_STATE,
    REFUSED,
    ERROR,
};

struct Linux {
    static void SetThreadName(const std::string &name) {
        prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
    }

    static uint64_t CurrentTimeUs() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1e6 + tv.tv_usec;
    }
};

}  // namespace sel

#endif  // __SEL_BASE_H__