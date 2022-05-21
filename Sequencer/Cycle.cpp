//
// Created by Casper De Smet on 09/05/2022.
//

#include "Cycle.h"

#include <iostream>

namespace System
{
    Cycle::Cycle(int start, int end): start(start), end(end)
    {
        cycling = false;
    }

    int Cycle::set(int ticks)
    {
        if (cycling && ticks == end)
            return start;
        return ticks;
    }
}