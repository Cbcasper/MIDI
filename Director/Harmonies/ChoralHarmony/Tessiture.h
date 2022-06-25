//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_TESSITURE_H
#define MIDIASSISTANT_TESSITURE_H

#include "../../../Theory/Chord/ChordNote.h"

namespace Music
{
    // A range in which a voice can be played
    class Tessiture
    {
    public:
        int low;
        int high;

        Tessiture(int low, int high);

        // Compute the intersection between two tessitures
        Tessiture intersect(const Tessiture& tessiture);
        // Move a note up or down until it is inside the tessiture
        void operator()(const ChordNotePointer& chordNote);
    };
}

#endif //MIDIASSISTANT_TESSITURE_H
