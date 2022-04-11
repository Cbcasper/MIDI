//
// Created by Casper De Smet on 23/03/2022.
//

#include "Harmony.h"

namespace Music
{
    Harmony::Harmony(const std::string& port, int channel)
    {
        audioPlayer = MIDI::AudioPlayer();
        output = std::make_shared<MIDI::Instrument>(port, channel);
        noteFilter = NoteFilter();
    }

    void Harmony::processMessage(const MIDI::NoteMessagePointer& noteMessage)
    {
        if (noteFilter(noteMessage->note))
        {
            MIDI::NoteMessagePointer generatedNoteMessage = generate(noteMessage);
            if (chainedHarmony) chainedHarmony->processMessage(generatedNoteMessage);
        }
    }
}