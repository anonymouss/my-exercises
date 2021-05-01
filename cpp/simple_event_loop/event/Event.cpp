#include "Event.h"

#include <cstdio>
#include <string>

namespace sel {

Msg::Item::Item(int32_t i32) : type(INT32) { u.i32 = i32; }
Msg::Item::Item(const std::string &str)
    : type(INVALID) {
    u.str = new std::string(str.c_str());
    type = STRING;
}
Msg::Item::Item(std::shared_ptr<Object> obj)
    : type(INVALID) {
    // u.obj = std::make_shared<Object>();
    u.obj = new std::shared_ptr<Object>();
    *(u.obj) = obj;
    type = OBJECT;    
}

Msg::Item::Item(const Item &rhs) {
    type = rhs.type;
    if (type == INVALID) {
        return;
    } else if (type == OBJECT) {
        // u.obj = std::make_shared<Object>();
        u.obj = new std::shared_ptr<Object>();
        *(u.obj) = *(rhs.u.obj);
    } else if (type == STRING) {
        u.str = new std::string{rhs.u.str->c_str()};
    } else {
        u = rhs.u;
    }
}

Msg::Item::~Item() {
    if (type == OBJECT && u.obj != nullptr) {
        (*(u.obj)).reset();
        delete u.obj;
    } else if (type == STRING && u.str != nullptr) {
        delete u.str;
    }
}

}  // namespace sel