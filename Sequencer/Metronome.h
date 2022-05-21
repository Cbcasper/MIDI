//
// Created by Casper De Smet on 03/05/2022.
//

#ifndef MIDIASSISTANT_METRONOME_H
#define MIDIASSISTANT_METRONOME_H

#include "../MIDI/AudioPlayer.h"
#include "../Application/Song.h"
#include "../MIDI/Messages/Messages/NoteOff.h"
#include "../MIDI/Messages/Messages/NoteOn.h"

namespace System
{
    class Metronome
    {
    public:
        MIDI::AudioPlayer audioPlayer;
        State::SongPointer song;

        bool clicking;

        MIDI::NoteOnPointer measureSoundOn;
        MIDI::NoteOffPointer measureSoundOff;
        MIDI::NoteOnPointer beatSoundOn;
        MIDI::NoteOffPointer beatSoundOff;

        int measureTimer;
        int beatTimer;

        explicit Metronome(const State::SongPointer& song);

        void clickOnTick(int tick);
    };
    using MetronomePointer = std::shared_ptr<Metronome>;
}

#endif //MIDIASSISTANT_METRONOME_H
