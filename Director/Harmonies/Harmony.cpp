//
// Created by Casper De Smet on 23/03/2022.
//

#include "Harmony.h"

namespace Music
{
    Harmony::Harmony(Type type, const MIDI::InstrumentPointer& output): type(type), output(output)
    {
        audioPlayer = MIDI::AudioPlayer();
        noteFilter = NoteFilter();
        name = harmonyName(type);
    }

    void Harmony::processMessage(const MIDI::NoteMessagePointer& noteMessage)
    {
        if (noteFilter(noteMessage->note))
        {
            MIDI::NoteMessagePointer generatedNoteMessage = generate(noteMessage);
            if (chainedHarmony) chainedHarmony->processMessage(generatedNoteMessage);
        }
    }

    std::string Harmony::harmonyName(Harmony::Type type)
    {
        static std::map<Type, std::string> harmonyNames = {{Random, "Random"},
                                                           {Transposition, "Transposition"},
                                                           {Modulation, "Modulation"},
                                                           {Canon, "Canon"},
                                                           {Choral, "Choral"}};
        return harmonyNames[type];
    }

    std::vector<Harmony::Type> Harmony::allHarmonies()
    {
        return {Random, Transposition, Modulation, Canon, Choral};
    }
}