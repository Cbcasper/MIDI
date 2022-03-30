//
// Created by Casper De Smet on 02/03/2022.
//

#ifndef MIDIASSISTANT_NOTE_H
#define MIDIASSISTANT_NOTE_H

#include <map>
#include <vector>
#include <mutex>

namespace Music
{
    class Note;
    using NotePointer = std::shared_ptr<Note>;

    class Note
    {
    public:
        enum Name
        {
            C, D, E, F, G, A, B
        };
        using NoteKey = std::tuple<Note::Name, int, bool>;

        Name name;
        int octave;
        bool sharp;

        int value;

        static NotePointer getInstance(unsigned char value, Name instanceName, int instanceOctave, bool instanceSharp);
        static NotePointer getInstance(Name instanceName, int instanceOctave, bool instanceSharp);
        static NotePointer getInstance(unsigned char value);
        static NoteKey getKey(Name instanceName, int instanceOctave, bool instanceSharp);
        std::string toString() const;

        static std::vector<char> noteNames();

    private:
        static std::map<unsigned char, NotePointer> notes;
        static std::mutex mutex;

        Note(Name name, int octave, bool sharp, int value);
    };
}


#endif //MIDIASSISTANT_NOTE_H
