//
// Created by Casper De Smet on 23/03/2022.
//

#include <iostream>

#include "Track.h"

#include "Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"

namespace State
{
    Track::Track(Application* application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output):
                 application(application), input(input), output(output)
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

    void Track::recordMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        auto& [message, instrument] = messageOnInstrument;
        message->tick = application->currentTime;
        if (application->displayMessageFilter(message))
        {
            SoundingNotes& soundingNotes = instrumentSoundingNotes[instrument];

            if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
                soundingNotes[noteOn->note->value] = &noteOn->noteEnd;
            else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(message))
            {
                int value = noteOff->note->value;
                if (soundingNotes[value])
                    *soundingNotes[value] = noteOff;
                soundingNotes.erase(value);
            }
        }
        std::unique_lock<std::mutex> lock(mutex);
        midiMessages[message->tick].emplace_back(message);
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
                midiMessages[noteOff->tick].emplace_back(noteOff);
            }
        instrumentSoundingNotes = std::unordered_map<MIDI::InstrumentPointer, SoundingNotes>();
    }
}