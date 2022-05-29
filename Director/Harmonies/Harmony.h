//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_HARMONY_H
#define MIDIASSISTANT_HARMONY_H

#include <set>

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
        enum Type
        {
            Random,
            Transposition,
            Modulation,
            Canon,
            Choral
        };

        Type type;
        std::string name;

        bool selected;

        std::mutex mutex;

        MIDI::InstrumentPointer output;
        MIDI::AudioPlayer audioPlayer;

        HarmonyPointer chainedHarmony;

        NoteFilter inputRange;
        std::map<NotePointer, NotePointer> generatedNotes;

        Harmony(Type type, const MIDI::InstrumentPointer& output);

        void processMessage(const MIDI::NoteMessagePointer& noteMessage);
        virtual void generate(const MIDI::NoteMessagePointer& noteMessage) = 0;
        virtual void play(const MIDI::NoteMessagePointer& generated);

        virtual void getSoundingNotes(std::set<int>& soundingNotes);

        static std::string harmonyName(Type type);
        static std::vector<Type> allHarmonies();
    };
}

#endif //MIDIASSISTANT_HARMONY_H
