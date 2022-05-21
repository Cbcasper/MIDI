//
// Created by Casper De Smet on 03/05/2022.
//

#include "Metronome.h"

namespace System
{
    Metronome::Metronome(const State::SongPointer& song): song(song)
    {
        audioPlayer = MIDI::AudioPlayer();
        clicking = false;
        measureSoundOn = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(76), 127);
        measureSoundOff = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(76), 127);
        beatSoundOn = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(77), 127);
        beatSoundOff = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(77), 127);
    }

    void Metronome::clickOnTick(int tick)
    {
        if (clicking)
        {
            float divisionsPerBeat;
            float ticksPerBeat;
            float ticksPerMeasure;
            song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, ticksPerMeasure);

            if (--measureTimer == 0)
                audioPlayer.processMIDIMessage(measureSoundOff->rawMessage(10));
            if (--beatTimer == 0)
                audioPlayer.processMIDIMessage(beatSoundOff->rawMessage(10));

            if (tick % static_cast<int>(ticksPerMeasure) == 0)
            {
                audioPlayer.processMIDIMessage(measureSoundOff->rawMessage(10));
                audioPlayer.processMIDIMessage(measureSoundOn->rawMessage(10));
                measureTimer = song->ticksPerDivision;
            }
            else if (tick % static_cast<int>(ticksPerBeat) == 0)
            {
                audioPlayer.processMIDIMessage(beatSoundOff->rawMessage(10));
                audioPlayer.processMIDIMessage(beatSoundOn->rawMessage(10));
                beatTimer = song->ticksPerDivision;
            }
        }
    }
}