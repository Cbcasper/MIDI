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
#include "../Director/Harmonies/ModulationHarmony.h"

namespace State
{
    class Application
    {
    public:
        std::mutex mutex;

        std::vector<std::string> inputPorts;
        std::vector<std::string> outputPorts;

        std::list<MIDI::MessageOnInstrument> messages;
        std::vector<TrackPointer> tracks;

        MIDI::MessageFilter displayMessageFilter;

        Music::ModulationHarmony modulationHarmony;

        SongPointer song;

        bool recording;

        int currentTime = -1;
        int ticksPerDivision = 240;

        Application();

        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void clearMessages();

        void stopRecording();
    };
}


#endif //MIDIASSISTANT_APPLICATION_H
