//
// Created by Casper De Smet on 05/05/2022.
//

#ifndef MIDIASSISTANT_CANONHARMONY_H
#define MIDIASSISTANT_CANONHARMONY_H

#include <map>
#include <future>

#include "Harmony.h"
#include "../Sequencer/Timer.h"

namespace Music
{
    // Harmony that plays the played note a number of time divisions later
    class CanonHarmony: public Harmony
    {
    public:
        // Parameters
        State::SongPointer song;

        Music::TimeDivision timeDivision;
        int numberOfDivisions;

        std::map<unsigned int, MIDI::NoteMessagePointer> scheduledMessages;

        // Listen to the timer in order to play the note at an offset
        System::TimerPointer timer;
        std::future<void> timerFuture;

        bool running;

        CanonHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);
        virtual ~CanonHarmony();

        void timerThread();
        void processScheduledMessages();
        void generate(const MIDI::NoteMessagePointer& noteMessage) override;
    };

    using CanonHarmonyPointer = std::shared_ptr<CanonHarmony>;
}

#endif //MIDIASSISTANT_CANONHARMONY_H
