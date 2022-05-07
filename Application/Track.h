//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_TRACK_H
#define MIDIASSISTANT_TRACK_H

#include <set>
#include <unordered_map>

#include "../MIDI/Processor.h"
#include "../MIDI/AudioPlayer.h"
#include "../MIDI/Messages/Instrument.h"
#include "../Director/Harmonies/Harmony.h"
#include "../MIDI/Messages/Messages/NoteOff.h"
#include "../Theory/TimeDivision.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;
    using SoundingNotes = std::map<int, MIDI::NoteOffPointer*>;

    class Track
    {
    public:
        ApplicationPointer application;

        std::mutex mutex;

        MIDI::InstrumentPointer input;
        MIDI::InstrumentPointer output;
        MIDI::AudioPlayer audioPlayer;

        float height;

        std::unordered_map<MIDI::InstrumentPointer, SoundingNotes> instrumentSoundingNotes;
        std::map<int, std::set<MIDI::MessagePointer>> midiMessages;

        bool notesRecorded;
        int lowestNote;
        int highestNote;

        std::vector<Music::HarmonyPointer> harmonies;

        Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output);

        void incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void recordMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument, int tick = -1);
        void cleanupNotes();

        bool hasSelectedHarmonies();
        void clearSelectedHarmonies();

        void quantize(Music::TimeDivision quantizeDivision);
        int quantizeTick(int tick, Music::TimeDivision quantizeDivision);
    };

    using TrackPointer = std::shared_ptr<Track>;
}

#endif //MIDIASSISTANT_TRACK_H
