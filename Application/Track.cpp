//
// Created by Casper De Smet on 23/03/2022.
//

#include <iostream>

#include "Track.h"

#include "./Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"

namespace State
{
    Track::Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output):
                 application(application), input(input), output(output), height(150),
                 lowestNote(64), highestNote(64), notesRecorded(false)
    {
        audioPlayer = MIDI::AudioPlayer();
    }

    void Track::incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        if (*instrument == *input)
        {
            playMIDIMessage(messageOnInstrument);
            if (application->recording)
                recordMIDIMessage(messageOnInstrument);
        }
    }

    void Track::playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        audioPlayer.processMIDIMessage(message->rawMessage(instrument->channel));
    }

    void Track::recordMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument, int tick)
    {
        auto& [message, instrument] = messageOnInstrument;
        message->tick = tick == -1 ? application->currentTime : tick;
        if (application->displayMessageFilter(message))
        {
            SoundingNotes& soundingNotes = instrumentSoundingNotes[instrument];

            if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
            {
                soundingNotes[noteOn->note->value] = &noteOn->noteEnd;
                if (!notesRecorded)
                {
                    lowestNote = noteOn->note->value;
                    highestNote = noteOn->note->value;
                    notesRecorded = true;
                }
                else
                {
                    lowestNote = std::min(lowestNote, noteOn->note->value);
                    highestNote = std::max(highestNote, noteOn->note->value);
                }
            }
            else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(message))
            {
                int value = noteOff->note->value;
                if (soundingNotes[value])
                    *soundingNotes[value] = noteOff;
                soundingNotes.erase(value);
            }
        }
        std::unique_lock<std::mutex> lock(mutex);
        midiMessages[message->tick].insert(message);
    }

    void Track::cleanupNotes()
    {
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& [instrument, soundingNotes]: instrumentSoundingNotes)
            for (const auto& [noteValue, noteOffPointer]: soundingNotes)
            {
                Music::NotePointer note = Music::Note::getInstance(noteValue);
                MIDI::NoteOffPointer noteOff = std::make_shared<MIDI::NoteOff>(note, 127);
                noteOff->tick = application->currentTime;

                *noteOffPointer = noteOff;
                midiMessages[noteOff->tick].insert(noteOff);
            }
        instrumentSoundingNotes = std::unordered_map<MIDI::InstrumentPointer, SoundingNotes>();
    }

    bool Track::hasSelectedHarmonies()
    {
        for (const Music::HarmonyPointer& harmony: harmonies)
            if (harmony->selected)
                return true;
        return false;
    }

    void Track::clearSelectedHarmonies()
    {
        for (const Music::HarmonyPointer& harmony: harmonies)
            harmony->selected = false;
    }

    void Track::quantize(Music::TimeDivision quantizeDivision)
    {
        std::map<MIDI::MessagePointer, int> quantizedMessages;
        for (const auto& [tick, messages]: midiMessages)
            for (const MIDI::MessagePointer& message: messages)
                if (message->type == MIDI::MessageType::NOTE_ON)
                {
                    message->move(quantizeTick(tick, quantizeDivision));
                    quantizedMessages[message] = tick;
                }
        for (const auto& [message, oldTick]: quantizedMessages)
        {
            midiMessages[oldTick].erase(message);
            midiMessages[message->tick].insert(message);
        }
    }

    int Track::quantizeTick(int tick, Music::TimeDivision quantizeDivision)
    {
        float divisionRatio = static_cast<float>(Music::Sixteenth) / static_cast<float>(quantizeDivision);
        int divisionLength = static_cast<int>(static_cast<float>(application->song->ticksPerDivision) * divisionRatio);
        int tickOffset = tick % divisionLength;
        if (tickOffset < divisionLength / 2)
            return tick - tickOffset;
        return tick - tickOffset + divisionLength;
    }
}