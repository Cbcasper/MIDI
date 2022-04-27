//
// Created by Casper De Smet on 11/04/2022.
//

#include "RandomHarmony.h"

namespace Music
{
    RandomHarmony::RandomHarmony(const MIDI::InstrumentPointer& output): SingleHarmony(Random, output)
    {
        randomEngine = std::default_random_engine(randomDevice());
        uniformIntDistribution = std::uniform_int_distribution(0, 127);
    }

    std::pair<bool, NotePointer> RandomHarmony::generateNote(const MIDI::NoteOnPointer& noteOn)
    {
        return std::make_pair(true, Note::getInstance(uniformIntDistribution(randomEngine)));
    }
}