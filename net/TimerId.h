//
// Created by root on 17-11-21.
//

#ifndef ENGINE_TIMERID_H
#define ENGINE_TIMERID_H

//#include "./../base/Type.h"
#include <inttypes.h>

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId
{
public:
    TimerId()
            : timer_(NULL),
              sequence_(0)
    {
    }

    TimerId(Timer* timer, int64_t seq)
            : timer_(timer),
              sequence_(seq)
    {
    }

    // default copy-ctor, dtor and assignment are okay

    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};



#endif //ENGINE_TIMERID_H
