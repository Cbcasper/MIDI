//
// Created by Casper De Smet on 09/05/2022.
//

#ifndef MIDIASSISTANT_CYCLE_H
#define MIDIASSISTANT_CYCLE_H

#include <memory>

namespace System
{
    // Class that implements a cycle in the sequencer
    class Cycle
    {
    public:
        bool cycling;
        int start;
        int end;

        Cycle(int start, int end);

        // Reset the ticks to the beginning if we are at the end
        int set(int ticks);
    };

    using CyclePointer = std::shared_ptr<Cycle>;
}

#endif //MIDIASSISTANT_CYCLE_H
