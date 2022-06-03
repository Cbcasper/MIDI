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
    class CanonHarmony: public Harmony
    {
    public:
        State::SongPointer song;

        Music::TimeDivision timeDivision;
        int numberOfDivisions;

        std::map<unsigned int, MIDI::NoteMessagePointer> scheduledMessages;

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
