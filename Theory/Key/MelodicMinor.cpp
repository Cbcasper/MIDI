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
        fillSequence(intervalSequence, rootNote, {2, 1, 2, 2, 2, 2});
        fillSequence(downwardSequence, rootNote, {2, 1, 2, 2, 1, 2});
    }

    RootNotePointer MelodicMinor::getModulatedNote(int offset, int newIndex)
    {
        if (offset < 0)
            return downwardSequence[newIndex];
        else
            return intervalSequence[newIndex];
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
