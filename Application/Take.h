//
// Created by Casper De Smet on 17/05/2022.
//

#ifndef MIDIASSISTANT_TAKE_H
#define MIDIASSISTANT_TAKE_H

#include <map>
#include <set>
#include <list>
#include <unordered_map>

#include "../MIDI/Processor.h"
#include "../MIDI/Messages/Instrument.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../Theory/TimeDivision.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;
    using UnfinishedNotes = std::map<int, MIDI::NoteOnPointer>;

    class Take
    {
    public:
        ApplicationPointer application;

        std::mutex mutex;

        std::unordered_map<MIDI::InstrumentPointer, UnfinishedNotes> instrumentUnfinishedNotes;
        std::map<int, std::set<MIDI::MessagePointer>> midiMessages;

        int lowestNote;
        int highestNote;

        explicit Take(const ApplicationPointer& application);

        void record(const MIDI::MessageOnInstrument& messageOnInstrument, int currentTime);
        void updateUnfinishedNotes(const MIDI::MessageOnInstrument& messageOnInstrument);
        void cleanupNotes(int currentTime);

        void quantize(Music::TimeDivision quantizeDivision);
        int quantizeTick(int tick, Music::TimeDivision quantizeDivision);
    };
    using TakePointer = std::shared_ptr<Take>;
}

#endif //MIDIASSISTANT_TAKE_H
