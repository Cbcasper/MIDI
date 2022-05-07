//
// Created by Casper De Smet on 29/03/2022.
//

#include "Timer.h"
#include "../Utilities/Utilities.h"

namespace System
{
    std::mutex Timer::instanceMutex;
    TimerPointer Timer::instance;

    Timer::Timer(): tickLength(std::chrono::nanoseconds(500000000 / 960)) // 120 bpm in nanoseconds
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

    void Timer::timerThread()
    {
        while (running)
        {
            ticks++;
            statusOn();
            signalCV.notify_all();
            if (running) std::this_thread::sleep_for(tickLength);
        }
        statusOn();
        signalCV.notify_all();
    }

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

    void Timer::setTempo(int microseconds)
    {
        double divisionsPerBeat = static_cast<double>(Music::Sixteenth) / static_cast<double>(application->song->timeDivision);
        double microsecondsPerTick = static_cast<double>(microseconds) / divisionsPerBeat / application->song->ticksPerDivision;
        tickLength = std::chrono::nanoseconds(static_cast<int>(round(microsecondsPerTick * 1000)));
    }
}