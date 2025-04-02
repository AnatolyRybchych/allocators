#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

namespace allocators {

namespace arena {

struct Buffer final{
    inline Buffer(size_t capacity) {
        data.reserve(capacity);
    }

    std::vector<uint8_t> data;
    std::unique_ptr<Buffer> next;
};

struct Context{
    std::unique_ptr<Buffer> buffer = nullptr;
    std::unique_ptr<Buffer> garbage;
};

};

template <typename T>
class Arena {
    static constexpr size_t MIN_BUFFER_SIZE = 1<<10;
public:
    using value_type = T;

    Arena(): master(nullptr){}

    template <typename U>
    Arena(const Arena<U> &from) noexcept:
        master(&const_cast<Arena<U>&>(from).get_context())
    {}

    Arena(const Arena &from) noexcept:
        master(&const_cast<Arena&>(from).get_context())
    {}

    T *allocate(std::size_t n) {
        arena::Context &ctx = get_context();
        if(!ctx.buffer) {
            ctx.buffer = std::unique_ptr<arena::Buffer>(new arena::Buffer(std::max<size_t>(MIN_BUFFER_SIZE, sizeof(T) * n)));
        }

        void *cur = & ctx.buffer->data[ ctx.buffer->data.size()];
        size_t remaining_size =  ctx.buffer->data.capacity() -  ctx.buffer->data.size();

        T *ret = reinterpret_cast<T*>(std::align(alignof(T), sizeof(T), cur, remaining_size));
        if(!ret || (uintptr_t)&ret[n] > (uintptr_t)&ctx.buffer->data[ctx.buffer->data.capacity()]) {
            std::unique_ptr<arena::Buffer> new_buffer(new arena::Buffer(std::max<size_t>( ctx.buffer->data.capacity() * 2, sizeof(T) * n)));
            ctx.buffer->next = std::move(ctx.garbage);
            ctx.garbage = std::move(ctx.buffer);
            ctx.buffer = std::move(new_buffer);

        }

        cur = & ctx.buffer->data[ ctx.buffer->data.size()];
        remaining_size =  ctx.buffer->data.capacity() -  ctx.buffer->data.size();
        ret = reinterpret_cast<T*>(std::align(alignof(T), sizeof(T), cur, remaining_size));
        remaining_size -= (intptr_t)std::next(ret, n) - (intptr_t)ret;

        ctx.buffer->data.resize( ctx.buffer->data.capacity() - remaining_size);
        return ret;
    }

    /// @brief Does nothing; 
    void deallocate(T *, std::size_t) noexcept {}

    void reset() noexcept {
        if(master) {
            return;
        }

        arena::Context &ctx = get_context();
        std::unique_ptr<arena::Buffer> garbage(std::move(ctx.garbage));
        ctx.buffer->data.clear();
    }

    arena::Context &get_context() noexcept {
        return master ? *master : context;
    }
private:

    arena::Context context;
    arena::Context *master;
};

template <typename T, typename U>
bool operator==(const Arena<T>&, const Arena<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const Arena<T>&, const Arena<U>&) { return false; }

}