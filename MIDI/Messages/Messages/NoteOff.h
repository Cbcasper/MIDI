//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_NOTEOFF_H
#define MIDIASSISTANT_NOTEOFF_H

#include "NoteMessage.h"
#include "../Message.h"
#include "../../../Theory/Note/Note.h"

namespace MIDI
{
    class NoteOff: public NoteMessage
    {
    public:
        NoteOff(const libremidi::message& message);
        NoteOff(const Music::NotePointer& note, int velocity);

        libremidi::message rawMessage(int channel) override;
    };
    using NoteOffPointer = std::shared_ptr<NoteOff>;
}



#endif //MIDIASSISTANT_NOTEOFF_H
