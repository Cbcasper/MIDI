//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_MESSAGE_H
#define MIDIASSISTANT_MESSAGE_H

#include <libremidi/message.hpp>
#include <string>
#include "../../Theory/Note/Note.h"

namespace MIDI
{
    class Message;
    using MessagePointer = std::shared_ptr<Message>;
    using MessageType = libremidi::message_type;

    class Message
    {
    public:
        MessageType type;
        std::string typeName;
        int tick;

        Message(const libremidi::message& message, const std::string& typeName);
        Message(MessageType type, const std::string& typeName);
        virtual ~Message() = default;

        virtual std::string toString();
        virtual std::string message();
        virtual std::string value();
        virtual libremidi::message rawMessage(int channel) = 0;

        virtual void move(int newTick);
    };
}

#endif //MIDIASSISTANT_MESSAGE_H
