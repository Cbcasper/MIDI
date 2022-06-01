//
// Created by Casper De Smet on 31/05/2022.
//

#include "Average.h"

namespace Utilities
{
    Average::Average(): total(0), numberOfElements(0)
    {}

    int Average::get()
    {
        return total / numberOfElements;
    }

    void Average::operator+=(int value)
    {
        total += value;
        numberOfElements++;
    }
}