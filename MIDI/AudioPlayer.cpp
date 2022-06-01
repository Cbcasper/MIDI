//
// Created by Casper De Smet on 01/03/2022.
//

#include "AudioPlayer.h"

#include <iostream>

namespace MIDI
{
    AudioPlayer::AudioPlayer()
    {
        NewAUGraph(&auGraph);
        AudioComponentDescription outputDescription;
        outputDescription.componentType = kAudioUnitType_Output;
        outputDescription.componentSubType = kAudioUnitSubType_DefaultOutput;
        outputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

        AUNode outputNode;
        AUGraphAddNode(auGraph, &outputDescription, &outputNode);

        AudioComponentDescription instrumentDescription;
        instrumentDescription.componentType = kAudioUnitType_MusicDevice;
        instrumentDescription.componentSubType = kAudioUnitSubType_DLSSynth;
        instrumentDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

        AUNode instrumentNode;
        AUGraphAddNode(auGraph, &instrumentDescription, &instrumentNode);

        AUGraphOpen(auGraph);
        AUGraphNodeInfo(auGraph, instrumentNode, nullptr, &audioUnit);
        AUGraphConnectNodeInput(auGraph, instrumentNode, 0, outputNode, 0);
        AUGraphInitialize(auGraph);
        AUGraphStart(auGraph);

        for (int i = 0; i < 16; ++i)
            processMIDIMessage(libremidi::message::control_change(i + 1, 91, 10));
    }

    void AudioPlayer::processMIDIMessage(const libremidi::message& message) const
    {
        MusicDeviceMIDIEvent(audioUnit, message[0], message[1], message[2], 0);
    }
}