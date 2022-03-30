//
// Created by Casper De Smet on 02/03/2022.
//

#include "NoteOff.h"

namespace MIDI
{
    NoteOff::NoteOff(const libremidi::message& message):
             NoteMessage(message, "Note Off")
    {}

    NoteOff::NoteOff(const Music::NotePointer& note, int velocity):
             NoteMessage("Note Off", MessageType::NOTE_OFF, note, velocity)
    {}

    libremidi::message NoteOff::rawMessage(int channel)
    {
        return libremidi::message::note_off(channel, note->value, velocity);
    }
}
