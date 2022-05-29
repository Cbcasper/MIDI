//
// Created by Casper De Smet on 11/04/2022.
//

#include "SingleHarmony.h"
#include "../../../Theory/Theory.h"

namespace Music
{
    SingleHarmony::SingleHarmony(Type type, const MIDI::InstrumentPointer& output): Harmony(type, output)
    {}

    void SingleHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        MIDI::NoteMessagePointer generatedMessage;
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(noteMessage))
            generatedMessage = handleNoteOn(noteOn);
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(noteMessage))
            generatedMessage = handleNoteOff(noteOff);

        if (generatedMessage)
            play(generatedMessage);
    }

    MIDI::NoteMessagePointer SingleHarmony::handleNoteOn(const MIDI::NoteOnPointer& noteOn)
    {
        if (noteOn->velocity == 0)
            return handleNoteOff(std::make_shared<MIDI::NoteOff>(noteOn->note, 64));

        Music::NotePointer generatedNote = generateNote(noteOn);
        if (generatedNote)
        {
            std::unique_lock<std::mutex> lock(mutex);
            generatedNotes[noteOn->note] = generatedNote;
            return std::make_shared<MIDI::NoteOn>(generatedNote, noteOn->velocity);
        }
        return nullptr;
    }

    MIDI::NoteMessagePointer SingleHarmony::handleNoteOff(const MIDI::NoteOffPointer& noteOff)
    {
        std::unique_lock<std::mutex> lock(mutex);
        Music::NotePointer generatedNote = generatedNotes[noteOff->note];
        generatedNotes.erase(noteOff->note);
        if (generatedNote)
            return std::make_shared<MIDI::NoteOff>(generatedNote, noteOff->velocity);
        return nullptr;
    }
}