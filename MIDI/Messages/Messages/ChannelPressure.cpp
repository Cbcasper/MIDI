//
// Created by Casper De Smet on 02/03/2022.
//

#include "ChannelPressure.h"

namespace MIDI
{
    ChannelPressure::ChannelPressure(const libremidi::message& message):
                     Message(message, "Channel Pressure")
    {
        pressure = message[1];
    }

    std::string ChannelPressure::value()
    {
        return std::to_string(pressure);
    }

    libremidi::message ChannelPressure::rawMessage(int channel)
    {
        return libremidi::message::poly_pressure(channel, pressure, 0);
    }
}