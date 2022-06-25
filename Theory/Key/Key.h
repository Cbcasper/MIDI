//
// Created by Casper De Smet on 09/04/2022.
//

#ifndef MIDIASSISTANT_KEY_H
#define MIDIASSISTANT_KEY_H

#include "../Note/RootNote.h"
#include "IntervalSequence.h"

namespace Music
{
    class Key;
    using KeyPointer = std::shared_ptr<Key>;

    // Class that has a root note and a sequence of notes starting from that one
    class Key
    {
    public:
        enum Degree
        {
            First = 0,
            Second = 1,
            Third = 2,
            Fourth = 3,
            Fifth = 4,
            Sixth = 5,
            Seventh = 6,
            Invalid = -1
        };

        RootNotePointer rootNote;
        ISPointer intervalSequence;

        Key();

        // Query methods
        Degree getDegree(const RootNotePointer& note);
        std::pair<RootNotePointer, int> modulate(const RootNotePointer& note, Degree modulation, bool up = true);
        int getOctaveOffset(int index, int newIndex, bool up);

        // Parameters need to be set, because the sequence has to be recalculated every time
        void setRootNote(RootNote::Name name, bool sharp);
        void setRootNote(const std::pair<RootNote::Name, bool>& note);
        void setIntervalSequence(IntervalSequence::Type type);

        static std::string degreeName(Degree degree);
    };
}

#endif //MIDIASSISTANT_KEY_H
