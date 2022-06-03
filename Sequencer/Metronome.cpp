//
// Created by Casper De Smet on 03/05/2022.
//

#include "Metronome.h"

#include "../Application/Application.h"

namespace System
{
    Metronome::Metronome(const State::ApplicationPointer& application): application(application)
    {
        audioPlayer = std::make_shared<MIDI::AudioPlayer>(application);
        clicking = false;

        measureTimer = 0;
        beatTimer = 0;

        measureSoundOn = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(76), 100);
        measureSoundOff = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(76), 100);
        beatSoundOn = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(77), 100);
        beatSoundOff = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(77), 100);
    }

    void Metronome::clickOnTick(int tick)
    {
        if (clicking)
        {
            float divisionsPerBeat;
            float ticksPerBeat;
            float ticksPerMeasure;
            application->song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, ticksPerMeasure);

            if (--measureTimer == 0)
                audioPlayer->processMIDIMessage(measureSoundOff->rawMessage(10));
            if (--beatTimer == 0)
                audioPlayer->processMIDIMessage(beatSoundOff->rawMessage(10));

            if (tick % static_cast<int>(ticksPerMeasure) == 0)
            {
                audioPlayer->processMIDIMessage(measureSoundOff->rawMessage(10));
                audioPlayer->processMIDIMessage(measureSoundOn->rawMessage(10));
                measureTimer = application->song->ticksPerDivision;
            }
            else if (tick % static_cast<int>(ticksPerBeat) == 0)
            {
                audioPlayer->processMIDIMessage(beatSoundOff->rawMessage(10));
                audioPlayer->processMIDIMessage(beatSoundOn->rawMessage(10));
                beatTimer = application->song->ticksPerDivision;
            }
        }
    }
}