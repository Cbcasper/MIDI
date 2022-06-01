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
        Key::Degree modulation;
        int octaves;
        bool up;

        ModulationHarmony(const MIDI::InstrumentPointer& output, const KeyPointer& key);

        NotePointer generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };

    using ModulationHarmonyPointer = std::shared_ptr<ModulationHarmony>;
}


#endif //MIDIASSISTANT_MODULATIONHARMONY_H
