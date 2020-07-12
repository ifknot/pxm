#pragma once

#include <array>
#include <atomic>

#include "error_messages.h"

namespace pxm {

    /**
    * Wait free, lock free, single producer, single consumer, bounded FIFO queue
    * Capacity must an unsigned (power of 2) - 1 to enable performance advantage of bitwise masking.
    */
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

        size_t abandon_all();

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
        // relaxed op, there are no synchronization or ordering constraints imposed on other reads or
        // writes, only this operation's atomicity is guaranteed 
        const auto current_tail = tail.load(std::memory_order_relaxed);
        // significant performance advantage over modulo operator (%)
        const auto next_tail = (current_tail + 1) & mask; 
        // load op, memory order performs the acquire operation on the affected memory location: 
        // no reads or writes in the current thread can be reordered before this load
        // all writes in other threads that release the same atomic variable are visible in the current thread
        if (next_tail != head.load(std::memory_order_acquire)) {
            q[current_tail] = item;
            // store op, memory order performs the release operation: 
            // no reads or writes in the current thread can be reordered after this store
            // all writes in the current thread are visible in other threads that acquire the same atomic variable 
            tail.store(next_tail, std::memory_order_release);
            return true;
        }
        else {
            return false;
        }
    }

    template<typename T, size_t CAPACITY>
    inline bool spsc_concurrent_bounded_queue<T, CAPACITY>::try_pop(T& item) {
        const auto current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire)) {
            return false;
        }
        else {
            item = q[current_head];
            head.store((current_head + 1) & mask, std::memory_order_release);
            return true;
        }
    }

    template<typename T, size_t CAPACITY>
    inline size_t spsc_concurrent_bounded_queue<T, CAPACITY>::abandon_all() {
        auto abandoned = size_guess();
        head = tail = 0;
        return abandoned;
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