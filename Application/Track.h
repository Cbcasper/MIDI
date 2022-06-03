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
#include "../Utilities/Zip.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;
    using ChronologicNotesZip = Utilities::Zip<MIDI::ChronologicNotes, MIDI::NoteOnPointer>;

    class Track
    {
    public:
        enum Quantization
        {
            Simple,
            Structured,
            TimeWindow
        };

        ApplicationPointer application;

        int preset;
        MIDI::InstrumentPointer input;
        MIDI::InstrumentPointer output;
        MIDI::AudioPlayerPointer audioPlayer;

        TakePointer recordingTake;
        std::list<TakePointer> takes;
        std::map<int, int> soundingNotes;

        std::list<Music::HarmonyPointer> harmonies;

        Quantization quantization;
        Music::TimeDivision quantizeDivision;
        int timeWindow;

        float height;

        Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output);

        void incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void updateSoundingNotes(const MIDI::MessagePointer& message);
        void stopRecording();

        void deleteHarmony(const Music::HarmonyPointer& harmony);
        bool hasSelectedHarmonies();
        void clearSelectedHarmonies();

        void addTake();
        void deleteTake();

        void quantize();
        bool equalTakes(std::vector<NoteSequences>& takeNoteSequences);
        void structuredQuantize();
        void timeWindowQuantize();
        int getLastMessage();

        void average(const TakePointer& take, const MIDI::ChronologicNotes& notes, int noteValue);

        static std::string quantizationName(Quantization quantization);
    };

    using TrackPointer = std::shared_ptr<Track>;
}

#endif //MIDIASSISTANT_TRACK_H
