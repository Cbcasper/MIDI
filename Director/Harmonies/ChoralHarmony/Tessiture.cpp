//
// Created by Casper De Smet on 28/05/2022.
//

#include "Tessiture.h"

#include <cmath>
#include <algorithm>

namespace Music
{
    Tessiture::Tessiture(int low, int high): low(low), high(high)
    {}

    Tessiture Tessiture::intersect(const Tessiture& tessiture)
    {
        int newLow = std::max(low, tessiture.low);
        int newHigh = std::min(high, tessiture.high);
        if (newHigh < newLow)
            return Tessiture(-1, 128);
        return Tessiture(newLow, newHigh);
    }

    void Tessiture::operator()(const ChordNotePointer& chordNote)
    {
        while (chordNote->getNote()->value < low)
            ++*chordNote;
        while (chordNote->getNote()->value > high)
            --*chordNote;
    }
}