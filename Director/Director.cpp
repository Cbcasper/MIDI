//
// Created by Casper De Smet on 27/02/2022.
//

#include <iostream>

#include "Director.h"

namespace Music
{
    Director::Director(const std::shared_ptr<State::Application>& applicationState): applicationState(applicationState)
    {
        running = true;
        // Only react to note messages
        messageFilter.disallowAllTypes();
        messageFilter.allowTypes({MIDI::MessageType::NOTE_ON, MIDI::MessageType::NOTE_OFF});
        directorThread = std::thread([&]{ this->messageHandler(); });
    }

    Director::~Director()
    {
        running = false;
        messageCV.notify_one();
        directorThread.join();
    }

    void Director::messageHandler()
    {
        // Block thread until new message arrives
        std::unique_lock<std::mutex> lock(messageMutex);
        messageCV.wait(lock, [&]{ return !running || !messageQueue.empty(); });
        while (running)
        {
            // Process one message at a time
            const MIDI::MessageOnInstrument messageOnInstrument = messageQueue.front();
            messageQueue.pop();
            processMessage(messageOnInstrument);

            messageCV.wait(lock, [&]{ return !running || !messageQueue.empty(); });
        }
    }

    void Director::processMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        // Pass the message to all harmonies of all tracks
        const auto& [message, instrument] = messageOnInstrument;
        for (const State::TrackPointer& track: applicationState->tracks)
            if (*instrument == *track->input)
                for (const HarmonyPointer& harmony: track->harmonies)
                    harmony->processMessage(std::static_pointer_cast<MIDI::NoteMessage>(message));
    }

    void Director::inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument)
    {
        // Save the message and wake up the processing thread
        if (!messageFilter(messageOnInstrument.first)) return;

        messageMutex.lock();
        messageQueue.push(messageOnInstrument);
        messageMutex.unlock();

        messageCV.notify_one();
    }
}
