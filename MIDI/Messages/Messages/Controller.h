//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_CONTROLLER_H
#define MIDIASSISTANT_CONTROLLER_H

#include "../Message.h"

namespace MIDI
{
    class Controller: public Message
    {
    public:
        int number;
        int controllerValue;

        Controller(const libremidi::message& message);

        std::string message() override;
        std::string value() override;
        libremidi::message rawMessage(int channel) override;
    };
}

#endif //MIDIASSISTANT_CONTROLLER_H
