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
    // Click on regular time points to indicate the tempo
    class Metronome
    {
    public:
        MIDI::AudioPlayerPointer audioPlayer;
        State::ApplicationPointer application;

        bool clicking;

        // Messages to play
        MIDI::NoteOnPointer measureSoundOn;
        MIDI::NoteOffPointer measureSoundOff;
        MIDI::NoteOnPointer beatSoundOn;
        MIDI::NoteOffPointer beatSoundOff;

        // Countdowns for note off messages
        int measureTimer;
        int beatTimer;

        explicit Metronome(const State::ApplicationPointer& application);

        // Method that checks whether click needs to happen and performs the click
        void clickOnTick(int tick);
    };
    using MetronomePointer = std::shared_ptr<Metronome>;
}

#endif //MIDIASSISTANT_METRONOME_H
