#pragma once

#include <array>
#include <atomic>

#include "error_messages.h"

namespace pxm {

    template<typename T, size_t CAPACITY>
    class spsc_concurrent_bounded_queue {

        static_assert((CAPACITY + 1 & (CAPACITY)) == 0, ERR_CAPACITY_POWER2);

        static constexpr size_t Capacity = CAPACITY + 1;

    public:

        typedef T value_type;
        typedef std::array<value_type, Capacity> container_type;

        explicit spsc_concurrent_bounded_queue();

        bool try_push(const T& item);

        bool try_pop(T& item);

        size_t size_guess();

        bool was_empty();

        bool was_full();

        size_t capacity();

    private:

        std::atomic<size_t> head;
        std::atomic<size_t> tail;
        size_t mask;
        std::array<T, Capacity> q;

    };




    template<typename T, size_t CAPACITY>
    inline spsc_concurrent_bounded_queue<T, CAPACITY>::spsc_concurrent_bounded_queue() :
        head(0),
        tail(0),
        mask(CAPACITY),
        q({})
    {}

    template<typename T, size_t CAPACITY>
    inline bool spsc_concurrent_bounded_queue<T, CAPACITY>::try_push(const T & item) {
        const auto current_tail = tail.load();
        const auto next_tail = (current_tail + 1) & mask;
        if (next_tail != head.load()) {
            q[current_tail] = item;
            tail.store(next_tail);
            return true;
        }
        else {
            return false;
        }
    }

    template<typename T, size_t CAPACITY>
    inline bool spsc_concurrent_bounded_queue<T, CAPACITY>::try_pop(T& item) {
        const auto current_head = head.load();
        if (current_head == tail.load()) {
            return false;
        }
        else {
            item = q[current_head];
            head.store((current_head + 1) & mask);
            return true;
        }
    }

    template<typename T, size_t CAPACITY>
    inline size_t spsc_concurrent_bounded_queue<T, CAPACITY>::size_guess() {
        return (tail - head + Capacity) & mask;
    }

    template<typename T, size_t CAPACITY>
    inline bool spsc_concurrent_bounded_queue<T, CAPACITY>::was_empty() {
        return head.load() == tail.load();
    }

    template<typename T, size_t CAPACITY>
    inline bool spsc_concurrent_bounded_queue<T, CAPACITY>::was_full() {
        return head.load() == ((tail.load() + 1) & mask);
    }

    template<typename T, size_t CAPACITY>
    inline size_t spsc_concurrent_bounded_queue<T, CAPACITY>::capacity() {
        return mask;
    }

}