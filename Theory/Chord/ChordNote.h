//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_CHORDNOTE_H
#define MIDIASSISTANT_CHORDNOTE_H

#include "Chord.h"
#include "../Note/Note.h"

namespace Music
{
    class ChordNote
    {
    public:
        ChordPointer chord;

        int octave;
        IntervalSequence::Modulation modulation;

        ChordNote(const ChordPointer& chord, IntervalSequence::Modulation modulation, int octave);

        void operator++();
        void operator--();
        void operator+(int offset);
        void operator-(int offset);
        void operator+=(int offset);
        void operator-=(int offset);
        IntervalSequence::Modulation getNextModulation();
        IntervalSequence::Modulation getPreviousModulation();

        NotePointer getNote();
    };

    using ChordNotePointer = std::shared_ptr<ChordNote>;
}

#endif //MIDIASSISTANT_CHORDNOTE_H
