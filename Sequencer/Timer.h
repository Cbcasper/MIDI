//
// Created by Casper De Smet on 29/03/2022.
//

#ifndef MIDIASSISTANT_TIMER_H
#define MIDIASSISTANT_TIMER_H

#include <vector>
#include <chrono>
#include <future>
#include <shared_mutex>
#include <condition_variable>

namespace System
{
    class Timer;
    using TimerPointer = std::shared_ptr<Timer>;

    class Timer
    {
    public:
        unsigned long ticks;
        bool running;

        std::shared_mutex signalMutex;
        std::condition_variable_any signalCV;
        std::vector<bool*> statusFlags;

        std::future<void> timerFuture;
        std::chrono::microseconds tickLength;

        static TimerPointer getInstance();

        void timerThread();
        void subscribe(bool* status);
        void unsubscribe(bool* status);
        void statusOn();

        virtual ~Timer();

    private:
        static std::mutex instanceMutex;
        static TimerPointer instance;

        Timer();
    };
}


#endif //MIDIASSISTANT_TIMER_H
