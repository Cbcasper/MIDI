//
// Created by Casper De Smet on 02/03/2022.
//

#include "NoteOn.h"

#include <iostream>

namespace MIDI
{
    NoteOn::NoteOn(const libremidi::message& message):
            NoteMessage(message, "Note On"), noteEnd(nullptr)
    {}

    NoteOn::NoteOn(const Music::NotePointer& note, int velocity):
            NoteMessage("Note On", MessageType::NOTE_ON, note, velocity), noteEnd(nullptr)
    {}

    libremidi::message NoteOn::rawMessage(int channel)
    {
        return libremidi::message::note_on(channel, note->value, velocity);
    }
}
