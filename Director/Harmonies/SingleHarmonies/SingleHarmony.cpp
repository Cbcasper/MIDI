//
// Created by Casper De Smet on 11/04/2022.
//

#include "SingleHarmony.h"
#include "../../../Theory/Theory.h"
#include "../../../MIDI/IO/IOManager.h"

namespace Music
{
    SingleHarmony::SingleHarmony(const std::string& port, int channel): Harmony(port, channel)
    {}

    MIDI::NoteMessagePointer SingleHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        MIDI::IOManagerPointer ioManager = MIDI::IOManager::getInstance();

        MIDI::NoteMessagePointer generatedMessage;
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(noteMessage))
            handleNoteOn(noteOn, generatedMessage);
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(noteMessage))
            handleNoteOff(noteOff, generatedMessage);

        if (generatedMessage)
        {
            ioManager->sendMIDIOut(std::make_pair(generatedMessage, output));
            audioPlayer.processMIDIMessage(generatedMessage->rawMessage(output->channel));
        }
        return generatedMessage;
    }

    void SingleHarmony::handleNoteOn(const MIDI::NoteOnPointer& noteOn, MIDI::NoteMessagePointer& generatedMessage)
    {
        if (noteOn->velocity == 0)
            return handleNoteOff(std::make_shared<MIDI::NoteOff>(noteOn->note, 64), generatedMessage);

        const auto& [generated, generatedNote] = generateNote(noteOn);
        if (generated)
        {
            generatedMessage = std::make_shared<MIDI::NoteOn>(generatedNote, noteOn->velocity);
            soundingNotes[noteOn->note] = generatedNote;
        }
    }

    void SingleHarmony::handleNoteOff(const MIDI::NoteOffPointer& noteOff, MIDI::NoteMessagePointer& generatedMessage)
    {
        Music::NotePointer generatedNote = soundingNotes[noteOff->note];
        if (generatedNote)
        {
            generatedMessage = std::make_shared<MIDI::NoteOff>(generatedNote, noteOff->velocity);
            soundingNotes.erase(noteOff->note);
        }
    }
}