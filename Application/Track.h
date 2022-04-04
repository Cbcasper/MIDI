//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_TRACK_H
#define MIDIASSISTANT_TRACK_H

#include <unordered_map>

#include "../MIDI/Processor.h"
#include "../MIDI/AudioPlayer.h"
#include "../MIDI/Messages/Instrument.h"
#include "../Director/Harmonies/Harmony.h"
#include "../MIDI/Messages/Messages/NoteOff.h"

namespace State
{
    using SoundingNotes = std::map<int, MIDI::NoteOffPointer*>;

    class Track
    {
    public:
        Application* application;

        MIDI::InstrumentPointer input;
        MIDI::InstrumentPointer output;
        MIDI::AudioPlayer audioPlayer;

        std::unordered_map<MIDI::InstrumentPointer, SoundingNotes> soundingNotes;
        std::map<int, std::vector<MIDI::MessagePointer>> midiMessages;

        std::vector<Music::HarmonyPointer> harmonies;

        Track(Application* application, const std::string& port, int channel);

        void incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
    };

    using TrackPointer = std::shared_ptr<Track>;
}

#endif //MIDIASSISTANT_TRACK_H
