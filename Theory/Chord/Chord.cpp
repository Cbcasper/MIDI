//
// Created by Casper De Smet on 28/05/2022.
//

#include "Chord.h"

namespace Music
{
    Chord::Chord(const ISPointer& intervalSequence, const RootNotePointer& rootNote): rootNote(rootNote)
    {
        std::tie(third, std::ignore) = intervalSequence->modulate(rootNote, IntervalSequence::Third);
        std::tie(fifth, std::ignore) = intervalSequence->modulate(rootNote, IntervalSequence::Fifth);

        RootNote::Name lowestChordNote = (RootNote::Name) std::min({rootNote->name, third->name, fifth->name});
        if (lowestChordNote == rootNote->name)       rollOverModulation = IntervalSequence::First;
        else if (lowestChordNote == third->name)     rollOverModulation = IntervalSequence::Third;
        else if (lowestChordNote == fifth->name)     rollOverModulation = IntervalSequence::Fifth;
    }

    RootNotePointer Chord::operator()(IntervalSequence::Modulation modulation)
    {
        switch (modulation)
        {
            case IntervalSequence::First: return rootNote;
            case IntervalSequence::Third: return third;
            case IntervalSequence::Fifth: return fifth;
            default:                      return nullptr;
        }
    }
}