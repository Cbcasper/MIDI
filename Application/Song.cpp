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

    void Song::computeMeasureLength(float& divisionsPerBeat, float& ticksPerBeat, float& ticksPerMeasure)
    {
        divisionsPerBeat = static_cast<float>(Music::Sixteenth) / static_cast<float>(timeDivision);
        ticksPerBeat = divisionsPerBeat * static_cast<float>(ticksPerDivision);
        ticksPerMeasure = ticksPerBeat * static_cast<float>(numberOfBeats);
    }
}