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
        // Default cycle is first measure
        cycle = std::make_shared<Cycle>(0, ticksPerMeasure * 1);
    }

    Sequencer::~Sequencer()
    {
        stop();
    }

    // Master thread that listens to the timer, manages global tasks and directs track threads
    void Sequencer::masterThread()
    {
        // Subscribe to timer thread in order to get waken up at regular time intervals
        bool status = false;
        TimerPointer timer = Timer::getInstance();
        timer->subscribe(&status);

        std::shared_lock<std::shared_mutex> lock(timer->signalMutex);
        timer->signalCV.wait(lock, [&]{ return status; });
        while (running)
        {
            // Check cycle
            applicationState->currentTime = cycle->set(applicationState->currentTime + 1);

            // Wake up tracks
            trackStatusOn();
            tracksCV.notify_all();

            // Check metronome
            metronome->clickOnTick(applicationState->currentTime);

            // Wait until next tick
            status = false;
            timer->signalCV.wait(lock, [&]{ return status; });
        }
        // Cleanup
        timer->unsubscribe(&status);
        trackStatusOn();
        tracksCV.notify_all();
    }

    // Thread that plays recorded notes from a single track
    void Sequencer::trackThread(const State::TrackPointer& track)
    {
        // Subscribe to master thread
        bool status = false;
        trackStatuses.emplace_back(&status);
        std::shared_lock<std::shared_mutex> lock(tracksMutex);
        tracksCV.wait(lock, [&]{ return status; });
        while (running)
        {
            // Only play notes from the selected take
            State::TakePointer take = track->recordingTake;
            take->mutex.lock();
            // Play all messages at this tick
            for (const MIDI::MessagePointer& message: take->midiMessages[applicationState->currentTime])
            {
                // Feed to both track and director
                track->playMIDIMessage(std::make_pair(message, track->output));
                director->inputMIDIMessage(std::make_pair(message, track->input));
            }
            if (take->midiMessages[applicationState->currentTime].empty())
                take->midiMessages.erase(applicationState->currentTime);
            take->mutex.unlock();

            // Wait until next tick
            status = false;
            tracksCV.wait(lock, [&]{ return status; });
        }
    }

    void Sequencer::trackStatusOn()
    {
        // Make all tracks ready to wake up
        for (bool* status: trackStatuses)
            *status = true;
    }

    void Sequencer::start()
    {
        // Start up threads
        running = true;
        masterFuture = Utilities::makeThread([=] { return masterThread(); });
        for (const State::TrackPointer& track: applicationState->tracks)
            trackFutures.push_back(Utilities::makeThread([=] { return trackThread(track); }));
    }

    void Sequencer::stop()
    {
        // Can only stop when running
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
        // Start is at -1, because before every tick, the current time gets incremented, so in the first tick that value is 0
        applicationState->currentTime = -1;
    }
}
