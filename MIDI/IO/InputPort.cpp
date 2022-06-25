//
// Created by Casper De Smet on 12/03/2022.
//

#include "InputPort.h"

namespace MIDI

{
    InputPort::InputPort()
    {
        midiIn = std::make_unique<libremidi::midi_in>();
    }

    InputPort::InputPort(int portNumber, const CallbackFunction& callbackFunction)
    {
        // Open the given port number
        midiIn = std::make_unique<libremidi::midi_in>();
        name = midiIn->get_port_name(portNumber);
        midiIn->set_callback([=](const libremidi::message& message){ callbackFunction(message, name); });
        midiIn->open_port(portNumber);
    }

    InputPort::InputPort(const CallbackFunction& callbackFunction)
    {
        // Open a virtual port
        midiIn = std::make_unique<libremidi::midi_in>();
        name = "MIDI Assistant";
        midiIn->open_virtual_port(name);
        midiIn->set_callback([=](const libremidi::message& message){ callbackFunction(message, name); });
    }

    int InputPort::portCount()
    {
        return midiIn->get_port_count();
    }

    std::string InputPort::portName(int portNumber)
    {
        return midiIn->get_port_name(portNumber);
    }
}
