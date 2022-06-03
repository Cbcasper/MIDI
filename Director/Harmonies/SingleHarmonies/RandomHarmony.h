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
