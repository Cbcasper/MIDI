//
// Created by Casper De Smet on 27/02/2022.
//

#include <iostream>

#include "Sequencer.h"
#include "../Director/Director.h"


namespace System
{
    Sequencer::Sequencer(const State::ApplicationPointer& applicationState, const std::shared_ptr<Music::Director>& director):
                         applicationState(applicationState), director(director)
    {
        float divisionsPerBeat;
        float ticksPerBeat;
        float ticksPerMeasure;
        applicationState->song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, ticksPerMeasure);

        running = false;
        metronome = std::make_shared<Metronome>(applicationState);
        cycle = std::make_shared<Cycle>(0, ticksPerMeasure * 1);
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
            applicationState->currentTime = cycle->set(applicationState->currentTime + 1);

            trackStatusOn();
            tracksCV.notify_all();

            metronome->clickOnTick(applicationState->currentTime);

            status = false;
            timer->signalCV.wait(lock, [&]{ return status; });
        }
        timer->unsubscribe(&status);
        trackStatusOn();
        tracksCV.notify_all();
    }

    void Sequencer::trackThread(const State::TrackPointer& track)
    {
        bool status = false;
        trackStatuses.emplace_back(&status);
        std::shared_lock<std::shared_mutex> lock(tracksMutex);
        tracksCV.wait(lock, [&]{ return status; });
        while (running)
        {
            State::TakePointer take = track->recordingTake;
            take->mutex.lock();
            for (const MIDI::MessagePointer& message: take->midiMessages[applicationState->currentTime])
            {
                track->playMIDIMessage(std::make_pair(message, track->output));
                director->inputMIDIMessage(std::make_pair(message, track->input));
            }
            if (take->midiMessages[applicationState->currentTime].empty())
                take->midiMessages.erase(applicationState->currentTime);
            take->mutex.unlock();

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
        for (const State::TrackPointer& track: applicationState->tracks)
            trackFutures.push_back(Utilities::makeThread([=] { return trackThread(track); }));
    }

    void Sequencer::stop()
    {
        if (!running) return;
        running = false;
        for (const std::future<void>& future: trackFutures)
            future.wait();
        trackFutures.clear();
        trackStatuses.clear();

        masterFuture.wait();
    }

    void Sequencer::reset()
    {
        applicationState->currentTime = -1;
    }
}
