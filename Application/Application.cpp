//
// Created by Casper De Smet on 03/03/2022.
//

#include "Application.h"

namespace State
{
    Application::Application(): modulationHarmony("KOMPLETE KONTROL A25", 1, 0)
    {
        tracks.emplace_back(std::make_shared<Track>(this, "KOMPLETE KONTROL A25", 5));
        song = std::make_shared<Song>(200, 4, Music::Fourth);
        displayMessageFilter = MIDI::MessageFilter();
        displayMessageFilter.allowTypes({MIDI::MessageType::NOTE_ON, MIDI::MessageType::NOTE_OFF});
        recording = false;
    }

    void Application::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        for (const TrackPointer& track: tracks)
            track->incomingMIDIMessage(messageOnInstrument);

        std::unique_lock<std::mutex> lock(mutex);
        messages.emplace_back(messageOnInstrument);
        if (messages.size() > 10000) messages.pop_front();
    }

    void Application::clearMessages()
    {
        std::unique_lock<std::mutex> lock(mutex);
        messages.clear();
    }

    void Application::stopRecording()
    {
        recording = false;
        for (const TrackPointer& track: tracks)
            track->cleanupNotes();
    }
}
