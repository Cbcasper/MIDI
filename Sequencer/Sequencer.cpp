//
// Created by Casper De Smet on 27/02/2022.
//

#include <iostream>

#include "Sequencer.h"
#include "../Director/Director.h"


namespace System
{
    Sequencer::Sequencer(const std::shared_ptr<State::Application>& applicationState,
                         const std::shared_ptr<Music::Director>& director):
                         applicationState(applicationState), director(director)
    {
        running = false;
    }

    Sequencer::~Sequencer()
    {
        stop();
    }

    void Sequencer::masterThread()
    {
        bool status = false;
        TimerPointer timer = Timer::getInstance();
        timer->subscribe(&status);

        std::shared_lock<std::shared_mutex> lock(timer->signalMutex);
        timer->signalCV.wait(lock, [&]{ return status; });
        while (running)
        {
            if (applicationState->currentTime) applicationState->currentTime++;

            trackStatusOn();
            tracksCV.notify_all();

            status = false;
            timer->signalCV.wait(lock, [&]{ return status; });
        }
        timer->unsubscribe(&status);
    }

    void Sequencer::trackThread()
    {
        bool status = false;
        trackStatuses.emplace_back(&status);
        std::shared_lock<std::shared_mutex> lock(tracksMutex);
        tracksCV.wait(lock, [&]{ return status; });
        while (running)
        {
            status = false;
            tracksCV.wait(lock, [&]{ return status; });
        }
    }

    void Sequencer::trackStatusOn()
    {
        for (bool* status: trackStatuses)
            *status = true;
    }

    void Sequencer::start()
    {
        running = true;
        masterFuture = Utilities::makeThread([=] { return masterThread(); });
        for (int i = 0; i < applicationState->tracks.size(); ++i)
            trackFutures.push_back(Utilities::makeThread([=] { return trackThread(); }));
    }

    void Sequencer::stop()
    {
        if (!running) return;
        running = false;
        for (const std::future<void>& future: trackFutures)
            future.wait();
        trackFutures.clear();
        masterFuture.wait();
        trackStatuses.clear();
    }

    void Sequencer::reset()
    {
        applicationState->currentTime = 0;
    }
}
