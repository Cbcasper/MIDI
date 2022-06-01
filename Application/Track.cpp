//
// Created by Casper De Smet on 23/03/2022.
//

#include <iostream>

#include "Track.h"

#include "./Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../Utilities/Average.h"

namespace State
{
    Track::Track(const ApplicationPointer& application, const MIDI::InstrumentPointer& input, const MIDI::InstrumentPointer& output):
                 application(application), input(input), output(output), height(150)
    {
        audioPlayer = MIDI::AudioPlayer();

        TakePointer initialTake = std::make_shared<Take>(application);
        recordingTake = initialTake;
        takes.emplace_back(initialTake);

        float divisionsPerBeat;
        float ticksPerBeat;
        float ticksPerMeasure;
        application->song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, ticksPerMeasure);
        timeWindow = static_cast<int>(round(ticksPerMeasure));
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
        audioPlayer.processMIDIMessage(message->rawMessage());
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

    void Track::deleteTake()
    {
        recordingTake = *--takes.erase(std::find(takes.begin(), takes.end(), recordingTake));
    }

    bool Track::equalTakes(std::vector<NoteSequences>& takeNoteSequences)
    {
        if (takes.size() < 2) return true;
        for (int i = 0; i < takes.size() - 1; ++i)
            for (int j = 0; j < 128; ++j)
                if (takeNoteSequences[i][j].size() != takeNoteSequences[i + 1][j].size())
                    return false;
        return true;
    }

    void Track::orderedQuantize()
    {
        int takeIndex = 0;
        std::vector<NoteSequences> takeNoteSequences = std::vector<NoteSequences>(takes.size());
        for (const TakePointer& take: takes)
            take->getNoteSequences(takeNoteSequences[takeIndex++]);
        if (equalTakes(takeNoteSequences))
        {
            TakePointer newTake = std::make_shared<Take>(application);
            for (int i = 0; i < 128; ++i)
            {
                std::vector<MIDI::ChronologicNotes> chronologicNotes;
                for (const NoteSequences& takeNoteSequence: takeNoteSequences)
                    chronologicNotes.emplace_back(takeNoteSequence[i]);
                for (const MIDI::ChronologicNotes& notes: ChronologicNotesZip(chronologicNotes))
                    average(newTake, notes, i);
            }
            takes.emplace_back(newTake);
        }
    }

    void Track::timeWindowQuantize()
    {
        TakePointer newTake = std::make_shared<Take>(application);
        int lastMessage = getLastMessage();
        for (int i = 0; i < lastMessage + timeWindow; i += timeWindow)
        {
            NoteSequences noteSequences;
            for (int j = i; j < i + timeWindow; ++j)
                for (const TakePointer& take: takes)
                {
                    for (const MIDI::MessagePointer& message: take->midiMessages[j])
                        if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
                            noteSequences[noteOn->note->value].insert(noteOn);
                    if (take->midiMessages[i].empty())
                        take->midiMessages.erase(i);
                }
            int noteValue = 0;
            for (const MIDI::ChronologicNotes& chronologicNotes: noteSequences)
            {
                if (!chronologicNotes.empty())
                    average(newTake, chronologicNotes, noteValue);
                ++noteValue;
            }
        }
        takes.emplace_back(newTake);
    }

    int Track::getLastMessage()
    {
        std::vector<int> lastMessages;
        for (const TakePointer& take: takes)
            lastMessages.emplace_back(take->midiMessages.rbegin()->first);
        return *std::max_element(lastMessages.begin(), lastMessages.end());
    }

    void Track::average(const TakePointer& take, const MIDI::ChronologicNotes& notes, int noteValue)
    {
        Utilities::Average start;
        Utilities::Average end;
        Utilities::Average onVelocity;
        Utilities::Average offVelocity;
        for (const MIDI::NoteOnPointer& noteOn: notes)
        {
            start += noteOn->tick;
            end += noteOn->noteEnd->tick;
            onVelocity += noteOn->velocity;
            offVelocity += noteOn->noteEnd->velocity;
        }
        Music::NotePointer note = Music::Note::getInstance(noteValue);
        MIDI::NoteOnPointer noteOn = std::make_shared<MIDI::NoteOn>(note, onVelocity.get());
        MIDI::NoteOffPointer noteOff = std::make_shared<MIDI::NoteOff>(note, offVelocity.get());
        take->record(std::make_pair(noteOn, input), start.get());
        take->record(std::make_pair(noteOff, input), end.get());
    }
}