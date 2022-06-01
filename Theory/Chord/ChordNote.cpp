//
// Created by Casper De Smet on 28/05/2022.
//

#include "ChordNote.h"

namespace Music
{
    ChordNote::ChordNote(const ChordPointer& chord, Key::Degree degree, int octave):
            chord(chord), degree(degree), octave(octave)
    {}

    void ChordNote::operator++()
    {
        degree = getNextModulation();
        octave += (int) degree == chord->rollOverDegree;
    }

    void ChordNote::operator--()
    {
        octave -= (int) degree == chord->rollOverDegree;
        degree = getPreviousModulation();
    }

    void ChordNote::operator+(int offset)
    {
        for (int i = 0; i < offset; ++i)
            ++*this;
    }

    void ChordNote::operator-(int offset)
    {
        for (int i = 0; i < offset; ++i)
            --*this;
    }

    void ChordNote::operator+=(int offset)
    {
        for (int i = 0; i < offset; ++i)
            ++*this;
    }

    void ChordNote::operator-=(int offset)
    {
        for (int i = 0; i < offset; ++i)
            --*this;
    }

    Key::Degree ChordNote::getNextModulation()
    {
        switch (degree)
        {
            default:         return Key::First;
            case Key::First: return Key::Third;
            case Key::Third: return Key::Fifth;
            case Key::Fifth: return Key::First;
        }
    }

    Key::Degree ChordNote::getPreviousModulation()
    {
        switch (degree)
        {
            case Key::First: return Key::Fifth;
            case Key::Third: return Key::First;
            case Key::Fifth: return Key::Third;
            default:         return Key::First;
        }
    }

    NotePointer ChordNote::getNote()
    {
        RootNotePointer note = (*chord)(degree);
        return Note::getInstance(octave * 12 + RootNote::convert(note->name, note->sharp));
    }
}