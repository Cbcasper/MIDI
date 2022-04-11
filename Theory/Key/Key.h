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

    class Key
    {
    public:
        RootNotePointer rootNote;
        ISPointer intervalSequence;

        Key();

        void setRootNote(RootNote::Name name, bool sharp);
        void setIntervalSequence(IntervalSequence::Type type);
    };
}

#endif //MIDIASSISTANT_KEY_H
