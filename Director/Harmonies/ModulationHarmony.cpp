//
// Created by Casper De Smet on 11/04/2022.
//

#include "ModulationHarmony.h"

namespace Music
{
    ModulationHarmony::ModulationHarmony(const std::string& port, int channel, int modulationOffset):
                       Harmony(port, channel), modulationOffset(modulationOffset)
    {}
}
