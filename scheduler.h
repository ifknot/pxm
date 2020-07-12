#pragma once

#include <thread>
#include <atomic>

#include "command.h"    // command object pattern

namespace pxm {

    /**
    * schedule commands for execution on a single thread 
    * busy wait for messages by spinning on the done flag
    * destruction is delayed until any remaining commands are processed
    * (to avoid this clear the command queue prior)
    */
    template<typename T>
    class scheduler {

    public:

        typedef T queue_t;  // a concurrent queue of heterogeneous command objects (a.k.a. functors)

        scheduler(queue_t& q) :
            done(false),    // enable busy wait on command queue
            q(q),
            t(std::unique_ptr<std::thread>(new std::thread([this] {
                this->run();    // start the worker thread at construction
            }))) 
        {}

        scheduler(const scheduler&) = delete;

        void operator=(const scheduler&) = delete;

        ~scheduler() {
            done.store(true);   // leave the busy wait loop on the command queue
            t->join();          // wait to finish up any remaining commands
        }

    private:

        void run() {
            command m;
            while (!done) { // busy wait for messages
                if (q.try_pop(m)) {
                    m();
                }
            }
            while (!q.was_empty()) { // finish remaining messages
                if (q.try_pop(m)) m();
            }
        }

        std::atomic_bool done;
        queue_t& q; // command object queue
        std::unique_ptr<std::thread> t; // worker thread

    };

}