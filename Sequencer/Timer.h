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

#include "../Application/Application.h"

namespace System
{
    class Timer;
    using TimerPointer = std::shared_ptr<Timer>;

    class Timer
    {
    public:
        State::ApplicationPointer application;

        unsigned long ticks;
        bool running;

        std::shared_mutex signalMutex;
        std::condition_variable_any signalCV;
        std::vector<bool*> statusFlags;

        std::future<void> timerFuture;
        std::chrono::nanoseconds tickLength;

        static TimerPointer getInstance();

        void initialize(const State::ApplicationPointer& givenApplication);

        void timerThread();
        void subscribe(bool* status);
        void unsubscribe(bool* status);
        void statusOn();

        void setTempo(double microseconds);

        virtual ~Timer();

    private:
        static std::mutex instanceMutex;
        static TimerPointer instance;

        Timer();
    };
}


#endif //MIDIASSISTANT_TIMER_H
