#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <mutex>
#include <queue>

template <typename T>
class LockedQueue {
public:
    void push(T elem) {
        std::lock_guard<std::mutex> guard(lock);
        queue.push(elem);
    }

    void pop() {
        std::lock_guard<std::mutex> guard(lock);
        queue.pop();
    }

    T front() {
        std::lock_guard<std::mutex> guard(lock);
        return queue.front();
    }

    int size() {
        std::lock_guard<std::mutex> guard(lock);
        queue.size();
    }
private:
    std::mutex lock;
    std::queue<T> queue;
};

#endif
