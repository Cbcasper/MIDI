//
// Created by Casper De Smet on 30/03/2022.
//

#ifndef MIDIASSISTANT_SONG_H
#define MIDIASSISTANT_SONG_H

#include <memory>

#include "../Theory/TimeDivision.h"
#include "../Theory/Key/Key.h"

namespace State
{
    class Song;
    using SongPointer = std::shared_ptr<Song>;

    class Song
    {
    public:
        int tempo;
        int measures;
        int numberOfBeats;
        Music::TimeDivision timeDivision;
        Music::KeyPointer key;

        Song();

        void setTempo(int microseconds);
    };
}


#endif //MIDIASSISTANT_SONG_H
