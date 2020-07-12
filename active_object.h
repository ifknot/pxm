#pragma once

#include <stdexcept>

#include "spsc_concurrent_bounded_queue.h"
#include "scheduler.h"
#include "error_messages.h"

namespace pxm {

    /**
    * Active object design pattern decouples method execution from method invocation for objects 
    * that each reside in their own thread of control. The goal is to introduce concurrency, by 
    * using asynchronous method invocation and a scheduler for handling requests.
    * "Active Object an Object Behavioral Pattern for Concurrent Programming" 1996 R. Greg Lavender Douglas C. Schmidt
    * https://www.dre.vanderbilt.edu/~schmidt/PDF/Active-Objects.pdf
    */
    template<size_t Capacity>
    class active_object {

        using queue_t = spsc_concurrent_bounded_queue<pxm::command, Capacity>;

    public:

        active_object();

        active_object(const active_object&) = delete;

        void operator=(const active_object&) = delete;

        ~active_object() = default;

        void send(pxm::command m);

        bool busy(); 

    private:

        scheduler<queue_t> worker; 
        queue_t q;

    };

    template<size_t Capacity>
    inline active_object<Capacity>::active_object() : worker(q) {}

    template<size_t Capacity>
    inline void active_object<Capacity>::send(pxm::command m) {
        if (!q.try_push(m)) {
            throw std::runtime_error(ERR_QUEUE_FULL + __func__);
        }
    }

    template<size_t Capacity>
    inline bool active_object<Capacity>::busy() {
        return !q.was_empty();
    }

}