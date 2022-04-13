//
// Created by Casper De Smet on 11/04/2022.
//

#ifndef MIDIASSISTANT_SINGLEHARMONY_H
#define MIDIASSISTANT_SINGLEHARMONY_H

#include "../Harmony.h"
#include "../../../Theory/Note/Note.h"
#include "../../../MIDI/Messages/Messages/NoteOn.h"

namespace Music
{
    class SingleHarmony: public Harmony
    {
    public:
        SingleHarmony(const std::string& port, int channel);

        virtual std::pair<bool, NotePointer> generateNote(const MIDI::NoteOnPointer& noteOn) = 0;
        MIDI::NoteMessagePointer generate(const MIDI::NoteMessagePointer& noteMessage) override;
        void handleNoteOn(const MIDI::NoteOnPointer& noteOn, MIDI::NoteMessagePointer& generatedMessage);
        void handleNoteOff(const MIDI::NoteOffPointer& noteOff, MIDI::NoteMessagePointer& generatedMessage);
    };
}

#endif //MIDIASSISTANT_SINGLEHARMONY_H
