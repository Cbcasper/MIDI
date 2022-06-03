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
    class Sequencer
    {
    public:
        State::ApplicationPointer applicationState;
        std::shared_ptr<Music::Director> director;
        MetronomePointer metronome;
        CyclePointer cycle;

        std::future<void> masterFuture;
        std::vector<std::future<void>> trackFutures;

        std::shared_mutex tracksMutex;
        std::condition_variable_any tracksCV;
        std::vector<bool*> trackStatuses;

        bool running;

        explicit Sequencer(const State::ApplicationPointer& applicationState,
                           const std::shared_ptr<Music::Director>& director);
        ~Sequencer();

        void masterThread();
        void trackThread(const State::TrackPointer& track);

        void trackStatusOn();

        void start();
        void stop();
        void reset();
    };
    using SequencerPointer = std::shared_ptr<Sequencer>;
}


#endif //MIDIPLAYGROUND_SEQUENCER_H
