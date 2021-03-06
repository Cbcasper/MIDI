//
// Created by Casper De Smet on 23/03/2022.
//

#ifndef MIDIASSISTANT_NOTEFILTER_H
#define MIDIASSISTANT_NOTEFILTER_H

#include <map>
#include "Note.h"

namespace Music
{
    // Filters all notes outside a range
    class NoteFilter
    {
    public:
        int low;
        int high;
        std::map<int, bool> allowedNotes;

        NoteFilter();
        NoteFilter(int low, int high);

        // Set the range via methods in order to update allowed notes
        void moveLow(int newLow);
        void moveHigh(int newHigh);
        void setRange(int newLow, int newHigh);
        // Method that does the filtering
        bool operator()(const NotePointer& note);
    };
}


#endif //MIDIASSISTANT_NOTEFILTER_H
