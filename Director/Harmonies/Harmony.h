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

    // Super class for all harmonies
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

        // Display purposes
        Type type;
        std::string typeName;

        bool selected;

        // Protect the generated notes data structure
        std::mutex mutex;

        // IO
        MIDI::InstrumentPointer output;
        MIDI::AudioPlayerPointer audioPlayer;

        // Not implemented
        HarmonyPointer chainedHarmony;

        // Notes
        NoteFilter inputRange;
        std::map<NotePointer, NotePointer> generatedNotes;

        Harmony(const State::ApplicationPointer& application, Type type,
                const std::string& typeName, const MIDI::InstrumentPointer& output);

        // Processing of messages
        void processMessage(const MIDI::NoteMessagePointer& noteMessage);
        virtual void generate(const MIDI::NoteMessagePointer& noteMessage) = 0;
        virtual void play(const MIDI::NoteMessagePointer& generated);

        // Query method
        virtual void getSoundingNotes(std::set<int>& soundingNotes);

        static std::string harmonyName(Type type);
        static std::vector<Type> allHarmonies();
    };
}

#endif //MIDIASSISTANT_HARMONY_H
