//
// Created by Casper De Smet on 01/03/2022.
//

#include "AudioPlayer.h"

#include "../Application/Application.h"

namespace MIDI
{
    AudioPlayer::AudioPlayer(const State::ApplicationPointer& application): application(application)
    {
        preset = 0;
        muted = false;
        solo = false;

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

    void AudioPlayer::setPreset(int newPreset)
    {
        preset = newPreset;
        for (int i = 0; i < 16; ++i)
            processMIDIMessage(libremidi::message::program_change(i + 1, preset));
    }

    void AudioPlayer::processMIDIMessage(const libremidi::message& message) const
    {
        if (!muted && (solo || application->soloAudioPlayers.empty()))
            MusicDeviceMIDIEvent(audioUnit, message[0], message[1], message[2], 0);
    }

    void AudioPlayer::processMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument) const
    {
        auto& [message, instrument] = messageOnInstrument;
        switch (instrument->channelSpecificity)
        {
            case Instrument::All:
                processMIDIMessage(message->rawMessage(0));
                break;
            case Instrument::None:
                break;
            case Instrument::Specific:
                processMIDIMessage(message->rawMessage(instrument->channel));
                break;
        }
    }
}