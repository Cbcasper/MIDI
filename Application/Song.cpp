//
// Created by Casper De Smet on 30/03/2022.
//

#include "Song.h"

namespace State
{
    Song::Song(int measures, int numberOfBeats, Music::TimeDivision timeDivision):
               measures(measures), numberOfBeats(numberOfBeats), timeDivision(timeDivision)
    {}
}