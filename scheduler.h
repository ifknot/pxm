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

        scheduler(queue_t& q);

        scheduler(const scheduler&) = delete;

        void operator=(const scheduler&) = delete;

        ~scheduler();

    private:

        void run();

        std::atomic_bool done;
        queue_t& q; // command object queue
        std::unique_ptr<std::thread> t; // worker thread

    };

    template<typename T>
    inline scheduler<T>::scheduler(queue_t& q) :
        done(false),    // enable busy wait on command queue
        q(q),
        t(std::unique_ptr<std::thread>(new std::thread([this] {
            this->run();    // start the worker thread at construction
        })))
    {}

    template<typename T>
    inline scheduler<T>::~scheduler() {
        done.store(true);   // leave the busy wait loop on the command queue
        t->join();          // wait to finish up any remaining commands
    }

    template<typename T>
    inline void scheduler<T>::run() {
        command cmd;
        while (!done) { // busy wait for command
            if (q.try_pop(cmd)) { // pop a command
                cmd();  // execute it as a functor
            }
        }
        while (!q.was_empty()) { // finish any remaining commands
            if (q.try_pop(cmd)) {
                cmd();
            }
        }
    }

}