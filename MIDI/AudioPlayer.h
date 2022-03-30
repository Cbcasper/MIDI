//
// Created by Casper De Smet on 01/03/2022.
//

#ifndef MIDIASSISTANT_AUDIOPLAYER_H
#define MIDIASSISTANT_AUDIOPLAYER_H

#include <AudioToolbox/AudioToolbox.h>
#include <libremidi/message.hpp>

namespace MIDI
{
    class AudioPlayer
    {
    public:
        AUGraph auGraph{};
        AudioUnit audioUnit{};

        AudioPlayer();
        void processMIDIMessage(const libremidi::message& message) const;
    };
}


#endif //MIDIASSISTANT_AUDIOPLAYER_H
