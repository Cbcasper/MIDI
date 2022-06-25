//
// Created by Casper De Smet on 09/04/2022.
//

#include "Key.h"

#include <cmath>

#include "MelodicMinor.h"

namespace Music
{
    Key::Key()
    {
        // Default is C Major
        rootNote = std::make_shared<RootNote>(RootNote::C, false);
        intervalSequence = std::make_shared<IntervalSequence>(Scale::Major);
        intervalSequence->apply(rootNote);
    }

    Key::Degree Key::getDegree(const RootNotePointer& note)
    {
        // Wrap the method of the interval sequence
        return (Degree) intervalSequence->getDegree(note);
    }

    std::pair<RootNotePointer, int> Key::modulate(const RootNotePointer& note, Key::Degree modulation, bool up)
    {
        // Calculate the offset and the current index
        int offset = ((int) modulation) * (up ? 1 : -1);
        int index = intervalSequence->getDegree(note);

        if ((Degree) index != Invalid)
        {
            // Offset the index and correct it to be a value between 0 and 7
            int offsetIndex = index + offset;
            int newIndex = Utilities::positiveModulo(offsetIndex, 7);

            // Turn the index into a note
            RootNotePointer modulatedNote = intervalSequence->getModulatedNote(newIndex, up);
            return std::make_pair(modulatedNote, getOctaveOffset((int) note->name, (int) modulatedNote->name, up));
        }
        return std::make_pair(nullptr, 0);
    }

    // Compare the index to find whether the new note has crossed into a new octave
    int Key::getOctaveOffset(int index, int newIndex, bool up)
    {
        if (up) return newIndex < index ? 1 : 0;
        else    return index < newIndex ? -1 : 0;
    }

    // Set the parameters and recalculate the sequence
    void Key::setRootNote(RootNote::Name name, bool sharp)
    {
        rootNote = std::make_shared<RootNote>(name, sharp);
        intervalSequence->apply(rootNote);
    }

    void Key::setRootNote(const std::pair<RootNote::Name, bool>& note)
    {
        const auto& [name, sharp] = note;
        return setRootNote(name, sharp);
    }

    void Key::setIntervalSequence(IntervalSequence::Type type)
    {
        if (type == Scale::MelodicMinor) intervalSequence = std::make_shared<MelodicMinor>();
        else                             intervalSequence = std::make_shared<IntervalSequence>(type);
        intervalSequence->apply(rootNote);
    }

    std::string Key::degreeName(Key::Degree degree)
    {
        static std::map<Degree, std::string> names = {{First,   "First"},
                                                      {Second,  "Second"},
                                                      {Third,   "Third"},
                                                      {Fourth,  "Fourth"},
                                                      {Fifth,   "Fifth"},
                                                      {Sixth,   "Sixth"},
                                                      {Seventh, "Seventh"}};
        return names[degree];
    }
}