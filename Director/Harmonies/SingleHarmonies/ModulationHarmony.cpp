//
// Created by Casper De Smet on 11/04/2022.
//

#include "ModulationHarmony.h"

namespace Music
{
    ModulationHarmony::ModulationHarmony(const std::string& port, int channel, int modulationOffset, const KeyPointer& key):
                       SingleHarmony(port, channel), modulationOffset(modulationOffset), key(key)
    {}

    std::pair<bool, NotePointer> ModulationHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        const auto& [modulatedNote, octaveOffset] = key->intervalSequence->modulate(noteOn->note, modulationOffset);
        if (modulatedNote)
            return std::make_pair(true, Note::getInstance(modulatedNote, noteOn->note->octave + octaveOffset));
        return std::make_pair(true, nullptr);
    }
}
