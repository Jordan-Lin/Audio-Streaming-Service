#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <mutex>
#include <vector>

template <typename T>
class LockedQueue {
public:
    void push(T elem) {
        std::lock_guard<std::mutex> guard(lock);
        queue.push_back(elem);
    }

    void pop() {
        std::lock_guard<std::mutex> guard(lock);
        queue.erase(queue.begin());
    }

    T front() {
        std::lock_guard<std::mutex> guard(lock);
        return queue.front();
    }

    int size() {
        std::lock_guard<std::mutex> guard(lock);
        queue.size();
    }
protected:
    std::mutex lock;
    std::vector<T> queue;
};

#endif
