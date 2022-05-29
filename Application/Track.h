//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_TRACK_H
#define MIDIASSISTANT_TRACK_H

#include <vector>

#include "Take.h"
#include "../MIDI/Processor.h"
#include "../MIDI/AudioPlayer.h"
#include "../MIDI/Messages/Instrument.h"
#include "../Director/Harmonies/Harmony.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../Theory/TimeDivision.h"
#include "../MIDI/MessageFilter.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;

    class Track
    {
    public:
        ApplicationPointer application;

        MIDI::InstrumentPointer input;
        MIDI::InstrumentPointer output;
        MIDI::AudioPlayer audioPlayer;

        TakePointer mainTake;
        TakePointer recordingTake;
        std::vector<TakePointer> takes;

        std::map<int, int> soundingNotes;

        float height;

        std::vector<Music::HarmonyPointer> harmonies;

        Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output);

        void incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void updateSoundingNotes(const MIDI::MessagePointer& message);
        void stopRecording();

        bool hasSelectedHarmonies();
        void clearSelectedHarmonies();

        void addTake();

        bool equalTakes(std::vector<NoteSequences>& takeNoteSequences);
        void quantize();
    };

    using TrackPointer = std::shared_ptr<Track>;
}

#endif //MIDIASSISTANT_TRACK_H
