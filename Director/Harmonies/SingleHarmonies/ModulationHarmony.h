//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_MODULATIONHARMONY_H
#define MIDIASSISTANT_MODULATIONHARMONY_H

#include "SingleHarmony.h"
#include "../../../Theory/Key/Key.h"

namespace Music
{
    // Harmony that modulates the played note by a set degree
    class ModulationHarmony: public SingleHarmony
    {
    public:
        KeyPointer key;
        Key::Degree modulation;
        int octaves;
        bool up;

        ModulationHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);

        NotePointer generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };

    using ModulationHarmonyPointer = std::shared_ptr<ModulationHarmony>;
}


#endif //MIDIASSISTANT_MODULATIONHARMONY_H
