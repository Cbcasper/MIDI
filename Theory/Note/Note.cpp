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

    Note::Note(Name name, bool sharp, int octave, int value) : RootNote(name, sharp), octave(octave), value(value)
    {}

    NotePointer Note::getInstance(unsigned char value, Name name, bool sharp, int octave)
    {
        if (!notes[value])
        {
            mutex.lock();
            if (!notes[value])
                notes[value] = NotePointer(new Note(name, sharp, octave, value));
            mutex.unlock();
        }
        return notes[value];
    }

    NotePointer Note::getInstance(const RootNotePointer& rootNote, int octave)
    {
        return getInstance(rootNote->name, rootNote->sharp, octave);
    }

    NotePointer Note::getInstance(Name name, bool sharp, int octave)
    {
        unsigned char value = convert(name, sharp);
        return getInstance(value, name, sharp, octave);
    }

    NotePointer Note::getInstance(unsigned char value)
    {
        int octave = floor(static_cast<float>(value) / 12.0f);
        int localValue = value - 12 * octave;

        const auto& [name, sharp] = convert(localValue);
        return getInstance(value, name, sharp, octave);
    }

    std::string Note::toString() const
    {
        return RootNote::toString() + std::to_string(octave);
    }
}