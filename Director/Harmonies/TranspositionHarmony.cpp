//
// Created by Casper De Smet on 24/03/2022.
//

#include "TranspositionHarmony.h"
#include "../../Theory/Theory.h"
#include "../../MIDI/IO/IOManager.h"
#include "../../MIDI/Messages/Messages/NoteOn.h"
#include "../../MIDI/Messages/Messages/NoteOff.h"

namespace Music
{
    TranspositionHarmony::TranspositionHarmony(const std::string& port, int channel, int transpositionOffset):
                          Harmony(port, channel), transpositionOffset(transpositionOffset)
    {}

    MIDI::NoteMessagePointer TranspositionHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        TheoryPointer theory = Theory::getInstance();
        MIDI::IOManagerPointer ioManager = MIDI::IOManager::getInstance();

        NotePointer modulatedNote;
        MIDI::NoteMessagePointer modulatedMessage;
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(noteMessage))
        {
            modulatedNote = Music::Note::getInstance(noteOn->note->value + transpositionOffset);
            modulatedMessage = std::make_shared<MIDI::NoteOn>(modulatedNote, noteOn->velocity);
            soundingNotes[noteOn->note] = modulatedNote;
        }
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(noteMessage))
        {
            modulatedNote = soundingNotes[noteOff->note];
            modulatedMessage = std::make_shared<MIDI::NoteOff>(modulatedNote, noteOff->velocity);
            soundingNotes.erase(noteOff->note);
        }
        ioManager->sendMIDIOut(std::make_pair(modulatedMessage, output));
        audioPlayer.processMIDIMessage(modulatedMessage->rawMessage(output->channel));
        return modulatedMessage;
    }
}