//
// Created by Casper De Smet on 31/05/2022.
//

#ifndef MIDIASSISTANT_AVERAGE_H
#define MIDIASSISTANT_AVERAGE_H

namespace Utilities
{
    // Dynamically computed average
    class Average
    {
    public:
        int total;
        int numberOfElements;

        Average();

        // Compute the actual average
        int get();
        // Aggregate a new value
        void operator+=(int value);
    };
}

#endif //MIDIASSISTANT_AVERAGE_H
