#ifndef __SEL_EVENT_H__
#define __SEL_EVENT_H__

#include "Base.h"

#include <cstdio>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace sel {

class Msg {
public:
    struct Object {
        virtual ~Object() = default;
    };

    Msg() = default;

    template <typename T>
    status_t put(const std::string &key, T value) {
        auto it = mMap.find(key);
        if (it != mMap.end()) {
            mMap.erase(it);
        }
        auto result = mMap.emplace(key, Item{value});
        if (!result.second) {
            printf("faile to put key : %s\n", key.c_str());
            return ERROR;
        }
        return OK;
    }

    template <typename T>
    status_t get(const std::string &key, T *value) const {
        if (!value) { return BAD_VALUE; }
        auto it = mMap.find(key);
        if (it != mMap.end()) {
            return it->second.assignTo(value);
        } else {
            return NOT_FOUND;
        }
    }

    template <typename T>
    status_t get(const std::string &key, std::shared_ptr<T> *value) const {
        if (!value) { return BAD_VALUE; }
        if (!std::is_base_of<Object, T>::value) { return ERROR; }
        auto it = mMap.find(key);
        if (it != mMap.end()) {
            std::shared_ptr<Object> obj;
            auto ret = it->second.assignTo(&obj);
            if (ret == OK) {
                *value = std::static_pointer_cast<T>(obj);
            }
            return ret;
        } else {
            return NOT_FOUND;
        }
    }

    bool hasKey(const std::string &key) const {
        return mMap.count(key) > 0;
    }

    void copyTo(Msg *other) const {
        if (other) {
            for (auto &kv : mMap) {
                other->mMap.emplace(kv.first, Item(kv.second));
            }
        }
    }

    void clear() { mMap.clear(); }

private:
    DISALLOW_EVIL_CONSTRUCTORS(Msg);

    struct Item {
        explicit Item(int32_t);
        explicit Item(const std::string &);
        explicit Item(std::shared_ptr<Object>);

        Item(const Item &rhs);
        virtual ~Item();

        enum Type : uint32_t {
            INVALID = 0,
            INT32,
            STRING,
            OBJECT,
        } type;

        union {
            int32_t i32;
            std::string *str;
            std::shared_ptr<Object> *obj;
        } u;

        template <typename T>
        status_t assignTo(T *target) const {
            if (std::is_same<T, int32_t>::value && (type == INT32)) {
                *(reinterpret_cast<int32_t *>(target)) = u.i32;
                return OK;
            }
            return BAD_VALUE;
        }

        status_t assignTo(std::string *target) const {
            if (type != STRING || u.str == nullptr) {
                return BAD_VALUE;
            }
            *target = *(u.str);
            return OK;
        }

        status_t assignTo(std::shared_ptr<Object> *target) const {
            if (type != OBJECT || u.obj == nullptr) {
                return BAD_VALUE;
            }
            *target = *(u.obj);
            return OK;
        }
    };

    std::unordered_map<std::string, Item> mMap;
};  // class Msg

class Event {
public:
    explicit Event(uint32_t id) : mId(id), mRetired(false) {}

    uint32_t id() const { return mId; }

    Msg &message() { return mMessage; }
    const Msg &message() const { return mMessage; }

    Msg &reply() { return mReply; }
    const Msg &reply() const { return mReply; }

    void retire() { mRetired = true; }
    bool isRetired() const { return mRetired; }

private:
    DISALLOW_EVIL_CONSTRUCTORS(Event);

    const uint32_t mId;
    bool mRetired;
    Msg mMessage;
    Msg mReply;
};  // class Event

}  // namespace sel

#endif  // __SEL_EVENT_H__