//
// Created by Casper De Smet on 11/04/2022.
//

#include "ModulationHarmony.h"

#include <iostream>

namespace Music
{
    ModulationHarmony::ModulationHarmony(const MIDI::InstrumentPointer& output, const KeyPointer& key, int modulationOffset):
                       SingleHarmony(Modulation, output), modulationOffset(modulationOffset), key(key)
    {}

    NotePointer ModulationHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        const auto& [modulatedNote, octaveOffset] = key->intervalSequence->modulate(noteOn->note, modulationOffset);
        if (modulatedNote)
            return Note::getInstance(modulatedNote, noteOn->note->octave + octaveOffset);
        return nullptr;
    }
}
