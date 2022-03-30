//
// Created by Casper De Smet on 14/03/2022.
//

#include "NoteMessage.h"

namespace MIDI
{
    NoteMessage::NoteMessage(const libremidi::message& message, const std::string& typeName):
                 Message(message, typeName)
    {
        note = Music::Note::getInstance(message[1]);
        velocity = message[2];
    }

    NoteMessage::NoteMessage(const std::string& typeName, MessageType type, const Music::NotePointer& note, int velocity):
                 Message(type, typeName), note(note), velocity(velocity)
    {}

    std::string NoteMessage::toString()
    {
        return note->toString();
    }

    std::string NoteMessage::message()
    {
        return Message::message() + ": " + note->toString();
    }

    std::string NoteMessage::value()
    {
        return std::to_string(velocity);
    }
}