//
// Created by Casper De Smet on 11/04/2022.
//

#include "ModulationHarmony.h"

#include <iostream>

#include "../../../Application/Application.h"

namespace Music
{
    ModulationHarmony::ModulationHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application):
            SingleHarmony(application, Modulation, "Modulation", output), key(application->song->key),
            modulation(Key::First), octaves(0), up(true)
    {}

    NotePointer ModulationHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        auto [modulatedNote, octaveOffset] = key->modulate(noteOn->note, modulation, up);
        if (modulatedNote)
        {
            int modulatedOctave = noteOn->note->octave + (up ? octaves : -octaves) + octaveOffset;
            return Note::getInstance(modulatedNote, modulatedOctave);
        }
        return nullptr;
    }
}
