#include "EventQueue.h"

#include <unistd.h>

#include <memory>
#include <cstdio>
#include <cstdint>
#include <string>

struct Object : public sel::Msg::Object {
    Object() = default;
    Object(int32_t n) : magic_number(n) {}
    int32_t magic_number = 0;
};

class EventQueueTest {
public:
    EventQueueTest() {
        mQueue = std::make_shared<sel::EventQueue>("test_EventQ");
        mHandler = std::make_shared<Handler>();
    }

    virtual ~EventQueueTest() {
        mQueue = nullptr;
        mHandler = nullptr;
    }

    void startTest() {
        mQueue->start();
        mHandler->setEventQueue(mQueue);
    
        auto ei32 = std::make_shared<sel::Event>(EVENT_TEST_INT32);
        ei32->message().put("i32-value", 123);
        ei32->message().put("producer-id", 0);
        printf("Test: post i32 value : 123\n");
        mHandler->postAsync(ei32);

        auto estr = std::make_shared<sel::Event>(EVENT_TEST_STRING);
        estr->message().put("string-value", "hello world");
        estr->message().put("producer-id", 1);
        printf("Test: post string value : hello world, delay 500ms\n");
        mHandler->postAsyncWithDelay(estr, 500); 

        auto obj = std::make_shared<Object>(888);
        auto eobj = std::make_shared<sel::Event>(EVENT_TEST_OBJECT);
        eobj->message().put("obj", obj);
        eobj->message().put("producer-id", 2);
        printf("Test: post obj addr : %p\n", obj.get());
        mHandler->postAndAwait(eobj);
        int32_t magic;
        eobj->reply().get("magic-number", &magic);
        printf("Test: await done, magic number : %d\n", magic);

        usleep(500000);
        mQueue->stop();
    }

private:
    enum EventType : uint32_t {
        EVENT_TEST_INT32,
        EVENT_TEST_STRING,
        EVENT_TEST_OBJECT,
    };

    class Handler : public sel::EventHandler {
    protected:
        virtual sel::status_t onEvent(std::shared_ptr<sel::Event> e) final {
            int32_t pId = 0;
            e->message().get("producer-id", &pId);
            printf("Test: handling event %u from %d\n", e->id(), pId);
            switch(e->id()) {
                case EventType::EVENT_TEST_INT32: {
                    int32_t value = -1;
                    e->message().get("i32-value", &value);
                    printf("Test: handle INT32, received value %d\n", value);
                    break;
                }
                case EventType::EVENT_TEST_STRING: {
                    std::string value = "";
                    e->message().get("string-value", &value);
                    printf("Test: handle STRING, received value %s\n", value.c_str());
                    break;
                }
                case EventType::EVENT_TEST_OBJECT: {
                    std::shared_ptr<Object> obj;
                    e->message().get("obj", &obj);
                    printf("Test: handle Object, received obj addr %p\n", obj.get());
                    e->reply().put("magic-number", obj->magic_number);
                    usleep(1000);
                    break;
                }
                default: {
                    printf("Test: unsupported event\n");
                    break;
                }
            }
            return sel::OK;
        }
    };

    std::shared_ptr<Handler> mHandler;
    std::shared_ptr<sel::EventQueue> mQueue;
};

int main() {
    EventQueueTest{}.startTest();
}