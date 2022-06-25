//
// Created by Casper De Smet on 23/03/2022.
//

#include "Harmony.h"

#include "../../MIDI/IO/IOManager.h"

namespace Music
{
    Harmony::Harmony(const State::ApplicationPointer& application, Type type,
                     const std::string& typeName, const MIDI::InstrumentPointer& output):
                     type(type), typeName(typeName), output(output), selected(false)
    {
        audioPlayer = std::make_shared<MIDI::AudioPlayer>(application);
        inputRange = NoteFilter();
    }

    void Harmony::processMessage(const MIDI::NoteMessagePointer& noteMessage)
    {
        // Process the message only if it is in range
        if (inputRange(noteMessage->note))
            generate(noteMessage);
    }

    void Harmony::play(const MIDI::NoteMessagePointer& generated)
    {
        // Play the message and send MIDI Out
        MIDI::IOManagerPointer ioManager = MIDI::IOManager::getInstance();
        audioPlayer->processMIDIMessage(std::make_pair(generated, output));
        ioManager->sendMIDIOut(std::make_pair(generated, output));

        // Pass the generated message to the next harmony
        if (chainedHarmony) chainedHarmony->processMessage(generated);
    }

    void Harmony::getSoundingNotes(std::set<int>& soundingNotes)
    {
        // Get all notes that are currently playing
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