//
// Created by Casper De Smet on 09/04/2022.
//

#include "RootNote.h"

#include <sstream>
#include <cmath>

namespace Music
{
    RootNote::RootNote(RootNote::Name name, bool sharp)
    {
        std::tie(this->name, this->sharp) = correct(name, sharp);
    }

    std::pair<RootNote::Name, bool> RootNote::correct(RootNote::Name name, bool sharp)
    {
        if (name == B && sharp)
            return std::make_pair(C, false);
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
        int value = name * 2 - (int) name > 1;
        return value + (int) sharp;
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
        return {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
    }
}