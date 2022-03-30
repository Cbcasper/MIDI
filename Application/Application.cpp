//
// Created by Casper De Smet on 03/03/2022.
//

#include "Application.h"

namespace State
{
    Application::Application(): modulationHarmony("KOMPLETE KONTROL A25", 1, 0)
    {
        tracks.emplace_back(std::make_shared<Track>("KOMPLETE KONTROL A25", 1));
    }

    void Application::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        std::unique_lock<std::mutex> lock(mutex);
        messages.emplace_back(messageOnInstrument);
        if (messages.size() > 10000) messages.pop_front();

        for (const TrackPointer& track: tracks)
            track->inputMIDIMessage(messageOnInstrument);
    }

    void Application::clearMessages()
    {
        std::unique_lock<std::mutex> lock(mutex);
        messages.clear();
    }
}
