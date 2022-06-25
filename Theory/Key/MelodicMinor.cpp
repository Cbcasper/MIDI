//
// Created by Casper De Smet on 10/04/2022.
//

#include "MelodicMinor.h"

#include <sstream>

namespace Music
{
    MelodicMinor::MelodicMinor(): IntervalSequence(Scale::MelodicMinor)
    {}

    void MelodicMinor::apply(const RootNotePointer& rootNote)
    {
        // Compute the two sequences
        fillSequence(intervalSequence, rootNote, {2, 1, 2, 2, 2, 2});
        fillSequence(downwardSequence, rootNote, {2, 1, 2, 2, 1, 2});
    }

    RootNotePointer MelodicMinor::getModulatedNote(int index, bool up)
    {
        // Different behaviour when going downwards
        if (up)  return intervalSequence[index];
        else     return downwardSequence[index];
    }

    std::string MelodicMinor::toString()
    {
        std::stringstream rep;
        for (const RootNotePointer& note: intervalSequence)
            rep << note->toString() << " - ";
        rep << intervalSequence[0]->toString();
        for (auto noteIterator = downwardSequence.rbegin(); noteIterator != downwardSequence.rend(); ++noteIterator)
            rep << " - " << (*noteIterator)->toString();
        return rep.str();
    }
}
