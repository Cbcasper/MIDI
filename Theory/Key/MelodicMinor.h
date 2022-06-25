//
// Created by Casper De Smet on 10/04/2022.
//

#ifndef MIDIASSISTANT_MELODICMINOR_H
#define MIDIASSISTANT_MELODICMINOR_H

#include "IntervalSequence.h"

namespace Music
{
    // Subclass of interval sequence that keeps track of the sequence when the link between the notes is downward
    class MelodicMinor: public IntervalSequence
    {
    public:
        std::vector<RootNotePointer> downwardSequence;

        MelodicMinor();

        // Inherit these methods in order to behave differently when going downwards
        void apply(const RootNotePointer& rootNote) override;
        RootNotePointer getModulatedNote(int index, bool up) override;
        std::string toString() override;
    };
}


#endif //MIDIASSISTANT_MELODICMINOR_H
