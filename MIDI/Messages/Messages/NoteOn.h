//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_NOTEON_H
#define MIDIASSISTANT_NOTEON_H

#include <set>

#include "NoteOff.h"
#include "NoteMessage.h"
#include "../Message.h"
#include "../../../Theory/Note/Note.h"

namespace MIDI
{
    class NoteOn: public NoteMessage
    {
    public:
        NoteOffPointer noteEnd;

        NoteOn(const libremidi::message& message);
        NoteOn(const Music::NotePointer& note, int velocity);

        libremidi::message rawMessage(int channel) override;
        void move(int newTick) override;
    };

    using NoteOnPointer = std::shared_ptr<NoteOn>;

    struct NoteOnComparator
    {
        bool operator()(const NoteOnPointer& firstNoteOn, const NoteOnPointer& secondNoteOn) const
        {
            return firstNoteOn->tick - secondNoteOn->tick;
        }
    };

    using ChronologicNotes = std::multiset<NoteOnPointer, NoteOnComparator>;
}

#endif //MIDIASSISTANT_NOTEON_H
