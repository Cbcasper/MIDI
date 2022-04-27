//
// Created by Casper De Smet on 24/03/2022.
//

#include "TranspositionHarmony.h"
#include "../../../Theory/Theory.h"

namespace Music
{
    TranspositionHarmony::TranspositionHarmony(const MIDI::InstrumentPointer& output, int transpositionOffset):
                          SingleHarmony(Transposition, output), transpositionOffset(transpositionOffset)
    {}

    std::pair<bool, NotePointer> TranspositionHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        return std::make_pair(true, Note::getInstance(noteOn->note->value + transpositionOffset));
    }
}