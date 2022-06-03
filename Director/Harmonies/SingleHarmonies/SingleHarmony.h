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
        SingleHarmony(const State::ApplicationPointer& application, Type type,
                      const std::string& typeName, const MIDI::InstrumentPointer& output);

        virtual NotePointer generateNote(const MIDI::NoteOnPointer& noteOn) = 0;
        void generate(const MIDI::NoteMessagePointer& noteMessage) override;
        MIDI::NoteMessagePointer handleNoteOn(const MIDI::NoteOnPointer& noteOn);
        MIDI::NoteMessagePointer handleNoteOff(const MIDI::NoteOffPointer& noteOff);
    };
}

#endif //MIDIASSISTANT_SINGLEHARMONY_H
