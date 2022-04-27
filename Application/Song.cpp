//
// Created by Casper De Smet on 30/03/2022.
//

#include "Song.h"
#include "../Sequencer/Timer.h"

namespace State
{
    Song::Song(): measures(200), numberOfBeats(4), timeDivision(Music::Fourth), tempo(500000)
    {
        key = std::make_shared<Music::Key>();
    }

    void Song::setTempo(int microseconds)
    {
        tempo = microseconds;
        System::Timer::getInstance()->setTempo(tempo);
    }
}