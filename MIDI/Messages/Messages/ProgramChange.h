//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_PROGRAMCHANGE_H
#define MIDIASSISTANT_PROGRAMCHANGE_H

#include "../Message.h"

namespace MIDI
{
    class ProgramChange: public Message
    {
    public:
        int programNumber;

        ProgramChange(const libremidi::message& message);

        std::string value() override;
        libremidi::message rawMessage(int channel) override;
    };
}

#endif //MIDIASSISTANT_PROGRAMCHANGE_H
