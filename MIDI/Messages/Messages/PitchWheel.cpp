//
// Created by Casper De Smet on 02/03/2022.
//

#include "PitchWheel.h"

namespace MIDI
{
    PitchWheel::PitchWheel(const libremidi::message& message):
                Message(message, "Pitch Wheel")
    {
        pitchOffset = ((unsigned short) message[2] << 7) | message[1];
    }

    std::string PitchWheel::value()
    {
        return std::to_string(pitchOffset);
    }

    libremidi::message PitchWheel::rawMessage(int channel)
    {
        return libremidi::message::pitch_bend(channel, pitchOffset);
    }
}