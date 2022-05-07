//
// Created by Casper De Smet on 05/05/2022.
//

#include "CanonHarmony.h"

namespace Music
{
    CanonHarmony::CanonHarmony(const MIDI::InstrumentPointer& output, const State::SongPointer& song) :
                  Harmony(Canon, output), song(song)
    {
        running = true;
        timer = System::Timer::getInstance();
        timerFuture = Utilities::makeThread([=]{ return timerThread(); });
        timeDivision = Music::Fourth;
        numberOfDivisions = 4;
    }

    void CanonHarmony::timerThread()
    {
        bool status = false;
        timer->subscribe(&status);

        std::shared_lock<std::shared_mutex> lock(timer->signalMutex);
        timer->signalCV.wait(lock, [&]{ return status; });
        while (running)
        {
            processScheduledMessages();

            status = false;
            timer->signalCV.wait(lock, [&]{ return status; });
        }
        timer->unsubscribe(&status);
    }

    void CanonHarmony::processScheduledMessages()
    {
        if (MIDI::NoteMessagePointer noteMessage = scheduledMessages[timer->ticks])
        {
            play(noteMessage, noteMessage);
            if (noteMessage->type == MIDI::MessageType::NOTE_ON)    soundingNotes[noteMessage->note] = noteMessage->note;
            else                                                    soundingNotes.erase(noteMessage->note);
        }
        scheduledMessages.erase(timer->ticks);
    }

    MIDI::NoteMessagePointer CanonHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        float divisionRatio = static_cast<float>(Music::Sixteenth) / static_cast<float>(timeDivision);
        float ticksPerDivision = static_cast<float>(song->ticksPerDivision) * divisionRatio;
        int offset = static_cast<int>(ticksPerDivision) * numberOfDivisions;
        scheduledMessages[timer->ticks + offset] = noteMessage;
        return noteMessage;
    }

    CanonHarmony::~CanonHarmony()
    {
        running = false;
        timerFuture.wait();
    }
}