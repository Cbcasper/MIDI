//
// Created by Casper De Smet on 12/03/2022.
//

#ifndef MIDIASSISTANT_INPUTPORT_H
#define MIDIASSISTANT_INPUTPORT_H

#include <memory>
#include <libremidi/libremidi.hpp>

#include "Port.h"

namespace MIDI
{
    // Wrapper class for libremidi midi in object
    class InputPort: public Port
    {
    public:
        // Use pointer in order to be able to move it around
        std::unique_ptr<libremidi::midi_in> midiIn{};

        InputPort();
        InputPort(int portNumber, const CallbackFunction& callbackFunction);
        // Make a virtual port
        InputPort(const CallbackFunction& callbackFunction);

        // Wrapped methods
        int portCount() override;
        std::string portName(int portNumber) override;
    };

    using InputPortPointer = std::shared_ptr<InputPort>;
}


#endif //MIDIASSISTANT_INPUTPORT_H
