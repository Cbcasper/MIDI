//
// Created by Casper De Smet on 03/03/2022.
//

#ifndef MIDIASSISTANT_APPLICATION_H
#define MIDIASSISTANT_APPLICATION_H

#include <list>
#include <mutex>

#include "Song.h"
#include "Track.h"
#include "../MIDI/Processor.h"
#include "../MIDI/MessageFilter.h"
#include "../MIDI/Messages/Message.h"
#include "../MIDI/Messages/Instrument.h"
#include "../Director/Harmonies/SingleHarmonies/TranspositionHarmony.h"
#include "../Theory/Key/Key.h"
#include "../MIDI/IO/IOType.h"

namespace State
{
    // Class containing all state off the application needed by the different components
    class Application
    {
    public:
        // Guard the list of incoming messages
        std::mutex mutex;
        std::list<MIDI::MessageOnInstrument> messages;

        // Available input and output ports
        std::vector<std::string> inputPorts;
        std::vector<std::string> outputPorts;

        std::list<TrackPointer> tracks;
        std::set<MIDI::AudioPlayerPointer> soloAudioPlayers;

        // Song data: tempo, signature and key
        SongPointer song;

        int currentTime;
        bool recording;

        Application();

        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void clearMessages();
        void stopRecording();

        // Select a default available instrument or return an instrument accepting all inputs
        MIDI::InstrumentPointer selectInstrument(MIDI::IOType ioType);

        TrackPointer deleteTrack(const TrackPointer& track);
    };

    using ApplicationPointer = std::shared_ptr<Application>;
}


#endif //MIDIASSISTANT_APPLICATION_H
