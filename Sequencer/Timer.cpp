//
// Created by Casper De Smet on 29/03/2022.
//

#include "Timer.h"
#include "../Utilities/Utilities.h"

namespace System
{
    std::mutex Timer::instanceMutex;
    TimerPointer Timer::instance;

    Timer::Timer()
    {
        // TODO: decide tick length
        ticks = 0;
        running = false;
        timerFuture = Utilities::makeThread([=] { return timerThread(); });
    }

    Timer::~Timer()
    {
        timerFuture.wait();
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
}