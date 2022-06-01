//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_CHORD_H
#define MIDIASSISTANT_CHORD_H

#include "../Note/RootNote.h"
#include "../Key/Key.h"

namespace Music
{
    class Chord;
    using ChordPointer = std::shared_ptr<Chord>;

    class Chord
    {
    public:
        KeyPointer key;

        RootNotePointer first;
        RootNotePointer third;
        RootNotePointer fifth;

        Key::Degree rollOverDegree;

        Chord(const KeyPointer& key, const RootNotePointer& rootNote);

        bool chordNote(const RootNotePointer& rootNote);
        RootNotePointer operator()(Key::Degree modulation);
        Key::Degree intersect(const ChordPointer& chord);
    };
}

#endif //MIDIASSISTANT_CHORD_H
