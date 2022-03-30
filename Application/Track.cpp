//
// Created by Casper De Smet on 23/03/2022.
//

#include "Track.h"

#include <iostream>

namespace State
{
    Track::Track(const std::string& port, int channel)
    {
        audioPlayer = MIDI::AudioPlayer();
        output = std::make_shared<MIDI::Instrument>(port, channel);
    }

    void Track::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        audioPlayer.processMIDIMessage(message->rawMessage(instrument->channel));
    }
}