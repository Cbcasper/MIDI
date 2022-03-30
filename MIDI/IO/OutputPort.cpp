//
// Created by Casper De Smet on 12/03/2022.
//

#include "OutputPort.h"

namespace MIDI
{
    OutputPort::OutputPort()
    {
        midiOut = std::make_unique<libremidi::midi_out>();
    }

    OutputPort::OutputPort(int portNumber)
    {
        midiOut = std::make_unique<libremidi::midi_out>();
        name = midiOut->get_port_name(portNumber);
        midiOut->open_port(portNumber);
    }

    void OutputPort::sendMessage(const libremidi::message& message)
    {
        midiOut->send_message(message);
    }

    int OutputPort::portCount()
    {
        return midiOut->get_port_count();
    }

    std::string OutputPort::portName(int portNumber)
    {
        return midiOut->get_port_name(portNumber);
    }
}
