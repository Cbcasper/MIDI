//
// Created by Casper De Smet on 03/03/2022.
//

#ifndef MIDIASSISTANT_APPLICATION_H
#define MIDIASSISTANT_APPLICATION_H

#include <list>
#include <mutex>

#include "Track.h"
#include "../MIDI/Processor.h"
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

        Music::ModulationHarmony modulationHarmony;

        int currentTime = 0;

        Application();

        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void clearMessages();
    };
}


#endif //MIDIASSISTANT_APPLICATION_H
