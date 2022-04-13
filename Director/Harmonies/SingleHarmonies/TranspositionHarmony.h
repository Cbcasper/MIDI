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

        TranspositionHarmony(const std::string& port, int channel, int transpositionOffset);

        std::pair<bool, NotePointer> generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };
}

#endif //MIDIASSISTANT_TRANSPOSITIONHARMONY_H
