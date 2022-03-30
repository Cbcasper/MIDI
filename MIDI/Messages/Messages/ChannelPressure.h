//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_CHANNELPRESSURE_H
#define MIDIASSISTANT_CHANNELPRESSURE_H

#include "../Message.h"

namespace MIDI
{
    class ChannelPressure: public Message
    {
    public:
        int pressure;

        ChannelPressure(const libremidi::message& message);

        std::string value() override;
        libremidi::message rawMessage(int channel) override;
    };
}

#endif //MIDIASSISTANT_CHANNELPRESSURE_H
