//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_HARMONY_H
#define MIDIASSISTANT_HARMONY_H

#include "../../Theory/Note/NoteFilter.h"
#include "../../MIDI/Messages/Messages/NoteMessage.h"
#include "../../MIDI/AudioPlayer.h"
#include "../../MIDI/Messages/Instrument.h"

namespace Music
{
    class Harmony;
    using HarmonyPointer = std::shared_ptr<Harmony>;

    class Harmony
    {
    public:
        MIDI::AudioPlayer audioPlayer;
        MIDI::InstrumentPointer output;

        HarmonyPointer chainedHarmony;

        NoteFilter noteFilter;
        std::map<NotePointer, NotePointer> soundingNotes;

        Harmony(const std::string& port, int channel);

        void processMessage(const MIDI::NoteMessagePointer& noteMessage);
        virtual MIDI::NoteMessagePointer generate(const MIDI::NoteMessagePointer& noteMessage) = 0;
    };
}

#endif //MIDIASSISTANT_HARMONY_H
