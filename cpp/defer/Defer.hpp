#ifndef __DEFER_H__
#define __DEFER_H__

// #include <functional>
// as <Effective Modern C++> says, type deduction is better than std::function

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define DEFER_OBJ CONCAT(__DEFER_,  CONCAT(__func__, __LINE__))
#define defer(expr) const auto &DEFER_OBJ = MakeObject([&](){ expr })

template <typename F>
class Defer {
public:
    explicit Defer(F &&f) : _f(std::move(f)){}
    ~Defer() {
        _f();
    }


private:
    F _f;
};

// this function helps to deduce lambda type (F)
template<typename F>
auto MakeObject(F &&f) {
    return Defer<F>(std::move(f));
}

#endif // __DEFER_H__