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

        // Synchronisation
        std::shared_mutex signalMutex;
        std::condition_variable_any signalCV;
        std::vector<bool*> statusFlags;

        // Track management
        std::future<void> timerFuture;
        double tickLength;

        static TimerPointer getInstance();

        void initialize(const State::ApplicationPointer& givenApplication);

        // Timing
        void preciseSleep(double seconds);
        void timerThread();

        // Subscription
        void subscribe(bool* status);
        void unsubscribe(bool* status);
        void statusOn();

        void setTempo(int microseconds);

        virtual ~Timer();

    private:
        // Singleton logic
        static std::mutex instanceMutex;
        static TimerPointer instance;

        Timer();
    };
}


#endif //MIDIASSISTANT_TIMER_H
