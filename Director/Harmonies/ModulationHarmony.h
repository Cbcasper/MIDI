//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_MODULATIONHARMONY_H
#define MIDIASSISTANT_MODULATIONHARMONY_H

#include "Harmony.h"

namespace Music
{
    class ModulationHarmony: public Harmony
    {
        int modulationOffset;

        ModulationHarmony(const std::string& port, int channel, int modulationOffset);

        MIDI::NoteMessagePointer generate(const MIDI::NoteMessagePointer& noteMessage) override;
    };
}


#endif //MIDIASSISTANT_MODULATIONHARMONY_H
