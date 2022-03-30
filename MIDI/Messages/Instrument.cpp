//
// Created by Casper De Smet on 23/03/2022.
//

#include "Instrument.h"

namespace MIDI
{
    Instrument::Instrument(): allPorts(true), allChannels(true), port(std::string()), channel(-1)
    {}

    Instrument::Instrument(int channel): allPorts(true), allChannels(false), port(std::string()), channel(channel)
    {}

    Instrument::Instrument(const std::string& port): allPorts(false), allChannels(true), port(port), channel(-1)
    {}

    Instrument::Instrument(const std::string& port, int channel):
                           allPorts(false), allChannels(false), port(port), channel(channel)
    {}

    bool Instrument::operator==(const Instrument& instrument)
    {
        return false;
    }
}