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

namespace Music
{
    class Director;
}

namespace System
{
    class Sequencer
    {
    public:
        std::shared_ptr<State::Application> applicationState;
        std::shared_ptr<Music::Director> director;

        std::future<void> masterFuture;
        std::vector<std::future<void>> trackFutures;

        std::shared_mutex tracksMutex;
        std::condition_variable_any tracksCV;
        std::vector<bool*> trackStatuses;

        bool running;

        explicit Sequencer(const std::shared_ptr<State::Application>& applicationState,
                           const std::shared_ptr<Music::Director>& director);
        ~Sequencer();

        void masterThread();
        void trackThread();

        void trackStatusOn();

        void start();
        void stop();
        void reset();
    };
}


#endif //MIDIPLAYGROUND_SEQUENCER_H
