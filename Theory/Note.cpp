//
// Created by Casper De Smet on 02/03/2022.
//

#include <sstream>
#include <cmath>

#include "Note.h"

namespace Music
{
    std::map<unsigned char, NotePointer> Note::notes;
    std::mutex Note::mutex;

    Note::Note(Name name, int octave, bool sharp, int value): name(name), octave(octave), sharp(sharp), value(value)
    {}

    NotePointer Note::getInstance(unsigned char value, Name instanceName, int instanceOctave, bool instanceSharp)
    {
        if (!notes[value])
        {
            mutex.lock();
            if (!notes[value])
                notes[value] = NotePointer(new Note(instanceName, instanceOctave, instanceSharp, value));
            mutex.unlock();
        }
        return notes[value];
    }

    NotePointer Note::getInstance(Name instanceName, int instanceOctave, bool instanceSharp)
    {
        int localValue = (int) instanceName * 2;
        if (localValue > 5) localValue--;
        unsigned char value = (instanceOctave + 2) * 12 + localValue + (int) instanceSharp;
        return getInstance(value, instanceName, instanceOctave, instanceSharp);
    }

    NotePointer Note::getInstance(unsigned char value)
    {
        int octave = floor(value / 12);
        int localValue = value - 12 * octave;

        if (localValue > 4) localValue++;
        return getInstance(value, Name(floor(localValue / 2)),
                           octave - 2, localValue % 2 == 1);
    }

    Note::NoteKey Note::getKey(Note::Name instanceName, int instanceOctave, bool instanceSharp)
    {
        if (instanceName == B && instanceSharp)
            return std::make_tuple(C, instanceOctave, false);
        else if (instanceName == E && instanceSharp)
            return std::make_tuple(F, instanceOctave, false);
        else
            return std::make_tuple(instanceName, instanceOctave, instanceSharp);
    }

    std::string Note::toString() const
    {
        std::stringstream rep;
        rep << noteNames()[name];
        if (sharp) rep << "#";
        rep << octave;
        return rep.str();
    }

    std::vector<char> Note::noteNames()
    {
        return {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
    }
}