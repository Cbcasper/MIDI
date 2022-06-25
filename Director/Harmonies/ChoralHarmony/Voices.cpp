//
// Created by Casper De Smet on 30/05/2022.
//

#include "Voices.h"

namespace Music
{
    Voices::Voices(const KeyPointer& key, const ChordPointer& chord):  key(key), chord(chord), discard(nullptr)
    {}

    // Get the voice that sings a given note
    Voice Voices::voice(const NotePointer& note)
    {
        if (sopranoNote == note)     return Soprano;
        else if (altoNote == note)   return Alto;
        else if (tenorNote == note)  return Tenor;
        return Invalid;
    }

    // Get the note a voice sings
    NotePointer& Voices::operator[](Voice voice)
    {
        switch (voice)
        {
            case Soprano: return sopranoNote;
            case Alto:    return altoNote;
            case Tenor:   return tenorNote;
            case Bass:
            case Invalid: return discard;
        }
    }

    // Get the note that is of the degree in the key
    NotePointer& Voices::operator[](Key::Degree degree)
    {
        if (degree == key->getDegree(sopranoNote))     return sopranoNote;
        else if (degree == key->getDegree(altoNote))   return altoNote;
        else if (degree == key->getDegree(tenorNote))  return tenorNote;
        return discard;
    }
}