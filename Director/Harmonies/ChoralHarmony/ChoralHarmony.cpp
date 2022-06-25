//
// Created by Casper De Smet on 28/05/2022.
//

#include "ChoralHarmony.h"
#include "../../../MIDI/IO/IOManager.h"

#include <iostream>

namespace Music
{
    // The tessitures are pre-defined and constant
    ChoralHarmony::ChoralHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application):
            Harmony(application, Choral, "Choral", output), key(application->song->key),
            sopranoTessiture(Tessiture(60, 81)), altoTessiture(Tessiture(55, 74)),
            tenorTessiture(Tessiture(48, 67)), bassTessiture(Tessiture(41, 62))
    {
        position = Narrow;
        inputRange = NoteFilter(bassTessiture.low, bassTessiture.high);
    }

    void ChoralHarmony::generate(const MIDI::NoteMessagePointer& noteMessage)
    {
        // Only notes in the current key can be analysed
        if (key->getDegree(noteMessage->note) == Key::Invalid)
            return;

        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(noteMessage))
            handleNoteOn(noteOn);
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(noteMessage))
            handleNoteOff(noteOff);
    }

    void ChoralHarmony::handleNoteOn(const MIDI::NoteOnPointer& noteOn)
    {
        // Generate the third and fifth note above the played note
        ChordPointer chord = std::make_shared<Chord>(key, noteOn->note);

        // Can only check if there was a note played before this one
        if (!lastPlayedNote || !handleLinks(noteOn, chord))
            handleDefaultCase(noteOn, chord);

        // Keep track of notes
        lastPlayedNote = noteOn->note;
        lastVoices = generatedNotes[noteOn->note];
    }

    // Check whether the previous and current note satisfy one of two conditions:
    // - The played note is a second above the previous
    // - The played note is at a difference of a perfect fifth from the previous
    bool ChoralHarmony::handleLinks(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        Key::Degree previousModulation = key->getDegree(lastPlayedNote);
        Key::Degree currentModulation = key->getDegree(noteOn->note);

        bool fifthLink = previousModulation == Key::First && checkFifthLink(currentModulation) ||
                         currentModulation == Key::First && checkFifthLink(previousModulation);
        bool secondLink = currentModulation == ((previousModulation + 1) % 7);

        // Both cases cannot occur at the same time
        if (fifthLink)          handleFifthLink(noteOn, chord);
        else if (secondLink)    handleSecondLink(noteOn, chord);
        return fifthLink || secondLink;
    }

    bool ChoralHarmony::checkFifthLink(Key::Degree degree)
    {
        return degree == Key::Fourth || degree == Key::Fifth;
    }

    // Generate a chord with one common note with the previously played chord
    void ChoralHarmony::handleFifthLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        // Find the common note and its degree in the chord
        Key::Degree commonDegree = chord->intersect(lastVoices->chord);
        NotePointer commonNote = (*lastVoices)[commonDegree];

        // Find the non-common voices
        std::set<NotePointer> otherNotes = {lastVoices->sopranoNote, lastVoices->altoNote, lastVoices->tenorNote};
        otherNotes.erase(commonNote);

        // The other voices either have to go up a second or down a second together
        VoicesPointer voices = makeSecondStep(chord, commonNote, otherNotes, true);
        if (!voices) voices = makeSecondStep(chord, commonNote, otherNotes, false);
        playVoices(voices, noteOn);
    }

    VoicesPointer ChoralHarmony::makeSecondStep(const ChordPointer& chord, const NotePointer& commonNote,
                                                const std::set<NotePointer>& otherNotes, bool up)
    {
        // Try to move the other notes in a direction by a second
        VoicesPointer voices = std::make_shared<Voices>(key, chord);
        (*voices)[lastVoices->voice(commonNote)] = commonNote;
        for (const NotePointer& note: otherNotes)
        {
            const auto& [modulatedNote, modulatedOctave] = key->modulate(note, Key::Second, up);
            // If the modulation results in a note that is note part of the chord, the direction is wrong
            if (!chord->chordNote(modulatedNote))
                return nullptr;
            (*voices)[lastVoices->voice(note)] = Note::getInstance(modulatedNote, note->octave + modulatedOctave);
        }
        return voices;
    }

    // Move all notes in the same direction as the played notes did such that there are no common notes,
    // two notes moving by a second and one note by a third
    void ChoralHarmony::handleSecondLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        bool up = noteOn->note->value > lastPlayedNote->value;

        VoicesPointer voices = std::make_shared<Voices>(key, chord);
        for (const NotePointer& note: {lastVoices->sopranoNote, lastVoices->altoNote, lastVoices->tenorNote})
            (*voices)[lastVoices->voice(note)] = makeStep(chord, note, !up);

        if (voices->tenorNote->value < noteOn->note->value) handleDefaultCase(noteOn, chord);
        else                                                playVoices(voices, noteOn);
    }

    // Try to move a note by a second, otherwise by a third if the resulting note is not in the key
    NotePointer ChoralHarmony::makeStep(const ChordPointer& chord, const NotePointer& note, bool up)
    {
        const auto& [second, secondOctave] = key->modulate(note, Key::Second, up);
        if (second && chord->chordNote(second))
            return Note::getInstance(second, note->octave + secondOctave);

        const auto& [third, thirdOctave] = key->modulate(note, Key::Third, up);
        if (third && chord->chordNote(third))
            return Note::getInstance(third, note->octave + thirdOctave);
        return nullptr;
    }

    // Choose a soprano based on the played bass note and generate the alto and tenor voice below it based on the position
    void ChoralHarmony::handleDefaultCase(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        VoicesPointer voices = std::make_shared<Voices>(key, chord);
        ChordNotePointer chordNote = computeSoprano(noteOn, chord);

        voices->sopranoNote = chordNote->getNote();

        // Move down to get to the next note
        *chordNote -= (int) position;
        voices->altoNote = chordNote->getNote();

        *chordNote -= (int) position;
        voices->tenorNote = chordNote->getNote();

        playVoices(voices, noteOn);
    }

    // Use G5, or the nearest chord note above as the soprano note
    ChordNotePointer ChoralHarmony::computeSoprano(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord)
    {
        NotePointer playedNote = noteOn->note;

        int minimumOffset = getMinimumOffset(chord);
        Tessiture rootNoteTessiture = Tessiture(playedNote->value + minimumOffset, 127);
        Tessiture possibleSopranoTessiture = sopranoTessiture.intersect(rootNoteTessiture);

        int octave = getIntendedOctave(chord);
        ChordNotePointer soprano = std::make_shared<ChordNote>(chord, Key::Third, octave);
        possibleSopranoTessiture(soprano);

        return soprano;
    }

    // Get the minimum difference between the bass note and the soprano note in order for the alto and tenor note to be generated
    int ChoralHarmony::getMinimumOffset(const ChordPointer& chord)
    {
        switch (position)
        {
            case Narrow:    return chord->fifth->computeOffset(chord->first);
            case Wide:      return 12 + chord->third->computeOffset(chord->first);
        }
    }

    int ChoralHarmony::getIntendedOctave(const ChordPointer& chord)
    {
        // If the third is before G in the C-B range, we've crossed into the next octave
        return (*chord)(Key::Third)->name >= Note::G ? 5 : 6;
    }

    // Generate the note on messages with the correct notes and play them. Save the generated voices
    void ChoralHarmony::playVoices(const VoicesPointer& voices, const MIDI::NoteOnPointer& noteOn)
    {
        MIDI::NoteOnPointer sopranoMessage = std::make_shared<MIDI::NoteOn>(voices->sopranoNote, noteOn->velocity);
        MIDI::NoteOnPointer altoMessage = std::make_shared<MIDI::NoteOn>(voices->altoNote, noteOn->velocity);
        MIDI::NoteOnPointer tenorMessage = std::make_shared<MIDI::NoteOn>(voices->tenorNote, noteOn->velocity);

        play(sopranoMessage);
        play(altoMessage);
        play(tenorMessage);

        std::unique_lock<std::mutex> lock(mutex);
        generatedNotes[noteOn->note] = voices;
    }

    // Finish the generated notes
    void ChoralHarmony::handleNoteOff(const MIDI::NoteOffPointer& noteOff)
    {
        std::unique_lock<std::mutex> lock(mutex);
        auto& [soprano, alto, tenor] = generatedNotes[noteOff->note];
        play(std::make_shared<MIDI::NoteOff>(soprano, noteOff->velocity));
        play(std::make_shared<MIDI::NoteOff>(alto, noteOff->velocity));
        play(std::make_shared<MIDI::NoteOff>(tenor, noteOff->velocity));
        generatedNotes.erase(noteOff->note);
    }

    // Gather all individual notes that are being played
    void ChoralHarmony::getSoundingNotes(std::set<int>& soundingNotes)
    {
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& [playedNote, voices]: generatedNotes)
        {
            soundingNotes.insert(voices->sopranoNote->value);
            soundingNotes.insert(voices->altoNote->value);
            soundingNotes.insert(voices->tenorNote->value);
        }
    }

    std::string ChoralHarmony::positionName(ChoralHarmony::Position position)
    {
        static std::map<Position, std::string> positionNames = {{Narrow, "Narrow"},
                                                                {Wide,   "Wide"}};
        return positionNames[position];
    }
}