#pragma once

#include <functional>

namespace pxm {

    /**
    * Command design pattern a.k.a. C++ functor 
    * Decouple the object that invokes the operation from the one that knows how to perform it
    * All clients of Command objects treat each object as a "black box" by simply invoking the 
    * object's overloaded operator()() member function to execute the Command's "service".
    */
    typedef std::function<void()> command;

}