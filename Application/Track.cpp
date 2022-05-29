//
// Created by Casper De Smet on 23/03/2022.
//

#include <iostream>

#include "Track.h"

#include "./Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"

namespace State
{
    Track::Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output):
                 application(application), input(input), output(output), height(150)
    {
        audioPlayer = MIDI::AudioPlayer();

        mainTake = std::make_shared<Take>(application);
        recordingTake = mainTake;
    }

    void Track::incomingMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        if (*instrument == *input)
        {
            playMIDIMessage(messageOnInstrument);
            if (application->recording)
                recordingTake->record(messageOnInstrument, application->currentTime);
        }
    }

    void Track::playMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        audioPlayer.processMIDIMessage(message->rawMessage(instrument->channel));
        updateSoundingNotes(message);
    }

    void Track::updateSoundingNotes(const MIDI::MessagePointer& message)
    {
        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
            soundingNotes[noteOn->note->value]++;
        else if (MIDI::NoteOffPointer noteOff = std::dynamic_pointer_cast<MIDI::NoteOff>(message))
            soundingNotes[noteOff->note->value]--;
    }

    void Track::stopRecording()
    {
        recordingTake->cleanupNotes(application->currentTime);
        soundingNotes = std::map<int, int>();
    }

    bool Track::hasSelectedHarmonies()
    {
        for (const Music::HarmonyPointer& harmony: harmonies)
            if (harmony->selected)
                return true;
        return false;
    }

    void Track::clearSelectedHarmonies()
    {
        for (const Music::HarmonyPointer& harmony: harmonies)
            harmony->selected = false;
    }

    void Track::addTake()
    {
        TakePointer take = std::make_shared<Take>(application);
        takes.emplace_back(take);
        recordingTake = take;
    }

    bool Track::equalTakes(std::vector<NoteSequences>& takeNoteSequences)
    {
        for (int i = 0; i < takes.size() - 1; ++i)
            for (int j = 0; j < 128; ++j)
                if (takeNoteSequences[i][j].size() != takeNoteSequences[i + 1][j].size())
                    return false;
        return true;
    }

    void Track::quantize()
    {
        std::vector<NoteSequences> takeNoteSequences;
        for (const TakePointer& take: takes)
            takeNoteSequences.push_back(take->noteSequences());
        if (equalTakes(takeNoteSequences))
            std::cout << "Quantizable\n";
        else
            std::cout << "Not quantizable\n";
    }
}