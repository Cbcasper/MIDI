//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIPLAYGROUND_PROCESSOR_H
#define MIDIPLAYGROUND_PROCESSOR_H

#include "Messages/Message.h"
#include "Messages/Instrument.h"

namespace State
{
    class Application;
}

namespace Music
{
    class Director;
}

namespace MIDI
{
    using MessageOnInstrument = std::pair<MessagePointer, InstrumentPointer>;
    class Processor
    {
    public:
        std::shared_ptr<State::Application> applicationState;
        std::shared_ptr<Music::Director> director;

        Processor(const std::shared_ptr<State::Application>& applicationState, const std::shared_ptr<Music::Director>& director);

        void processMIDIMessage(const libremidi::message& message, const std::string& port) const;
        static std::unique_ptr<Message> convertMIDIMessage(const libremidi::message& message);

        void updatePorts(const std::vector<std::string>& inputPorts, const std::vector<std::string>& outputPorts);
        void initializePorts(const std::vector<std::string>& inputPorts, const std::vector<std::string>& outputPorts);
    };
}


#endif //MIDIPLAYGROUND_PROCESSOR_H
