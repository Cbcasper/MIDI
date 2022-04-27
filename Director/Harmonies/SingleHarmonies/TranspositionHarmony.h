//
// Created by Casper De Smet on 24/03/2022.
//

#ifndef MIDIASSISTANT_TRANSPOSITIONHARMONY_H
#define MIDIASSISTANT_TRANSPOSITIONHARMONY_H

#include "SingleHarmony.h"

namespace Music
{
    class TranspositionHarmony: public SingleHarmony
    {
    public:
        int transpositionOffset;

        TranspositionHarmony(const MIDI::InstrumentPointer& output, int transpositionOffset = 0);

        std::pair<bool, NotePointer> generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };
}

#endif //MIDIASSISTANT_TRANSPOSITIONHARMONY_H
