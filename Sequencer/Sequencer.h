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

#include "../Application/Application.h"
#include "./Timer.h"
#include "Metronome.h"

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
        Metronome metronome;

        std::future<void> masterFuture;
        std::vector<std::future<void>> trackFutures;

        std::shared_mutex tracksMutex;
        std::condition_variable_any tracksCV;
        std::vector<bool*> trackStatuses;

        bool running;
        bool clicking;

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
}


#endif //MIDIPLAYGROUND_SEQUENCER_H
