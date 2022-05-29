//
// Created by Casper De Smet on 23/03/2022.
//

#include "Harmony.h"

#include "../../MIDI/IO/IOManager.h"

namespace Music
{
    Harmony::Harmony(Type type, const MIDI::InstrumentPointer& output):
                     type(type), output(output), selected(false)
    {
        audioPlayer = MIDI::AudioPlayer();
        inputRange = NoteFilter();
        name = harmonyName(type);
    }

    void Harmony::processMessage(const MIDI::NoteMessagePointer& noteMessage)
    {
        if (inputRange(noteMessage->note))
            generate(noteMessage);
    }

    void Harmony::play(const MIDI::NoteMessagePointer& generated)
    {
        MIDI::IOManagerPointer ioManager = MIDI::IOManager::getInstance();
        ioManager->sendMIDIOut(std::make_pair(generated, output));
        audioPlayer.processMIDIMessage(generated->rawMessage(output->channel));

        if (chainedHarmony) chainedHarmony->processMessage(generated);
    }

    void Harmony::getSoundingNotes(std::set<int>& soundingNotes)
    {
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& [note, generatedNote]: generatedNotes)
            soundingNotes.insert(generatedNote->value);
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