//
// Created by Casper De Smet on 10/04/2022.
//

#ifndef MIDIASSISTANT_MELODICMINOR_H
#define MIDIASSISTANT_MELODICMINOR_H

#include "IntervalSequence.h"

namespace Music
{
    class MelodicMinor: public IntervalSequence
    {
    public:
        std::vector<RootNotePointer> downwardSequence;

        MelodicMinor();

        void apply(const RootNotePointer& rootNote) override;
        RootNotePointer getModulatedNote(int index, bool up) override;
        std::string toString() override;
    };
}


#endif //MIDIASSISTANT_MELODICMINOR_H
