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
    class Application
    {
    public:
        std::mutex mutex;

        std::vector<std::string> inputPorts;
        std::vector<std::string> outputPorts;

        std::list<MIDI::MessageOnInstrument> messages;
        std::vector<TrackPointer> tracks;

        SongPointer song;

        int currentTime;
        bool recording;

        int ticksPerDivision = 240;

        MIDI::MessageFilter displayMessageFilter;

        Application();

        void inputMIDIMessage(const MIDI::MessageOnInstrument& messageOnInstrument);
        void clearMessages();

        void stopRecording();

        MIDI::InstrumentPointer selectInstrument(MIDI::IOType ioType);
        void initializeTracks();
    };

    using ApplicationPointer = std::shared_ptr<Application>;
}


#endif //MIDIASSISTANT_APPLICATION_H
