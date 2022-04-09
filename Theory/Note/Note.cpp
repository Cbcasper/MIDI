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

    Note::Note(Name name, int octave, bool sharp, int value): RootNote(name, sharp), octave(octave), value(value)
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
        unsigned char value = convert(instanceName, instanceSharp);
        return getInstance(value, instanceName, instanceOctave, instanceSharp);
    }

    NotePointer Note::getInstance(unsigned char value)
    {
        int octave = floor(value / 12);
        int localValue = value - 12 * octave;

        const auto& [name, sharp] = convert(localValue);
        return getInstance(value, name, octave, sharp);
    }

    std::string Note::toString() const
    {
        return RootNote::toString() + std::to_string(octave);
    }
}