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
        fillSequence(intervalSequence, rootNote, getIntervals());
    }

    std::vector<int> IntervalSequence::getIntervals()
    {
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

    RootNotePointer IntervalSequence::getModulatedNote(int offset, int newIndex)
    {
        return intervalSequence[newIndex];
    }

    std::pair<RootNotePointer, int> IntervalSequence::modulate(const RootNotePointer& note, int offset)
    {
        int index = 0;
        for (const RootNotePointer& diatonicNote: intervalSequence)
        {
            if (*diatonicNote == *note)
            {
                int offsetIndex = index + offset;
                int newIndex = Utilities::positiveModulo(offsetIndex % 7, 7);
                RootNotePointer modulatedNote = getModulatedNote(offset, newIndex);
                return std::make_pair(modulatedNote, static_cast<int>(floor(static_cast<float>(offsetIndex) / 7.0f)));
            }
            index++;
        }
        return std::make_pair(nullptr, 0);
    }

    std::string IntervalSequence::toString()
    {
        std::stringstream rep;
        for (const RootNotePointer& note: intervalSequence)
            rep << note->toString() << " - ";
        rep << intervalSequence[0]->toString();
        return rep.str();
    }
}