//
// Created by Casper De Smet on 23/03/2022.
//

#include "Instrument.h"

namespace MIDI
{
    Instrument::Instrument(): allPorts(true), noPorts(false), port(std::string()), channel(CHANNELS_ALL)
    {}

    Instrument::Instrument(const std::string& port, int channel):
                           allPorts(false), noPorts(false), port(port), channel(channel)
    {}

    bool Instrument::operator==(const Instrument& instrument) const
    {
        bool portsEqual = equal(allPorts || instrument.allPorts,
                                noPorts || instrument.noPorts,
                                port == instrument.port);
        bool channelsEqual = equal(allChannels() || instrument.allChannels(),
                                   noChannels() || instrument.noChannels(),
                                   channel == instrument.channel);
        return portsEqual && channelsEqual;
    }

    bool Instrument::equal(bool all, bool none, bool equal)
    {
        if (all)    return true;
        if (none)   return false;
        else        return equal;
    }

    bool Instrument::allChannels() const
    {
        return channel == CHANNELS_ALL;
    }

    bool Instrument::noChannels() const
    {
        return channel == CHANNELS_NONE;
    }

    void Instrument::setAllPorts()
    {
        allPorts = true;
        noPorts = false;
        port = "";
    }

    void Instrument::setNoPorts()
    {
        allPorts = false;
        noPorts = true;
        port = "";
    }

    void Instrument::setPort(const std::string& newPort)
    {
        allPorts = false;
        noPorts = false;
        port = newPort;
    }

    bool Instrument::specificPort()
    {
        return !allPorts && !noPorts;
    }
}