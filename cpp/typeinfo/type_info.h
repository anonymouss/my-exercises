#ifndef __TYPE_INFO_H__
#define __TYPE_INFO_H__

#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <type_traits>

#include <cxxabi.h>

// reference: https://www.cnblogs.com/zfyouxi/p/5060288.html

template <typename T, bool IsBase = false>
struct TypeInfo;

class Output {
public:
    explicit Output(std::string &str) : mString(str) {}
    Output &operator()() { return (*this); }

    template <typename T, typename... Args>
    Output &operator()(const T &o, const Args&... args) {
        out(o);
        return operator()(args...);
    }

    Output &compact() {
        mIsCompact = true;
        return (*this);
    }

private:
    bool mIsCompact = true;
    std::string &mString;

    template <typename T>
    bool isEmpty(const T &) { return false; }
    bool isEmpty(const char *str) {
        return (!str) || str[0] == 0;
    }

    template <typename T>
    void out(const T &o) {
        if (isEmpty(o)) return;
        if (!mIsCompact) mString += " ";
        using U = std::ostringstream;
        mString += (U() << o).str();
        mIsCompact = false;
    }
};

template <bool>
struct Bracket {
    Output &out;
    Bracket(Output &o, const char *str = nullptr) : out(o) {
        out("(").compact();
    }
    virtual ~Bracket() {
        out.compact()(")");
    }
};

template <>
struct Bracket<false> {
    Bracket(Output &o, const char *str = nullptr) {
        o(str);
    }
};

template <std::size_t N = 0>
struct Bound {
    Output &out;
    Bound(Output &o) : out(o) {}
    virtual ~Bound() {
        if constexpr(N == 0) {
            out("[]");
        } else {
            out("[").compact()(N).compact()("]");
        }
    }
};

template <bool, typename... P>
struct Parameter;

template <bool IsStart>
struct Parameter<IsStart> {
    Output &out;
    Parameter(Output &o) : out(o) {}
    virtual ~Parameter() {
        Bracket<IsStart>{out};
    }
};

template <bool IsStart, typename P1, typename... P>
struct Parameter<IsStart, P1, P...> {
    Output out;
    Parameter(Output &o) : out(o) {}
    virtual ~Parameter() {
        [this](Bracket<IsStart>&&) {
            TypeInfo<P1> { out };
            Parameter<false, P...> { out.compact() };
        } (Bracket<IsStart> { out, "," });
    }
};

template <typename T, bool IsBase>
struct TypeInfo {
    TypeInfo(const Output &o) : out(o) {
        char *realName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        out(realName);
        std::free(realName);
    }

    Output out;
};

#define TYPE_INFO__(TYPE)                                                       \
    template <typename T, bool IsBase>                                          \
    struct TypeInfo<T TYPE, IsBase> : TypeInfo<T, true> {                       \
        using base_t = TypeInfo<T, true>;                                       \
        using base_t::out;                                                      \
        TypeInfo(const Output &o) : base_t(o) { out(#TYPE); }                   \
    };

TYPE_INFO__(const)
TYPE_INFO__(volatile)
TYPE_INFO__(const volatile)
TYPE_INFO__(&)
TYPE_INFO__(&&)
TYPE_INFO__(*)

#define TYPE_INFO_ARRAY__(CV, BOUND, ...)                                               \
    template <typename T, bool IsBase __VA_ARGS__>                                      \
    struct TypeInfo<T CV [BOUND], IsBase> : TypeInfo<T CV, !std::is_array<T>::value> {  \
        using base_t = TypeInfo<T CV, !std::is_array<T>::value>;                        \
        using base_t::out;                                                              \
        Bound<BOUND> bound;                                                             \
        Bracket<IsBase> bracket;                                                        \
        TypeInfo(const Output &o) : base_t(o), bound(out), bracket(out) {}              \
    };

#define TYPE_INFO_ARRAY_CV__(BOUND, ...)                     \
    TYPE_INFO_ARRAY__(, BOUND, ,##__VA_ARGS__)               \
    TYPE_INFO_ARRAY__(const, BOUND, ,##__VA_ARGS__)          \
    TYPE_INFO_ARRAY__(volatile, BOUND, ,##__VA_ARGS__)       \
    TYPE_INFO_ARRAY__(const volatile, BOUND, ,##__VA_ARGS__)

#define TYPE_INFO_PLACEHOLDER
TYPE_INFO_ARRAY_CV__(TYPE_INFO_PLACEHOLDER)
TYPE_INFO_ARRAY_CV__(N, std::size_t N)

template <typename T, bool IsBase, typename... P>
struct TypeInfo<T(P...), IsBase> : TypeInfo<T, true> {
    using base_t = TypeInfo<T, true>;
    using base_t::out;

    Parameter<true, P...> parameter;
    Bracket<IsBase> bracket;

    TypeInfo(const Output &o) : base_t(o), parameter(out), bracket(out) {}
};

/*
template <typename T, bool IsBase, typename C, typename... P>
struct TypeInfo<T(C::*)(P...), IsBase> : TypeInfo<T(P...), true> {
    using base_t = TypeInfo<T(P...), true>;
    using base_t::out;

    TypeInfo(const Out &out) base_t(o) {
        TypeInfo<C>{out};
        out.compact()("::");
    }
};
*/

struct AtDestruct {
    Output &out;
    const char *str;

    AtDestruct(Output &o, const char *s = nullptr) : out(o), str(s) {}
    virtual ~AtDestruct() { out(str); }

    void setStr(const char *s = nullptr) { str = s; }
};

#define TYPE_INFO_MEM_FUNC__(...)                                   \
    template <typename T, bool IsBase, typename C, typename... P>   \
    struct TypeInfo<T(C::*)(P...) __VA_ARGS__, IsBase> {            \
        AtDestruct ad;                                              \
        TypeInfo<T(P...), true> base;                               \
        Output &out = base.out;                                     \
        TypeInfo(const Output &o) : ad(base.out), base(o) {       \
            ad.setStr(#__VA_ARGS__);                                \
            TypeInfo<C>{out};                                       \
            out.compact()("::");                                    \
        }                                                           \
    };

TYPE_INFO_MEM_FUNC__()
TYPE_INFO_MEM_FUNC__(const)
TYPE_INFO_MEM_FUNC__(volatile)
TYPE_INFO_MEM_FUNC__(const volatile)

template <typename T>
inline std::string my_type_info() {
    std::string str;
    TypeInfo<T>{Output(str)};
    return str;
}

#endif //  __TYPE_INFO_H__