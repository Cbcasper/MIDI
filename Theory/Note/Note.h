//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_NOTE_H
#define MIDIASSISTANT_NOTE_H

#include <map>
#include <vector>
#include <mutex>

#include "RootNote.h"

namespace Music
{
    class Note;
    using NotePointer = std::shared_ptr<Note>;

    // Class that represents a concrete note in a specific octave
    // It is a multiton, meaning that for every note value, there is only one instance
    class Note: public RootNote
    {
    public:
        int octave;
        int value;

        // Multiton logic, multiple ways of getting a note
        static NotePointer getInstance(unsigned char value, Name name, bool sharp, int octave);
        static NotePointer getInstance(const RootNotePointer& rootNote, int octave);
        static NotePointer getInstance(Name name, bool sharp, int octave);
        static NotePointer getInstance(unsigned char value);
        std::string toString() const override;

    private:
        static std::map<unsigned char, NotePointer> notes;
        static std::mutex mutex;

        Note(Name name, bool sharp, int octave, int value);
    };
}


#endif //MIDIASSISTANT_NOTE_H
