//
// Created by Casper De Smet on 28/05/2022.
//

#include "Chord.h"

namespace Music
{
    Chord::Chord(const KeyPointer& key, const RootNotePointer& rootNote): key(key), first(rootNote)
    {
        std::tie(third, std::ignore) = key->modulate(rootNote, Key::Third);
        std::tie(fifth, std::ignore) = key->modulate(rootNote, Key::Fifth);

        RootNote::Name lowestChordNote = (RootNote::Name) std::min({rootNote->name, third->name, fifth->name});
        if (lowestChordNote == rootNote->name)      rollOverDegree = Key::First;
        else if (lowestChordNote == third->name)    rollOverDegree = Key::Third;
        else if (lowestChordNote == fifth->name)    rollOverDegree = Key::Fifth;
    }

    bool Chord::chordNote(const RootNotePointer& rootNote)
    {
        return rootNote && (*rootNote == *first || *rootNote == *third || *rootNote == *fifth);
    }

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

    Key::Degree Chord::intersect(const ChordPointer& chord)
    {
        for (const RootNotePointer& note: {first, third, fifth})
            if (chord->chordNote(note))
                return key->getDegree(note);
        return Key::Invalid;
    }
}