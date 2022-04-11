//
// Created by Casper De Smet on 09/04/2022.
//

#ifndef MIDIASSISTANT_ROOTNOTE_H
#define MIDIASSISTANT_ROOTNOTE_H

#include <utility>
#include <tuple>
#include <memory>
#include <vector>

namespace Music
{
    class RootNote;
    using RootNotePointer = std::shared_ptr<RootNote>;

    class RootNote
    {
    public:
        enum Name
        {
            C, D, E, F, G, A, B
        };

        Name name{};
        bool sharp{};

        RootNote(Name name, bool sharp);
        virtual ~RootNote() = default;

        static std::pair<Name, bool> correct(Name name, bool sharp);
        static std::pair<Name, bool> convert(int value);
        static int convert(Name name, bool sharp);

        RootNotePointer operator+(int semitones) const;
        bool operator==(const RootNote& rootNote);

        virtual std::string toString() const;
        static std::vector<char> noteNames();
    };
}


#endif //MIDIASSISTANT_ROOTNOTE_H
