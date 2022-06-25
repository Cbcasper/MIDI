//
// Created by Casper De Smet on 12/03/2022.
//

#ifndef MIDIASSISTANT_OUTPUTPORT_H
#define MIDIASSISTANT_OUTPUTPORT_H

#include <memory>
#include <libremidi/libremidi.hpp>

#include "Port.h"

namespace MIDI
{
    // Wrapper class for libremidi midi out object
    class OutputPort: public Port
    {
    public:
        std::unique_ptr<libremidi::midi_out> midiOut{};

        OutputPort();
        explicit OutputPort(int portNumber);

        // Wrapper methods
        void sendMessage(const libremidi::message& message);

        int portCount() override;
        std::string portName(int portNumber) override;
    };

    using OutputPortPointer = std::shared_ptr<OutputPort>;
}


#endif //MIDIASSISTANT_OUTPUTPORT_H
