//
// Created by Casper De Smet on 12/03/2022.
//

#ifndef MIDIASSISTANT_PORT_H
#define MIDIASSISTANT_PORT_H

#include <string>
#include <libremidi/message.hpp>

namespace MIDI
{
    // Superclass for port wrapper classes
    class Port
    {
    public:
        std::string name;

        Port() = default;

        // Virtual wrapper methods
        virtual int portCount() = 0;
        virtual std::string portName(int portNumber) = 0;
    };

    using CallbackFunction = std::function<void(const libremidi::message& message, const std::string& portName)>;
    using PortPointer = std::shared_ptr<Port>;
}


#endif //MIDIASSISTANT_PORT_H
