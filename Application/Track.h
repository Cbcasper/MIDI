//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_TRACK_H
#define MIDIASSISTANT_TRACK_H

#include "../MIDI/Processor.h"
#include "../MIDI/AudioPlayer.h"
#include "../MIDI/Messages/Instrument.h"
#include "../Director/Harmonies/Harmony.h"

namespace State
{
    class Track
    {
    public:
        MIDI::AudioPlayer audioPlayer;
        MIDI::InstrumentPointer output;

        std::vector<Music::HarmonyPointer> harmonies;

        Track(const std::string& port, int channel);

        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
    };

    using TrackPointer = std::shared_ptr<Track>;
}

#endif //MIDIASSISTANT_TRACK_H
