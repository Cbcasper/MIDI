//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIASSISTANT_DIRECTOR_H
#define MIDIASSISTANT_DIRECTOR_H

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>

#include "../Application/Application.h"
#include "../MIDI/Messages/Messages/NoteMessage.h"
#include "../MIDI/MessageFilter.h"
#include "../MIDI/Messages/Messages/NoteOff.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../MIDI/IO/IOManager.h"
#include "../Theory/Theory.h"

namespace Music
{
    class Director
    {
    public:
        std::shared_ptr<State::Application> applicationState;

        bool running;
        std::thread directorThread;

        std::mutex messageMutex;
        std::condition_variable messageCV;

        MIDI::MessageFilter messageFilter;
        std::queue<MIDI::MessageOnInstrument> messageQueue;

        explicit Director(const std::shared_ptr<State::Application>& applicationState);
        virtual ~Director();

        void messageHandler();
        void processMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
    };
}


#endif //MIDIASSISTANT_DIRECTOR_H
