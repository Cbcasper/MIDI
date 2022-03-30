//
// Created by Casper De Smet on 27/02/2022.
//

#include "Processor.h"

#include <iostream>

#include "Messages/Messages/Aftertouch.h"
#include "Messages/Messages/ChannelPressure.h"
#include "Messages/Messages/Controller.h"
#include "Messages/Messages/NoteOff.h"
#include "Messages/Messages/NoteOn.h"
#include "Messages/Messages/PitchWheel.h"
#include "Messages/Messages/ProgramChange.h"
#include "Messages/Instrument.h"
#include "../Director/Director.h"

namespace MIDI
{
    Processor::Processor(const std::shared_ptr<State::Application>& applicationState,
                         const std::shared_ptr<Music::Director>& director):
                         applicationState(applicationState), director(director)
    {}

    void Processor::processMIDIMessage(const libremidi::message& message, const std::string& port) const
    {
        MessagePointer convertedMessage = convertMIDIMessage(message);
        InstrumentPointer instrument = std::make_shared<Instrument>(port, message.get_channel());
        applicationState->inputMIDIMessage(std::make_pair(convertedMessage, instrument));
        director->inputMIDIMessage(std::make_pair(convertedMessage, instrument));
    }

    std::unique_ptr<Message> Processor::convertMIDIMessage(const libremidi::message& message)
    {
        switch (message.get_message_type())
        {
            case libremidi::message_type::NOTE_OFF:         return std::make_unique<NoteOff>(message);
            case libremidi::message_type::NOTE_ON:          return std::make_unique<NoteOn>(message);
            case libremidi::message_type::POLY_PRESSURE:    return std::make_unique<ChannelPressure>(message);
            case libremidi::message_type::CONTROL_CHANGE:   return std::make_unique<Controller>(message);
            case libremidi::message_type::PROGRAM_CHANGE:   return std::make_unique<ProgramChange>(message);
            case libremidi::message_type::AFTERTOUCH:       return std::make_unique<Aftertouch>(message);
            case libremidi::message_type::PITCH_BEND:       return std::make_unique<PitchWheel>(message);
            default:                                        return nullptr;
        }
    }

    void Processor::updatePorts(const std::vector<std::string>& inputPorts, const std::vector<std::string>& outputPorts)
    {
        applicationState->inputPorts = inputPorts;
        applicationState->outputPorts = outputPorts;
    }
}