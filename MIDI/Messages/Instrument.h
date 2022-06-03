//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_INSTRUMENT_H
#define MIDIASSISTANT_INSTRUMENT_H

#include <string>

namespace MIDI
{
    class Instrument
    {
    public:
        enum Specificity
        {
            All,
            None,
            Specific
        };
        Specificity portSpecificity;
        Specificity channelSpecificity;
        std::string port;
        int channel;

        Instrument();
        Instrument(const std::string& port, int channel);

        bool operator==(const Instrument& instrument) const;
        static bool equal(bool all, bool none, bool equal);
    };

    using InstrumentPointer = std::shared_ptr<Instrument>;
}

namespace std
{
    template <>
    struct hash<MIDI::InstrumentPointer>
    {
        std::size_t operator()(const MIDI::InstrumentPointer& instrument) const
        {
            return hash<string>()(instrument->port) ^ (hash<int>()(instrument->channel) << 1);
        }
    };

    template <>
    struct equal_to<MIDI::InstrumentPointer>
    {
        constexpr bool operator()(const MIDI::InstrumentPointer& firstInstrument, const MIDI::InstrumentPointer& secondInstrument) const
        {
            return *firstInstrument == *secondInstrument;
        }
    };
}


#endif //MIDIASSISTANT_INSTRUMENT_H
