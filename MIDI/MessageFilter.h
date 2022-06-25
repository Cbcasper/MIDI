//
// Created by Casper De Smet on 05/03/2022.
//

#ifndef MIDIASSISTANT_MESSAGEFILTER_H
#define MIDIASSISTANT_MESSAGEFILTER_H

#include <vector>
#include "Messages/Message.h"

namespace MIDI
{
    class MessageFilter
    {
    public:
        // The system only supports these types, any other types will be disallowed no matter what
        std::vector<MessageType> supportedTypes = {MessageType::AFTERTOUCH,
                                                   MessageType::POLY_PRESSURE,
                                                   MessageType::CONTROL_CHANGE,
                                                   MessageType::NOTE_OFF,
                                                   MessageType::NOTE_ON,
                                                   MessageType::PITCH_BEND,
                                                   MessageType::PROGRAM_CHANGE};
        std::map<MessageType, bool> allowedTypes;

        MessageFilter();

        // Filter setup logic
        void allowAllTypes();
        void disallowAllTypes();
        void allowTypes(const std::vector<MessageType>& types);
        void disallowTypes(const std::vector<MessageType>& types);
        bool operator()(const MessagePointer& message);
    };
}


#endif //MIDIASSISTANT_MESSAGEFILTER_H
