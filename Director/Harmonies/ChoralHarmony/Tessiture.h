//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_TESSITURE_H
#define MIDIASSISTANT_TESSITURE_H

#include "../../../Theory/Chord/ChordNote.h"

namespace Music
{
    class Tessiture
    {
    public:
        int low;
        int high;

        Tessiture(int low, int high);

        Tessiture intersect(const Tessiture& tessiture);
        void operator()(const ChordNotePointer& chordNote);
    };
}

#endif //MIDIASSISTANT_TESSITURE_H
