//
// Created by Casper De Smet on 27/02/2022.
//

#include "Theory.h"

namespace Music
{
    TheoryPointer Theory::instance;
    std::mutex Theory::instanceMutex;

    Theory::Theory()
    {}

    NotePointer Theory::modulate(const NotePointer& note, int offset)
    {
        return Note::getInstance(note->value + offset);
    }

    TheoryPointer Theory::getInstance()
    {
        if (!instance)
        {
            std::unique_lock<std::mutex> lock(instanceMutex);
            if (!instance)
                instance = TheoryPointer(new Theory());
        }
        return instance;
    }
}