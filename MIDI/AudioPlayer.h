//
// Created by Casper De Smet on 01/03/2022.
//

#ifndef MIDIASSISTANT_AUDIOPLAYER_H
#define MIDIASSISTANT_AUDIOPLAYER_H

#include <AudioToolbox/AudioToolbox.h>
#include <libremidi/message.hpp>

#include "Processor.h"

namespace State
{
    class Application;
    using ApplicationPointer = std::shared_ptr<Application>;
}

namespace MIDI
{
    class AudioPlayer
    {
    public:
        State::ApplicationPointer application;

        // State
        int preset;
        bool muted;
        bool solo;

        // Core audio specifics
        AUGraph auGraph{};
        AudioUnit audioUnit{};

        AudioPlayer(const State::ApplicationPointer& application);
        void setPreset(int newPreset);
        void processMIDIMessage(const libremidi::message& message) const;
        void processMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument) const;
    };
    using AudioPlayerPointer = std::shared_ptr<AudioPlayer>;
}


#endif //MIDIASSISTANT_AUDIOPLAYER_H
