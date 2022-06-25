//
// Created by Casper De Smet on 28/05/2022.
//

#include "Chord.h"

namespace Music
{
    Chord::Chord(const KeyPointer& key, const RootNotePointer& rootNote): key(key), first(rootNote)
    {
        // Calculate notes
        std::tie(third, std::ignore) = key->modulate(rootNote, Key::Third);
        std::tie(fifth, std::ignore) = key->modulate(rootNote, Key::Fifth);

        RootNote::Name lowestChordNote = (RootNote::Name) std::min({rootNote->name, third->name, fifth->name});
        if (lowestChordNote == rootNote->name)      rollOverDegree = Key::First;
        else if (lowestChordNote == third->name)    rollOverDegree = Key::Third;
        else if (lowestChordNote == fifth->name)    rollOverDegree = Key::Fifth;
    }

    // Check whether a given note is one of the three notes in the chord
    bool Chord::chordNote(const RootNotePointer& rootNote)
    {
        return rootNote && (*rootNote == *first || *rootNote == *third || *rootNote == *fifth);
    }

    // Return the note matching the degree
    RootNotePointer Chord::operator()(Key::Degree modulation)
    {
        switch (modulation)
        {
            case Key::First: return first;
            case Key::Third: return third;
            case Key::Fifth: return fifth;
            default:         return nullptr;
        }
    }

    // Compute common note
    Key::Degree Chord::intersect(const ChordPointer& chord)
    {
        for (const RootNotePointer& note: {first, third, fifth})
            if (chord->chordNote(note))
                return key->getDegree(note);
        return Key::Invalid;
    }
}