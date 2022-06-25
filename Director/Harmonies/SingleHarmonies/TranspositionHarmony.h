//
// Created by Casper De Smet on 24/03/2022.
//

#ifndef MIDIASSISTANT_TRANSPOSITIONHARMONY_H
#define MIDIASSISTANT_TRANSPOSITIONHARMONY_H

#include "SingleHarmony.h"

namespace Music
{
    // Harmony that generates a note at an offset of a set number of semitones
    class TranspositionHarmony: public SingleHarmony
    {
    public:
        int transpositionOffset;

        TranspositionHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);

        NotePointer generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };

    using TranspositionHarmonyPointer = std::shared_ptr<TranspositionHarmony>;
}

#endif //MIDIASSISTANT_TRANSPOSITIONHARMONY_H
