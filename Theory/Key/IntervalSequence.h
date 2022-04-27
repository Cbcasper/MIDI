//
// Created by Casper De Smet on 10/04/2022.
//

#ifndef MIDIASSISTANT_INTERVALSEQUENCE_H
#define MIDIASSISTANT_INTERVALSEQUENCE_H

#include <map>
#include <memory>

#include "../Note/RootNote.h"
#include "../../Utilities/Utilities.h"

namespace Music
{
    class IntervalSequence;
    using ISPointer = std::shared_ptr<IntervalSequence>;

    class IntervalSequence
    {
    public:
        enum Type
        {
            // Modes
            Ionian,
            Dorian,
            Phrygian,
            Lydian,
            Mixolydian,
            Aeolian,
            Locrian,
            // Scales
            Major,
            NaturalMinor,
            HarmonicMinor,
            MelodicMinor
        };

        Type type;
        std::vector<RootNotePointer> intervalSequence;

        explicit IntervalSequence(Type type);

        static void fillSequence(std::vector<RootNotePointer>& sequence, const RootNotePointer& rootNote,
                                 const std::vector<int>& intervals);
        virtual void apply(const RootNotePointer& rootNote);
        virtual std::vector<int> getIntervals();

        virtual RootNotePointer getModulatedNote(int offset, int newIndex);
        std::pair<RootNotePointer, int> modulate(const RootNotePointer& note, int offset);

        virtual std::string toString();
        static std::string sequenceName(Type type);

        static std::vector<Type> allModes();
        static std::vector<Type> allScales();

        std::map<Type, Type> getTypeMap();
        bool ofType(Type otherType);
    };

    using Mode = IntervalSequence;
    using Scale = IntervalSequence;
}

#endif //MIDIASSISTANT_INTERVALSEQUENCE_H
