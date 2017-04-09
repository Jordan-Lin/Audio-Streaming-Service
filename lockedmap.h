#ifndef LOCKECMAP_H
#define LOCKECMAP_H

#include <mutex>
#include <map>
#include <vector>

template <typename Key,  typename Val>
class LockedMap {
public:
    void insert(Key key, Val& elem) {
        std::lock_guard<std::mutex> guard(lock);
        map.insert(std::pair<Key, Val>(key, elem));
    }

    void erase(Key key) {
        std::lock_guard<std::mutex> guard(lock);
        map.erase(map.find(key));
    }

    bool exists(Key id) {
        std::lock_guard<std::mutex> guard(lock);
        return map.find(id) != map.end();
    }

    int size() {
        std::lock_guard<std::mutex> guard(lock);
        return map.size();
    }

    const Val at(Key key) {
        std::lock_guard<std::mutex> guard(lock);
        return map.at(key);
    }

    const std::vector<const Val> getAll() {
        std::lock_guard<std::mutex> guard(lock);
        std::vector<const Val> all;
        for (auto& elem : map) {
            all.push_back(elem.second);
        }
        return all;
    }
private:
    std::mutex lock;
    std::map<Key, Val> map;
};

#endif
