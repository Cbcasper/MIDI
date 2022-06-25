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

    // Class containing the total length, tempo, signature and key
    class Song
    {
    public:
        int tempo;
        int measures;
        int numberOfBeats;
        Music::TimeDivision timeDivision;
        Music::KeyPointer key;

        int ticksPerDivision = 240;

        Song();

        void setTempo(float newTempo);
        void computeMeasureLength(float& divisionsPerBeat, float& ticksPerBeat, float& ticksPerMeasure);
    };
}


#endif //MIDIASSISTANT_SONG_H
