//
// Created by Casper De Smet on 05/03/2022.
//

#include "MessageFilter.h"

namespace MIDI
{
    MessageFilter::MessageFilter()
    {
        allowAllTypes();
    }

    void MessageFilter::allowAllTypes()
    {
        for (MessageType messageType: supportedTypes)
            allowedTypes[messageType] = true;
    }

    void MessageFilter::disallowAllTypes()
    {
        for (MessageType messageType: supportedTypes)
            allowedTypes[messageType] = false;
    }

    void MessageFilter::allowTypes(const std::vector<MessageType>& types)
    {
        for (MessageType messageType: types)
            allowedTypes[messageType] = true;
    }

    void MessageFilter::disallowTypes(const std::vector<MessageType>& types)
    {
        for (MessageType messageType: types)
            allowedTypes[messageType] = false;
    }

    bool MessageFilter::operator()(const MessagePointer & message)
    {
        return allowedTypes[message->type];
    }
}