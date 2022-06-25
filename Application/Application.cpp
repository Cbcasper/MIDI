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

        recording = false;
        currentTime = -1;
    }

    void Application::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        // Pass the incoming messages to the tracks
        for (const TrackPointer& track: tracks)
            track->incomingMIDIMessage(messageOnInstrument);

        // Store the up to 10000 last incoming messages
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

    TrackPointer Application::deleteTrack(const TrackPointer& track)
    {
        // Only delete a track if enough tracks exists and select a nearby track
        if (tracks.size() > 1)
        {
            auto position = std::find(tracks.begin(), tracks.end(), track);
            auto nextPosition = tracks.erase(position);
            if (nextPosition == tracks.end()) return *--nextPosition;
            else                              return *nextPosition;
        }
        return track;
    }
}
