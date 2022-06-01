//
// Created by Casper De Smet on 31/05/2022.
//

#ifndef MIDIASSISTANT_AVERAGE_H
#define MIDIASSISTANT_AVERAGE_H

namespace Utilities
{
    class Average
    {
    public:
        int total;
        int numberOfElements;

        Average();

        int get();
        void operator+=(int value);
    };
}

#endif //MIDIASSISTANT_AVERAGE_H
