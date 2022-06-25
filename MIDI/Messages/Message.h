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

    // Superclass for all messages
    class Message
    {
    public:
        // A message has a type and name and is played in a certain tick
        MessageType type;
        std::string typeName;
        int tick;

        Message(const libremidi::message& message, const std::string& typeName);
        Message(MessageType type, const std::string& typeName);
        virtual ~Message() = default;

        virtual std::string toString();
        virtual std::string message();
        virtual std::string value();
        // Convert to a raw libremidi message in order to play it or send it out
        virtual libremidi::message rawMessage(int channel = 1) = 0;

        // Move the message, in the case of a note on message, the note off message also needs to be moved
        virtual void move(int newTick);
    };
}

#endif //MIDIASSISTANT_MESSAGE_H
