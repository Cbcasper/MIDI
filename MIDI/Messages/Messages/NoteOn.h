//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_NOTEON_H
#define MIDIASSISTANT_NOTEON_H

#include "../Message.h"
#include "../../../Theory/Note.h"
#include "NoteMessage.h"

namespace MIDI
{
    class NoteOn: public NoteMessage
    {
    public:
        NoteOn(const libremidi::message& message);
        NoteOn(const Music::NotePointer& note, int velocity);

        libremidi::message rawMessage(int channel) override;
    };

    using NoteOnPointer = std::shared_ptr<NoteOn>;
}

#endif //MIDIASSISTANT_NOTEON_H
