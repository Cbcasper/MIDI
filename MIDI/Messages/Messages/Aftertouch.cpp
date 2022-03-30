//
// Created by Casper De Smet on 02/03/2022.
//

#include "Aftertouch.h"

namespace MIDI
{
    Aftertouch::Aftertouch(const libremidi::message& message):
                Message(message, "Aftertouch")
    {
        note = Music::Note::getInstance(message[1]);
        pressure = message[2];
    }

    std::string Aftertouch::toString()
    {
        return note->toString();
    }

    std::string Aftertouch::message()
    {
        return Message::message() + ": " + note->toString();
    }

    std::string Aftertouch::value()
    {
        return std::to_string(pressure);
    }

    libremidi::message Aftertouch::rawMessage(int channel)
    {
        return libremidi::message::aftertouch(channel, pressure);
    }
}