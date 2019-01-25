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
    explicit Defer(F &&f) : _f(std::forward<F>(f)) {}
    ~Defer() {
        _f();
    }

    Defer(Defer &&that) : _f(std::move(that._f)) {}

    Defer() = delete;
    Defer(const Defer &) = delete;
    void operator=(const Defer &) = delete;
    void operator=(Defer &&) = delete;

private:
    F _f;
};

// this function helps to deduce lambda type (F)
template<typename F>
auto MakeObject(F &&f) {
    return Defer<F>(std::forward<F>(f));
}

#endif // __DEFER_H__