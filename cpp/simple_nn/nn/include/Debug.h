#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>

namespace snn {

class Logger {
public:
    template<class T>
    Logger &operator<<(const T &t) {
#ifdef _DEBUG_
        std::cout << t;
#endif // _DEBUG_
        return *this;
    }

    Logger& operator<<(std::ostream &(*f)(std::ostream& o)) {
#ifdef _DEBUG_
        std::cout << f;
#endif // _DEBUG_
        return *this;
    };
};

static Logger gLogger;

} // namespace nn

#endif // __DEBUG_H__