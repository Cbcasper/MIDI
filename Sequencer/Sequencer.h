//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIPLAYGROUND_SEQUENCER_H
#define MIDIPLAYGROUND_SEQUENCER_H

#include <memory>
#include <vector>
#include <future>
#include <chrono>
#include <shared_mutex>
#include <condition_variable>

#include "Timer.h"
#include "Cycle.h"
#include "Metronome.h"
#include "../Application/Application.h"

namespace Music
{
    class Director;
}

namespace System
{
    // Class that enables the sequencer.
    // Advance the current time, play the metronome, use the cycle and play the recorded notes
    class Sequencer
    {
    public:
        State::ApplicationPointer applicationState;
        std::shared_ptr<Music::Director> director;
        MetronomePointer metronome;
        CyclePointer cycle;

        // Thread management
        std::future<void> masterFuture;
        std::vector<std::future<void>> trackFutures;

        // Synchronization
        std::shared_mutex tracksMutex;
        std::condition_variable_any tracksCV;
        std::vector<bool*> trackStatuses;

        // Cleanup
        bool running;

        explicit Sequencer(const State::ApplicationPointer& applicationState,
                           const std::shared_ptr<Music::Director>& director);
        ~Sequencer();

        // Thread methods
        void masterThread();
        void trackThread(const State::TrackPointer& track);

        // Synchronization
        void trackStatusOn();

        // External interface
        void start();
        void stop();
        void reset();
    };
    using SequencerPointer = std::shared_ptr<Sequencer>;
}


#endif //MIDIPLAYGROUND_SEQUENCER_H
