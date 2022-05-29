//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_CHORD_H
#define MIDIASSISTANT_CHORD_H

#include "../Note/RootNote.h"
#include "../Key/IntervalSequence.h"

namespace Music
{
    class Chord
    {
    public:
        RootNotePointer rootNote;
        RootNotePointer third;
        RootNotePointer fifth;

        IntervalSequence::Modulation rollOverModulation;

        Chord(const ISPointer& intervalSequence, const RootNotePointer& rootNote);

        RootNotePointer operator()(IntervalSequence::Modulation modulation);
    };
    using ChordPointer = std::shared_ptr<Chord>;
}

#endif //MIDIASSISTANT_CHORD_H
