//
// Created by Casper De Smet on 23/03/2022.
//

#include <iostream>

#include "Track.h"

#include "Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"

namespace State
{
    Track::Track(Application* application, const std::string& port, int channel): application(application)
    {
        audioPlayer = MIDI::AudioPlayer();
        input = std::make_shared<MIDI::Instrument>(port, channel);
    }

    void Track::incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        auto& [message, instrument] = messageOnInstrument;
        playMIDIMessage(messageOnInstrument);
        midiMessages[application->currentTime].emplace_back(message);
        if (application->displayMessageFilter(message))
        {
            SoundingNotes& instrumentSoundingNotes = soundingNotes[instrument];

            if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
                instrumentSoundingNotes[noteOn->note->value] = &noteOn->noteEnd;
            else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(message))
                if (instrumentSoundingNotes[noteOff->note->value])
                    *instrumentSoundingNotes[noteOff->note->value] = noteOff;
        }
    }

    void Track::playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        audioPlayer.processMIDIMessage(message->rawMessage(instrument->channel));
    }
}