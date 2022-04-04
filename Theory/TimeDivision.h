//
// Created by Casper De Smet on 30/03/2022.
//

#ifndef MIDIASSISTANT_TIMEDIVISION_H
#define MIDIASSISTANT_TIMEDIVISION_H

#include <map>
#include <string>

namespace Music
{
    enum TimeDivision
    {
        Second = 2,
        Fourth = 4,
        Eighth = 8,
        Sixteenth = 16,
        ThirtySecond = 32,
        SixtyFourth = 64
    };

    extern std::map<TimeDivision, std::string> allDivisions;
}

#endif //MIDIASSISTANT_TIMEDIVISION_H
