#pragma once

#include <memory>
#include <cstddef>
#include <iostream>

namespace allocators {

namespace static_arena {
    struct Storage {
        Storage(const Storage &) = delete;
        Storage(const Storage &&) = delete;

        void *buffer;
        size_t buffer_size;
        void *cur;
        size_t remaining_size;
    };
}

template <typename T>
class StaticArena {
public:
    using value_type = T;

    StaticArena(size_t size, void *buffer) noexcept:
        storage{},
        master_storage(nullptr)
    {
        storage.buffer = buffer;
        storage.buffer_size = size;
        reset();
    }

    template <typename U>
    StaticArena(const StaticArena<U> &from) noexcept:
        storage{},
        master_storage(&const_cast<StaticArena<U>&>(from).get_storage())
    {}

    StaticArena(const StaticArena& from) noexcept:
        storage{},
        master_storage(&const_cast<StaticArena&>(from).get_storage())
    {}

    T *allocate(std::size_t n) {
        static_arena::Storage &s = get_storage();

        T *ret = reinterpret_cast<T*>(std::align(alignof(T), sizeof(T) * n, s.cur, s.remaining_size));
        if(ret == nullptr) {
            throw std::bad_alloc();
        }
        
        s.cur = std::next(ret, n);
        if((uintptr_t)s.cur > (uintptr_t)s.buffer + s.buffer_size){
            throw std::bad_alloc();
        }

        return ret;
    }

    void reset() noexcept {
        if(master_storage) {
            return;
        }

        static_arena::Storage &s = get_storage();
        s.cur = s.buffer;
        s.remaining_size = s.buffer_size;
        std::align(alignof(T), sizeof(T), s.cur, s.remaining_size);
    }

    /// @brief Does nothing; 
    void deallocate(T *, std::size_t) noexcept {}

    static_arena::Storage &get_storage() noexcept {
        return master_storage ? *master_storage : storage;
    }
private:
    static_arena::Storage storage;
    static_arena::Storage *master_storage;
};

template <typename T, typename U>
bool operator==(const StaticArena<T>&, const StaticArena<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const StaticArena<T>&, const StaticArena<U>&) { return false; }

};
