//
// Created by Casper De Smet on 29/03/2022.
//

#include <iostream>

#include "Timer.h"
#include "../Utilities/Utilities.h"

namespace System
{
    std::mutex Timer::instanceMutex;
    TimerPointer Timer::instance;

    Timer::Timer(): tickLength(8333) // 120 bpm in microseconds
    {
        ticks = 0;
        running = true;
        timerFuture = Utilities::makeThread([=] { return timerThread(); });
    }

    Timer::~Timer()
    {
        running = false;
        timerFuture.wait();
    }

    void Timer::initialize(const State::ApplicationPointer& givenApplication)
    {
        application = givenApplication;
        setTempo(application->song->tempo);
    }

    TimerPointer Timer::getInstance()
    {
        if (!instance)
        {
            std::unique_lock<std::mutex> lock(instanceMutex);
            if (!instance)
                instance = TimerPointer(new Timer());
        }
        return instance;
    }

    // Try to sleep as precise as possible
    // Adapted from https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
    void Timer::preciseSleep(double seconds) {
        using namespace std;
        using namespace std::chrono;

        static double estimate = 5e-6;
        static double mean = 5e-6;
        static double m2 = 0;
        static int64_t count = 1;

        while (seconds > estimate) {
            auto start = high_resolution_clock::now();
            this_thread::sleep_for(microseconds(1));
            auto end = high_resolution_clock::now();

            std::chrono::duration<double> observed = end - start;
            seconds -= observed.count();

            ++count;
            double delta = observed.count() - mean;
            mean += delta / count;
            m2   += delta * (observed.count() - mean);
            double stddev = sqrt(m2 / (count - 1));
            estimate = mean + stddev;
        }

        // spin lock
        auto start = steady_clock::now();
        while ((steady_clock::now() - start).count() / 1e9 < seconds);
    }

    // Thread that wakes up every tick and notifies other threads
    void Timer::timerThread()
    {
        while (running)
        {
            ticks++;
            statusOn();
            signalCV.notify_all();
            if (running) preciseSleep(tickLength / 1e6);
        }
        statusOn();
        signalCV.notify_all();
    }

    // Subscription logic
    void Timer::subscribe(bool* status)
    {
        statusFlags.push_back(status);
    }

    void Timer::unsubscribe(bool* status)
    {
        statusFlags.erase(std::find(statusFlags.begin(), statusFlags.end(), status));
    }

    void Timer::statusOn()
    {
        for (bool* status: statusFlags)
            *status = true;
    }

    // Convert from bpm to microseconds to tick
    void Timer::setTempo(int microseconds)
    {
        double divisionsPerBeat = static_cast<double>(Music::Sixteenth) / static_cast<double>(application->song->timeDivision);
        tickLength = static_cast<double>(microseconds) / divisionsPerBeat / application->song->ticksPerDivision;
    }
}