//
// Created by Casper De Smet on 09/04/2022.
//

#include "RootNote.h"
#include "../../Utilities/Utilities.h"

#include <sstream>
#include <cmath>
#include <iostream>

namespace Music
{
    RootNote::RootNote(RootNote::Name name, bool sharp)
    {
        std::tie(this->name, this->sharp) = correct(name, sharp);
    }

    std::pair<RootNote::Name, bool> RootNote::correct(RootNote::Name name, bool sharp)
    {
        // B sharp is actually C
        if (name == B && sharp)
            return std::make_pair(C, false);
        // E sharp is actually F
        else if (name == E && sharp)
            return std::make_pair(F, false);
        else
            return std::make_pair(name, sharp);
    }

    std::pair<RootNote::Name, bool> RootNote::convert(int value)
    {
        if (value > 4) value++;
        return std::make_pair(Name(floor(value / 2)), value % 2 == 1);
    }

    int RootNote::convert(RootNote::Name name, bool sharp)
    {
        int value = ((int) name) * 2 - ((int) name > 2);
        return value + (int) sharp;
    }

    int RootNote::computeOffset(const RootNotePointer& rootNote) const
    {
        int value = convert(name, sharp);
        int otherValue = convert(rootNote->name, rootNote->sharp);
        return Utilities::positiveModulo(value - otherValue, 12);
    }

    RootNotePointer RootNote::operator+(int semitones) const
    {
        int value = convert(name, sharp);
        const auto& [newName, newSharp] = convert((value + semitones + 12) % 12);
        return std::make_shared<RootNote>(newName, newSharp);
    }

    std::string RootNote::toString() const
    {
        std::stringstream rep;
        rep << noteNames()[name];
        if (sharp) rep << "#";
        return rep.str();
    }

    std::vector<char> RootNote::noteNames()
    {
        static std::vector<char> noteNames = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
        return noteNames;
    }

    bool RootNote::operator==(const RootNote& rootNote)
    {
        return name == rootNote.name && sharp == rootNote.sharp;
    }
}