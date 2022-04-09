//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIASSISTANT_THEORY_H
#define MIDIASSISTANT_THEORY_H

#include <memory>

#include "Note/Note.h"

namespace Music
{
    class Theory;
    using TheoryPointer = std::shared_ptr<Theory>;

    class Theory
    {
    public:
        NotePointer modulate(const NotePointer& note, int offset);

        static TheoryPointer getInstance();

    private:
        Theory();

        static TheoryPointer instance;
        static std::mutex instanceMutex;
    };
}


#endif //MIDIASSISTANT_THEORY_H
