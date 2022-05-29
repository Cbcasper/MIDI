//
// Created by Casper De Smet on 28/05/2022.
//

#include "ChordNote.h"

namespace Music
{
    ChordNote::ChordNote(const ChordPointer& chord, IntervalSequence::Modulation modulation, int octave):
                         chord(chord), modulation(modulation), octave(octave)
    {}

    void ChordNote::operator++()
    {
        modulation = getNextModulation();
        octave += (int) modulation == chord->rollOverModulation;
    }

    void ChordNote::operator--()
    {
        octave -= (int) modulation == chord->rollOverModulation;
        modulation = getPreviousModulation();
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

    IntervalSequence::Modulation ChordNote::getNextModulation()
    {
        switch (modulation)
        {
            default:                      return IntervalSequence::First;
            case IntervalSequence::First: return IntervalSequence::Third;
            case IntervalSequence::Third: return IntervalSequence::Fifth;
            case IntervalSequence::Fifth: return IntervalSequence::First;
        }
    }

    IntervalSequence::Modulation ChordNote::getPreviousModulation()
    {
        switch (modulation)
        {
            case IntervalSequence::First: return IntervalSequence::Fifth;
            case IntervalSequence::Third: return IntervalSequence::First;
            case IntervalSequence::Fifth: return IntervalSequence::Third;
            default:                      return IntervalSequence::First;
        }
    }

    NotePointer ChordNote::getNote()
    {
        RootNotePointer note = (*chord)(modulation);
        return Note::getInstance(octave * 12 + RootNote::convert(note->name, note->sharp));
    }
}