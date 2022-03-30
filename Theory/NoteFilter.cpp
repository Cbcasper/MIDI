//
// Created by Casper De Smet on 23/03/2022.
//

#include "NoteFilter.h"

namespace Music
{
    NoteFilter::NoteFilter(): low(0), high(127)
    {
        for (int i = 0; i < 128; ++i)
            allowedNotes[i] = true;
    }

    NoteFilter::NoteFilter(int low, int high): low(low), high(high)
    {
        for (int i = 0; i < 128; ++i)
            allowedNotes[i] = i >= low && i <= high;
    }

    void NoteFilter::moveLow(int newLow)
    {
        for (int i = std::min(low, newLow); i < std::max(low, newLow); ++i)
            allowedNotes[i] = !allowedNotes[i];
    }

    void NoteFilter::moveHigh(int newHigh)
    {
        for (int i = std::min(high, newHigh); i < std::max(high, newHigh); ++i)
            allowedNotes[i] = !allowedNotes[i];
    }

    void NoteFilter::setRange(int newLow, int newHigh)
    {
        low = newLow;
        high = newHigh;
        for (int i = 0; i < 128; ++i)
            allowedNotes[i] = i >= low && i <= high;
    }

    bool NoteFilter::operator()(const NotePointer& note)
    {
        return allowedNotes[note->value];
    }
}