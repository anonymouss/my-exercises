#include "Base.h"
#include "EventQueue.h"

#include <chrono>
#include <algorithm>
#include <cstdio>

using namespace std::chrono_literals;

namespace sel {

EventHandler::EventHandler() : mEventQueue(nullptr) {}

status_t EventHandler::setEventQueue(std::shared_ptr<EventQueue> eventQueue) {
    if (mEventQueue) {
        printf("Error: EventQueue is already set\n");
        return REFUSED;
    }
    if (!eventQueue) {
        printf("Error: trying to set invalid EventQueue\n");
        return BAD_VALUE;
    }
    mEventQueue = eventQueue;
    return OK;
}

status_t EventHandler::postAsync(std::shared_ptr<Event> event) {
    return post(event, false);
}

status_t EventHandler::postAsyncWithDelay(std::shared_ptr<Event> event, uint32_t delayMs) {
    return post(event, false, delayMs);
}

status_t EventHandler::postAndAwait(std::shared_ptr<Event> event) {
    return post(event, true);
}

status_t EventHandler::post(std::shared_ptr<Event> event, bool sync, uint32_t delayMs) {
    if (!event) {
        printf("Error: post invalid event\n");
        return BAD_VALUE;
    }
    if (!mEventQueue) {
        printf("Error: handler has no EventQueue\n");
        return BAD_STATE;
    }

    return mEventQueue->postEvent(event, shared_from_this(), sync, delayMs);
}


EventQueue::EventQueue(const std::string &name)
    : mName(name), mThread(nullptr), mThreadState(ThreadState::UNINIT), mStopRequested(false) {
    std::thread::id invalid_id;
    mThisThreadId = invalid_id;
}

EventQueue::~EventQueue() {
    if (mThreadState == ThreadState::RUNNING) {
        printf("event thread is still running, force stopping it...\n");
        stop();
    }
}

status_t EventQueue::start() {
    std::lock_guard<std::mutex> lock(mQueueLock);
    mStopRequested.store(false);
    if (mThreadState == ThreadState::UNINIT) {
        printf("start: creating thread for %s\n", mName.c_str());
        mThread = std::make_shared<std::thread>(EventQueue::threadWrapper, std::ref(*this));
    } else {
        printf("start: thread is already started\n");
        return BAD_STATE;
    }
    mThreadState = ThreadState::RUNNING;
    mThisThreadId = mThread->get_id();
    return OK;
}

status_t EventQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(mQueueLock);
        if (mThreadState != ThreadState::RUNNING) {
            printf("stop: thread is already stopped\n");
            return OK;
        }
        if (!mThread) {
            printf("stop: thread is gone\n");
            return BAD_STATE;
        }
        mThreadState = ThreadState::STOPPING;
        mStopRequested.store(true);
    }

    mQueueCondition.notify_one();
    if (std::this_thread::get_id() == mThisThreadId) {
        printf("ERROR: thread %s is attempting to join itself\n", mName.c_str());
        return ERROR;
    }

    if (mThread->joinable()) {
        mThread->join();
    }

    {
        std::lock_guard<std::mutex> lock(mQueueLock);
        mThread = nullptr;
        {
            std::thread::id invalid_id;
            mThisThreadId = invalid_id;
        }
        mThreadState = ThreadState::UNINIT;
    }
    return OK;
}

// static
status_t EventQueue::threadWrapper(EventQueue &queue) {
    return queue.threadLoop();
}

status_t EventQueue::threadLoop() {
    printf("entering %s thread-loop\n", mName.c_str());
    Linux::SetThreadName(mName);
    constexpr auto kTimeOutUs = 1000000us;
    auto timeOutUs = kTimeOutUs;

    while (!mStopRequested) {
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            if (mQueue.empty()) {
                mQueueCondition.wait_for(lock, timeOutUs);
                timeOutUs = kTimeOutUs;
                continue;
            }
            bool ready = false;
            EventItem item = mQueue.front();
            auto currentTimeUs = Linux::CurrentTimeUs();
            for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
                if ((*it).mScheduledTimeUs <= currentTimeUs) {
                    item = *it;
                    auto waitTimeMs = (currentTimeUs - (*it).mScheduledTimeUs) / 1000;
                    mQueue.erase(it);
                    ready = true;
                    break;
                } else if (((*it).mScheduledTimeUs - currentTimeUs) * 1us < timeOutUs) {
                    timeOutUs = ((*it).mScheduledTimeUs - currentTimeUs) * 1us;
                    timeOutUs = std::min(timeOutUs, kTimeOutUs);
                }
            }

            if (!ready) {
                mQueueCondition.wait_for(lock, timeOutUs);
                timeOutUs = kTimeOutUs;
                continue;
            }

            // let other threads add event
            lock.unlock();

            auto handler = item.mHandler.lock();
            if (!handler) {
                printf("Handler is expired\n");
            } else {
                if (item.mEvent) {
                    handler->onEvent(item.mEvent);
                } else {
                    printf("Invalid event\n");
                }
            }

            if (item.mIsSynchronous) {
                std::unique_lock<std::mutex> awaitLock(mSyncAwaitLock);
                mSyncAwaitCondition.notify_one();
                item.mEvent->retire();
            }
        }
    }
    printf("exiting %s thread-loop\n", mName.c_str());
    return OK;
}

status_t EventQueue::postEvent(std::shared_ptr<Event> event, std::shared_ptr<EventHandler> handler,
            bool isSync, uint32_t delayMs) {
    {
        std::lock_guard<std::mutex> lock(mQueueLock);

        auto newItem = EventItem{event, handler, isSync, Linux::CurrentTimeUs() + delayMs * 1000,
                delayMs * 100};
        mQueue.push_back(newItem);
        mQueueCondition.notify_one();
    }

    if (isSync) {
        std::unique_lock<std::mutex> awaitLock(mSyncAwaitLock);
        if (!event->isRetired()) {
            mSyncAwaitCondition.wait(awaitLock);
        }
    }
    return OK;
}

}  // namespace sel