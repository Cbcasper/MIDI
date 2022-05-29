//
// Created by Casper De Smet on 28/05/2022.
//

#include "ChoralHarmony.h"
#include "../../../MIDI/IO/IOManager.h"

#include <iostream>

namespace Music
{
    ChoralHarmony::ChoralHarmony(const MIDI::InstrumentPointer& output, const KeyPointer& key):
                   Harmony(Choral, output), key(key), sopranoTessiture(Tessiture(60, 81)),
                   altoTessiture(Tessiture(55, 74)), tenorTessiture(Tessiture(48, 67)),
                   bassTessiture(Tessiture(41, 62))
    {
        position = Narrow;
    }

    void ChoralHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(noteMessage))
            handleNoteOn(noteOn);
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(noteMessage))
            handleNoteOff(noteOff);
    }

    void ChoralHarmony::handleNoteOn(const MIDI::NoteOnPointer& noteOn)
    {
        ChordPointer chord = std::make_shared<Chord>(key->intervalSequence, noteOn->note);

        ChordNotePointer sopranoNote = computeSoprano(noteOn, chord);
        MIDI::NoteOnPointer sopranoMessage = std::make_shared<MIDI::NoteOn>(sopranoNote->getNote(), noteOn->velocity);
        std::cout << "Notes: " << noteOn->note->toString() << " -> Soprano: " << sopranoNote->getNote()->toString();

        *sopranoNote -= (int) position;
        MIDI::NoteOnPointer altoMessage = std::make_shared<MIDI::NoteOn>(sopranoNote->getNote(), noteOn->velocity);
        std::cout << ", Alto: " << sopranoNote->getNote()->toString();

        *sopranoNote -= (int) position;
        MIDI::NoteOnPointer tenorMessage = std::make_shared<MIDI::NoteOn>(sopranoNote->getNote(), noteOn->velocity);
        std::cout << ", Tenor: " << sopranoNote->getNote()->toString() << "\n";

        play(sopranoMessage);
        play(altoMessage);
        play(tenorMessage);
        std::unique_lock<std::mutex> lock(mutex);
        generatedNotes[noteOn->note] = {sopranoMessage->note, altoMessage->note, tenorMessage->note};
    }

    ChordNotePointer ChoralHarmony::computeSoprano(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        NotePointer playedNote = noteOn->note;

        int minimumOffset = getMinimumOffset(chord);
        Tessiture rootNoteTessiture = Tessiture(playedNote->value + minimumOffset, 127);
        Tessiture possibleSopranoTessiture = sopranoTessiture.intersect(rootNoteTessiture);
        std::cout << "Tessiture: " << possibleSopranoTessiture.low << ", " << possibleSopranoTessiture.high << "\n";

        IntervalSequence::Modulation intendedModulation = getIntendedModulation(chord);
        int octave = 5 + (int)((*chord)(intendedModulation)->name < RootNote::G);
        ChordNotePointer soprano = std::make_shared<ChordNote>(chord, intendedModulation, octave);
        possibleSopranoTessiture(soprano);

        return soprano;
    }

    int ChoralHarmony::getMinimumOffset(const ChordPointer& chord)
    {
        switch (position)
        {
            case Narrow:    return chord->fifth->computeOffset(chord->rootNote);
            case Wide:      return 12 + chord->third->computeOffset(chord->rootNote);
        }
    }

    IntervalSequence::Modulation ChoralHarmony::getIntendedModulation(const ChordPointer& chord)
    {
        RootNotePointer G = std::make_shared<RootNote>(Note::G, false);

        IntervalSequence::Modulation intendedModulation = IntervalSequence::First;
        for (IntervalSequence::Modulation modulation: {IntervalSequence::Third, IntervalSequence::Fifth})
            if ((*chord)(modulation)->computeOffset(G) < (*chord)(intendedModulation)->computeOffset(G))
                intendedModulation = modulation;
        std::cout << "Modulation: " << intendedModulation << "\n";
        return intendedModulation;
    }

    void ChoralHarmony::handleNoteOff(const MIDI::NoteOffPointer& noteOff)
    {
        std::unique_lock<std::mutex> lock(mutex);
        auto& [soprano, alto, tenor] = generatedNotes[noteOff->note];
        if (soprano && alto && tenor)
        {
            play(std::make_shared<MIDI::NoteOff>(soprano, noteOff->velocity));
            play(std::make_shared<MIDI::NoteOff>(alto, noteOff->velocity));
            play(std::make_shared<MIDI::NoteOff>(tenor, noteOff->velocity));
        }
        generatedNotes.erase(noteOff->note);
    }

    void ChoralHarmony::getSoundingNotes(std::set<int>& soundingNotes)
    {
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& [playedNote, generatedNoteSet]: generatedNotes)
            for (const NotePointer& note: generatedNoteSet)
                soundingNotes.insert(note->value);
    }

    std::string ChoralHarmony::positionName(ChoralHarmony::Position position)
    {
        static std::map<Position, std::string> positionNames = {{Narrow, "Narrow"},
                                                                {Wide, "Wide"}};
        return positionNames[position];
    }
}