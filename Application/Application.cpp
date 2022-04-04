//
// Created by Casper De Smet on 03/03/2022.
//

#include "Application.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../MIDI/Messages/Messages/NoteOff.h"

namespace State
{
    Application::Application(): modulationHarmony("KOMPLETE KONTROL A25", 1, 0)
    {
        tracks.emplace_back(std::make_shared<Track>(this, "KOMPLETE KONTROL A25", 5));
        tracks[0]->midiMessages[0].emplace_back(std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(64), 64));
        tracks[0]->midiMessages[960].emplace_back(std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(64), 64));
        tracks[0]->midiMessages[960].emplace_back(std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(74), 64));
        tracks[0]->midiMessages[1920].emplace_back(std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(74), 64));
        tracks[0]->midiMessages[1920].emplace_back(std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(54), 64));
        tracks[0]->midiMessages[2880].emplace_back(std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(54), 64));
        tracks[0]->midiMessages[2880].emplace_back(std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(64), 64));
        tracks[0]->midiMessages[3840].emplace_back(std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(64), 64));
        song = std::make_shared<Song>(200, 4, Music::Fourth);
        displayMessageFilter = MIDI::MessageFilter();
        displayMessageFilter.allowTypes({MIDI::MessageType::NOTE_ON, MIDI::MessageType::NOTE_OFF});
    }

    void Application::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        std::unique_lock<std::mutex> lock(mutex);
        messages.emplace_back(messageOnInstrument);
        if (messages.size() > 10000) messages.pop_front();

        for (const TrackPointer& track: tracks)
            track->incomingMIDIMessage(messageOnInstrument);
    }

    void Application::clearMessages()
    {
        std::unique_lock<std::mutex> lock(mutex);
        messages.clear();
    }
}
