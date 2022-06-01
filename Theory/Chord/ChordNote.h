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
        Key::Degree degree;

        ChordNote(const ChordPointer& chord, Key::Degree degree, int octave);

        void operator++();
        void operator--();
        void operator+(int offset);
        void operator-(int offset);
        void operator+=(int offset);
        void operator-=(int offset);
        Key::Degree getNextModulation();
        Key::Degree getPreviousModulation();

        NotePointer getNote();
    };

    using ChordNotePointer = std::shared_ptr<ChordNote>;
}

#endif //MIDIASSISTANT_CHORDNOTE_H
