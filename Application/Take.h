//
// Created by Casper De Smet on 17/05/2022.
//

#ifndef MIDIASSISTANT_TAKE_H
#define MIDIASSISTANT_TAKE_H

#include <map>
#include <set>
#include <list>
#include <array>
#include <unordered_map>

#include "../MIDI/Processor.h"
#include "../MIDI/Messages/Instrument.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../Theory/TimeDivision.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;
    using UnfinishedNotes = std::array<MIDI::NoteOnPointer, 128>;
    using NoteSequences = std::array<MIDI::ChronologicNotes, 128>;

    // A collection of recorded notes
    class Take
    {
    public:
        ApplicationPointer application;

        // Guard the recorded notes collection
        std::mutex mutex;

        // Notes
        std::unordered_map<MIDI::InstrumentPointer, UnfinishedNotes> instrumentUnfinishedNotes;
        std::map<int, std::set<MIDI::MessagePointer>> midiMessages;
        std::set<MIDI::NoteOnPointer> selectedNotes;

        // Display purposes
        int lowestNote;
        int highestNote;

        explicit Take(const ApplicationPointer& application);

        // Record and link note messages
        void record(const MIDI::MessageOnInstrument& messageOnInstrument, int currentTime);
        void updateUnfinishedNotes(const MIDI::MessageOnInstrument& messageOnInstrument);
        void cleanupNotes(int currentTime);

        // Simple quantize
        void quantize(Music::TimeDivision quantizeDivision);
        int quantizeTick(int tick, Music::TimeDivision quantizeDivision);

        std::set<MIDI::NoteOnPointer> allNotes();
        // Selection
        bool noteSelected(const MIDI::NoteOnPointer& noteOn);
        void selectAllNotes();
        void selectNote(const MIDI::NoteOnPointer& noteOn, bool shifting);
        void deleteSelectedNotes();

        // Convert data to note sequences for quantization in parent track
        void getNoteSequences(NoteSequences& noteSequences);
    };
    using TakePointer = std::shared_ptr<Take>;
}

#endif //MIDIASSISTANT_TAKE_H
