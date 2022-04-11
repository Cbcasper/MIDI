//
// Created by Casper De Smet on 24/03/2022.
//

#ifndef MIDIASSISTANT_TRANSPOSITIONHARMONY_H
#define MIDIASSISTANT_TRANSPOSITIONHARMONY_H

#include "Harmony.h"

namespace Music
{
    class TranspositionHarmony: public Harmony
    {
    public:
        int transpositionOffset;

        TranspositionHarmony(const std::string& port, int channel, int transpositionOffset);

        MIDI::NoteMessagePointer generate(const MIDI::NoteMessagePointer& noteMessage) override;
    };
}

#endif //MIDIASSISTANT_TRANSPOSITIONHARMONY_H
