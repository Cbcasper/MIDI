//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_AFTERTOUCH_H
#define MIDIASSISTANT_AFTERTOUCH_H

#include "../Message.h"
#include "../../../Theory/Note.h"

namespace MIDI
{
    class Aftertouch: public Message
    {
    public:
        Music::NotePointer note;
        int pressure;

        Aftertouch(const libremidi::message& message);

        std::string toString() override;
        std::string value() override;
        std::string message() override;
        libremidi::message rawMessage(int channel) override;
    };
}

#endif //MIDIASSISTANT_AFTERTOUCH_H
