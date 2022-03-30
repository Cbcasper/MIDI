//
// Created by Casper De Smet on 02/03/2022.
//

#include "ProgramChange.h"

namespace MIDI
{
    ProgramChange::ProgramChange(const libremidi::message& message):
                   Message(message, "Program Change")
    {
        programNumber = message[1];
    }

    std::string ProgramChange::value()
    {
        return std::to_string(programNumber);
    }

    libremidi::message ProgramChange::rawMessage(int channel)
    {
        return libremidi::message::program_change(channel, programNumber);
    }
}