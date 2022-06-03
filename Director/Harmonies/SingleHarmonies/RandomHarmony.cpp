//
// Created by Casper De Smet on 11/04/2022.
//

#include "RandomHarmony.h"

#include <iostream>

#include "../../../Utilities/Utilities.h"

namespace Music
{
    RandomHarmony::RandomHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application):
                   SingleHarmony(application, Random, "Random", output), absolute(true), low(0), high(127)
    {
        randomEngine = std::default_random_engine(randomDevice());
        uniformIntDistribution = std::uniform_int_distribution(low, high);
    }

    NotePointer RandomHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        int offset = uniformIntDistribution(randomEngine);
        int newNote = absolute ? offset : noteOn->note->value + offset;
        return Note::getInstance(Utilities::clamp(newNote, 0, 127));
    }

    void RandomHarmony::setAbsolute(bool newAbsolute)
    {
        absolute = newAbsolute;
        if (absolute)
        {
            low = 0;
            high = 127;
        }
        else
        {
            low = -12;
            high = 12;
        }
        uniformIntDistribution = std::uniform_int_distribution(low, high);
    }

    void RandomHarmony::reinitialize()
    {
        uniformIntDistribution = std::uniform_int_distribution(low, high);
    }
}