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

    class Note: public RootNote
    {
    public:
        int octave;
        int value;

        static NotePointer getInstance(unsigned char value, Name instanceName, int instanceOctave, bool instanceSharp);
        static NotePointer getInstance(Name instanceName, int instanceOctave, bool instanceSharp);
        static NotePointer getInstance(unsigned char value);
        std::string toString() const override;

    private:
        static std::map<unsigned char, NotePointer> notes;
        static std::mutex mutex;

        Note(Name name, int octave, bool sharp, int value);
    };
}


#endif //MIDIASSISTANT_NOTE_H
