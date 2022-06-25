//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_RANDOMHARMONY_H
#define MIDIASSISTANT_RANDOMHARMONY_H

#include <random>

#include "SingleHarmony.h"

namespace Music
{
    // Harmony that generates random notes
    class RandomHarmony: public SingleHarmony
    {
    public:
        // Random generation
        std::random_device randomDevice;
        std::default_random_engine randomEngine;
        std::uniform_int_distribution<int> uniformIntDistribution;

        // Parameters
        int low;
        int high;
        bool absolute;

        explicit RandomHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);

        void setAbsolute(bool newAbsolute);
        void reinitialize();
        NotePointer generateNote(const MIDI::NoteOnPointer& noteOn) override;
    };

    using RandomHarmonyPointer = std::shared_ptr<RandomHarmony>;
}

#endif //MIDIASSISTANT_RANDOMHARMONY_H
