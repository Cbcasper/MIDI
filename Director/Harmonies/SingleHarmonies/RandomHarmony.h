//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_RANDOMHARMONY_H
#define MIDIASSISTANT_RANDOMHARMONY_H

#include <random>

#include "SingleHarmony.h"

namespace Music
{
    class RandomHarmony: public SingleHarmony
    {
    public:
        std::random_device randomDevice;
        std::default_random_engine randomEngine;
        std::uniform_int_distribution<int> uniformIntDistribution;

        RandomHarmony(const std::string& port, int channel);

        std::pair<bool, NotePointer> generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };
}

#endif //MIDIASSISTANT_RANDOMHARMONY_H
