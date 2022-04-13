//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_MODULATIONHARMONY_H
#define MIDIASSISTANT_MODULATIONHARMONY_H

#include "SingleHarmony.h"
#include "../../../Theory/Key/Key.h"

namespace Music
{
    class ModulationHarmony: public SingleHarmony
    {
    public:
        KeyPointer key;
        int modulationOffset;

        ModulationHarmony(const std::string& port, int channel, int modulationOffset, const KeyPointer& key);

        std::pair<bool, NotePointer> generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };
}


#endif //MIDIASSISTANT_MODULATIONHARMONY_H
