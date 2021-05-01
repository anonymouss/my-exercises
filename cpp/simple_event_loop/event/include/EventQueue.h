#ifndef __SEL_EVENT_QUEUE_H__
#define __SEL_EVENT_QUEUE_H__

#include "Base.h"
#include "Event.h"

#include <atomic>
#include <condition_variable>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

namespace sel {

class EventQueue;

class EventHandler : public virtual std::enable_shared_from_this<EventHandler> {
public:
    EventHandler();
    virtual ~EventHandler() = default;

    status_t setEventQueue(std::shared_ptr<EventQueue> eventQueue);
    status_t postAsync(std::shared_ptr<Event> event);
    status_t postAsyncWithDelay(std::shared_ptr<Event> event, uint32_t delayMs);
    status_t postAndAwait(std::shared_ptr<Event> event);

protected:
    virtual status_t onEvent(std::shared_ptr<Event> event) = 0;

private:
    DISALLOW_EVIL_CONSTRUCTORS(EventHandler);
    friend class EventQueue;
    std::shared_ptr<EventQueue> mEventQueue;

    status_t post(std::shared_ptr<Event> event, bool sync = true, uint32_t delayMs = 0);
};  // class EventHandler

class EventQueue {
public:
    explicit EventQueue(const std::string &name);
    virtual ~EventQueue();

    status_t start();
    status_t stop();

    static status_t threadWrapper(EventQueue &queue);

private:
    DISALLOW_EVIL_CONSTRUCTORS(EventQueue);

    struct EventItem {
        EventItem(std::shared_ptr<Event> event, std::shared_ptr<EventHandler> handler, bool isSync,
                uint64_t scheduledTimeUs, uint64_t delayedTimeUs)
            : mEvent(event), mHandler(handler), mIsSynchronous(isSync), mScheduledTimeUs(scheduledTimeUs),
              mDelayedTimeUs(delayedTimeUs), mPostedThreadId(std::this_thread::get_id()) {}

        std::shared_ptr<Event> mEvent;
        std::weak_ptr<EventHandler> mHandler;
        bool mIsSynchronous;
        uint64_t mScheduledTimeUs;
        uint64_t mDelayedTimeUs;
        std::thread::id mPostedThreadId;
    };

    std::string mName;
    mutable std::mutex mQueueLock;
    std::list<EventItem> mQueue;
    std::condition_variable mQueueCondition;
    mutable std::mutex mSyncAwaitLock;
    std::condition_variable mSyncAwaitCondition;

    std::shared_ptr<std::thread> mThread;
    enum class ThreadState : uint32_t {
        UNINIT = 0,
        RUNNING,
        STOPPING,
    };
    ThreadState mThreadState;
    std::thread::id mThisThreadId;

    friend class EventHandler;

    status_t threadLoop();
    status_t postEvent(std::shared_ptr<Event> event, std::shared_ptr<EventHandler> handler,
            bool isSync, uint32_t delayMs);
    std::atomic_bool mStopRequested;
};  // class EventQueue

}  // namespace sel

#endif  // __SEL_EVENT_QUEUE_H__