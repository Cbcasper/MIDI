//
// Created by Casper De Smet on 14/03/2022.
//

#ifndef MIDIASSISTANT_NOTEMESSAGE_H
#define MIDIASSISTANT_NOTEMESSAGE_H

#include "../Message.h"

namespace MIDI
{
    class NoteMessage: public Message
    {
    public:
        Music::NotePointer note;
        int velocity;

        NoteMessage(const libremidi::message& message, const std::string& typeName);
        NoteMessage(const std::string& typeName, MessageType type, const Music::NotePointer& note, int velocity);
        ~NoteMessage() override = default;

        std::string toString() override;
        std::string message() override;
        std::string value() override;
    };

    using NoteMessagePointer = std::shared_ptr<NoteMessage>;
}


#endif //MIDIASSISTANT_NOTEMESSAGE_H
