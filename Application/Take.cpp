//
// Created by Casper De Smet on 17/05/2022.
//

#include "Take.h"

#include "Application.h"

namespace State
{
    Take::Take(const ApplicationPointer& application):
               application(application), lowestNote(128), highestNote(-1)
    {}

    void Take::record(const MIDI::MessageOnInstrument& messageOnInstrument, int currentTime)
    {
        updateUnfinishedNotes(messageOnInstrument);

        auto& [message, instrument] = messageOnInstrument;
        message->tick = currentTime;
        std::unique_lock<std::mutex> lock(mutex);
        midiMessages[message->tick].insert(message);
    }

    void Take::updateUnfinishedNotes(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        auto& [message, instrument] = messageOnInstrument;
        UnfinishedNotes& unfinishedNotes = instrumentUnfinishedNotes[instrument];
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
        {
            unfinishedNotes[noteOn->note->value] = noteOn;
            lowestNote = std::min(lowestNote, noteOn->note->value);
            highestNote = std::max(highestNote, noteOn->note->value);
        }
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(message))
        {
            int value = noteOff->note->value;
            if (unfinishedNotes[value])
                unfinishedNotes[value]->noteEnd = noteOff;
            unfinishedNotes.erase(value);
        }
    }

    void Take::cleanupNotes(int currentTime)
    {
        int currentTick = currentTime;
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& [instrument, unfinishedNotes]: instrumentUnfinishedNotes)
            for (const auto& [noteValue, noteOn]: unfinishedNotes)
            {
                Music::NotePointer note = Music::Note::getInstance(noteValue);
                MIDI::NoteOffPointer noteOff = std::make_shared<MIDI::NoteOff>(note, 127);
                noteOff->tick = currentTick;

                noteOn->noteEnd = noteOff;
                midiMessages[currentTick].insert(noteOff);
            }
        instrumentUnfinishedNotes = std::unordered_map<MIDI::InstrumentPointer, UnfinishedNotes>();
    }

    void Take::quantize(Music::TimeDivision quantizeDivision)
    {
        std::list<std::pair<MIDI::MessagePointer, int>> quantizedMessages;
        for (const auto& [tick, messages]: midiMessages)
            for (const MIDI::MessagePointer& message: messages)
                if (message->type == MIDI::MessageType::NOTE_ON)
                {
                    message->move(quantizeTick(tick, quantizeDivision));
                    quantizedMessages.emplace_back(std::make_pair(message, tick));
                }
        for (const auto& [message, oldTick]: quantizedMessages)
        {
            midiMessages[oldTick].erase(message);
            midiMessages[message->tick].insert(message);
        }
    }

    int Take::quantizeTick(int tick, Music::TimeDivision quantizeDivision)
    {
        float divisionRatio = static_cast<float>(Music::Sixteenth) / static_cast<float>(quantizeDivision);
        int divisionLength = static_cast<int>(static_cast<float>(application->song->ticksPerDivision) * divisionRatio);
        int tickOffset = tick % divisionLength;
        if (tickOffset < divisionLength / 2)
            return tick - tickOffset;
        return tick - tickOffset + divisionLength;
    }
}