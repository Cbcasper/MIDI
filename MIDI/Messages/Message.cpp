//
// Created by Casper De Smet on 02/03/2022.
//

#include "Message.h"

namespace MIDI
{
    Message::Message(const libremidi::message& message, const std::string& typeName): typeName(typeName), tick(-2)
    {
        type = message.get_message_type();
    }

    Message::Message(MessageType type, const std::string& typeName): typeName(typeName), type(type), tick(-2)
    {}

    std::string Message::toString()
    {
        return "Message";
    }

    std::string Message::message()
    {
        return typeName;
    }

    std::string Message::value()
    {
        return std::string();
    }

    void Message::move(int newTick)
    {
        tick = newTick;
    }
}
