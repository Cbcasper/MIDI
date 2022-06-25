//
// Created by Casper De Smet on 24/03/2022.
//

#include "TranspositionHarmony.h"

namespace Music
{
    TranspositionHarmony::TranspositionHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application):
                          SingleHarmony(application, Transposition, "Transposition", output),
                          transpositionOffset(0)
    {}

    NotePointer TranspositionHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        return Note::getInstance(noteOn->note->value + transpositionOffset);
    }
}