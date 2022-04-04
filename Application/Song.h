//
// Created by Casper De Smet on 30/03/2022.
//

#ifndef MIDIASSISTANT_SONG_H
#define MIDIASSISTANT_SONG_H

#include <memory>

#include "../Theory/TimeDivision.h"

namespace State
{
    class Song;
    using SongPointer = std::shared_ptr<Song>;

    class Song
    {
    public:
        int measures;

        int numberOfBeats;
        Music::TimeDivision timeDivision;

        Song(int measures, int numberOfBeats, Music::TimeDivision timeDivision);
    };
}


#endif //MIDIASSISTANT_SONG_H
