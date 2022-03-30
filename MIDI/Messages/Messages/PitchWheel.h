//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_PITCHWHEEL_H
#define MIDIASSISTANT_PITCHWHEEL_H

#include "../Message.h"

namespace MIDI
{
    class PitchWheel: public Message
    {
    public:
        int pitchOffset;

        PitchWheel(const libremidi::message& message);

        std::string value() override;
        libremidi::message rawMessage(int channel) override;
    };
}

#endif //MIDIASSISTANT_PITCHWHEEL_H
