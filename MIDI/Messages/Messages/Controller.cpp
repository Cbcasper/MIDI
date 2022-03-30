//
// Created by Casper De Smet on 02/03/2022.
//

#include "Controller.h"

namespace MIDI
{
    Controller::Controller(const libremidi::message& message):
                Message(message, "Controller")
    {
        number = message[1];
        controllerValue = message[2];
    }

    std::string Controller::message()
    {
        return Message::message() + ": " + std::to_string(number);
    }

    std::string Controller::value()
    {
        return std::to_string(controllerValue);
    }

    libremidi::message Controller::rawMessage(int channel)
    {
        return libremidi::message::control_change(channel, number, controllerValue);
    }
}