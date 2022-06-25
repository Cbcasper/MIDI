//
// Created by Casper De Smet on 10/04/2022.
//

#include <sstream>
#include <cmath>

#include "IntervalSequence.h"

namespace Music
{
    IntervalSequence::IntervalSequence(IntervalSequence::Type type): type(type)
    {}

    void IntervalSequence::fillSequence(std::vector<RootNotePointer>& sequence, const RootNotePointer& rootNote,
                                        const std::vector<int>& intervals)
    {
        // Start with the root note and move it up according to the intervals
        RootNotePointer previousNote = rootNote;
        sequence = {rootNote};
        for (int interval: intervals)
        {
            RootNotePointer newNote = *previousNote + interval;
            sequence.emplace_back(newNote);
            previousNote = newNote;
        }
    }

    void IntervalSequence::apply(const RootNotePointer& rootNote)
    {
        // A general interval sequence is theoretically predefined and the same upwards as downwards
        fillSequence(intervalSequence, rootNote, getIntervals());
    }

    std::vector<int> IntervalSequence::getIntervals()
    {
        // Theoretical sequences
        std::map<Type, std::vector<int>> intervals = {{Ionian,          {2, 2, 1, 2, 2, 2}},
                                                      {Dorian,          {2, 1, 2, 2, 2, 1}},
                                                      {Phrygian,        {1, 2, 2, 2, 1, 2}},
                                                      {Lydian,          {2, 2, 2, 1, 2, 2}},
                                                      {Mixolydian,      {2, 2, 1, 2, 2, 1}},
                                                      {Aeolian,         {2, 1, 2, 2, 1, 2}},
                                                      {Locrian,         {1, 2, 2, 1, 2, 2}},
                                                      {Major,           {2, 2, 1, 2, 2, 2}},
                                                      {NaturalMinor,    {2, 1, 2, 2, 1, 2}},
                                                      {HarmonicMinor,   {2, 1, 2, 2, 1, 3}}};
        return intervals[type];
    }

    // Run through the notes until the correct one is found
    int IntervalSequence::getDegree(const RootNotePointer& note)
    {
        for (int i = 0; i < 7; ++i)
            if (*intervalSequence[i] == *note)
                return i;
        return -1;
    }

    RootNotePointer IntervalSequence::getModulatedNote(int index, bool up)
    {
        // By default, the note is just the note at that index in the sequence, "up" is unnecessary in this case
        return intervalSequence[index];
    }

    std::string IntervalSequence::toString()
    {
        std::stringstream rep;
        for (const RootNotePointer& note: intervalSequence)
            rep << note->toString() << " - ";
        rep << intervalSequence[0]->toString();
        return rep.str();
    }

    // These methods are mostly for display purposes
    std::string IntervalSequence::sequenceName(IntervalSequence::Type sequenceType)
    {
        static std::map<Type, std::string> sequenceNames = {{Ionian,          "Ionian"},
                                                            {Dorian,          "Dorian"},
                                                            {Phrygian,        "Phrygian"},
                                                            {Lydian,          "Lydian"},
                                                            {Mixolydian,      "Mixolydian"},
                                                            {Aeolian,         "Aeolian"},
                                                            {Locrian,         "Locrian"},
                                                            {Major,           "Major"},
                                                            {NaturalMinor,    "Natural Minor"},
                                                            {HarmonicMinor,   "Harmonic Minor"},
                                                            {MelodicMinor,    "Melodic Minor"}};
        return sequenceNames[sequenceType];
    }

    std::vector<Mode::Type> IntervalSequence::allModes()
    {
        return {Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian};
    }

    std::vector<Scale::Type> IntervalSequence::allScales()
    {
        return {Major, NaturalMinor, HarmonicMinor, MelodicMinor};
    }

    // There are some scales that are technically the same as a mode
    IntervalSequence::Type IntervalSequence::mapType(Type type)
    {
        if (type == Major)              return Ionian;
        else if (type == NaturalMinor)  return Aeolian;
        else                            return type;
    }

    bool IntervalSequence::ofType(IntervalSequence::Type otherType) {
        return mapType(type) == mapType(otherType);
    }
}