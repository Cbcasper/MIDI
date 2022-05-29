//
// Created by Casper De Smet on 03/03/2022.
//

#include "Application.h"

#include <iostream>

namespace State
{
    Application::Application()
    {
        song = std::make_shared<Song>();

        displayMessageFilter = MIDI::MessageFilter();
        displayMessageFilter.allowTypes({MIDI::MessageType::NOTE_ON, MIDI::MessageType::NOTE_OFF});

        recording = false;
        currentTime = -1;
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
            track->stopRecording();
    }

    MIDI::InstrumentPointer Application::selectInstrument(MIDI::IOType ioType)
    {
        std::vector<std::string> ports = ioType == MIDI::Input ? inputPorts : outputPorts;
        if (ports.empty())
            return std::make_shared<MIDI::Instrument>();
        else
            return std::make_shared<MIDI::Instrument>(ports[0], 1);
    }
}
