//
// Created by Casper De Smet on 30/03/2022.
//

#include "Song.h"
#include "../Sequencer/Timer.h"

namespace State
{
    // Default 200 measures, 4/4 and 120 bpm
    Song::Song(): measures(200), numberOfBeats(4), timeDivision(Music::Fourth), tempo(500000)
    {
        key = std::make_shared<Music::Key>();
    }

    // Convert bpm to microseconds per tick
    void Song::setTempo(float newTempo)
    {
        tempo = static_cast<int>(round(60.f * 1000.f * 1000.f / newTempo));;
        System::Timer::getInstance()->setTempo(tempo);
    }

    // Compute some values relevant to the tempo
    void Song::computeMeasureLength(float& divisionsPerBeat, float& ticksPerBeat, float& ticksPerMeasure)
    {
        divisionsPerBeat = static_cast<float>(Music::Sixteenth) / static_cast<float>(timeDivision);
        ticksPerBeat = divisionsPerBeat * static_cast<float>(ticksPerDivision);
        ticksPerMeasure = ticksPerBeat * static_cast<float>(numberOfBeats);
    }
}