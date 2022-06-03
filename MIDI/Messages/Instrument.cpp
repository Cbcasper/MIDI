//
// Created by Casper De Smet on 23/03/2022.
//

#include "Instrument.h"

namespace MIDI
{
    Instrument::Instrument(): portSpecificity(All), channelSpecificity(All), port(std::string()), channel(17)
    {}

    Instrument::Instrument(const std::string& port, int channel):
                           portSpecificity(Specific), channelSpecificity(Specific), port(port), channel(channel)
    {}

    bool Instrument::operator==(const Instrument& instrument) const
    {
        bool portsEqual = equal(portSpecificity == All || instrument.portSpecificity == All,
                                portSpecificity == None || instrument.portSpecificity == None,
                                port == instrument.port);
        bool channelsEqual = equal(channelSpecificity == All || instrument.channelSpecificity == All,
                                   channelSpecificity == None || instrument.channelSpecificity == None,
                                   channel == instrument.channel);
        return portsEqual && channelsEqual;
    }

    bool Instrument::equal(bool all, bool none, bool equal)
    {
        if (all)    return true;
        if (none)   return false;
        else        return equal;
    }
}