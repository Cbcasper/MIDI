//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIASSISTANT_IOMANAGER_H
#define MIDIASSISTANT_IOMANAGER_H

#include <thread>
#include <set>

#include "libremidi/libremidi.hpp"

#include "IOType.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "../Processor.h"
#include "../AudioPlayer.h"
#include "../Utilities/Utilities.h"
#include "../../Application/Application.h"

namespace MIDI
{
    class IOManager;
    using PortMap = std::map<std::string, PortPointer>;
    using PortMapPair = std::pair<std::string, PortPointer>;
    using OutputPortMap = std::map<std::string, OutputPortPointer>;
    using IOManagerPointer = std::shared_ptr<IOManager>;

    // Class that listens to MIDI input and produces MIDI output
    class IOManager
    {
    public:
        std::shared_ptr<Processor> processor;

        // Ports
        PortMap inputPorts;
        PortMap outputPorts;
        // Ports used to get port counts and port names
        InputPort systemMIDIIn{};
        OutputPort systemMIDIOut{};

        // Observe thread management
        bool observing = false;
        std::thread observerThread;

        std::mutex observerMutex;
        std::condition_variable observerCV;

        // Link with processor, because this is a singleton
        void initialize(const std::shared_ptr<Processor>& givenProcessor);
        static IOManagerPointer getInstance();

        // Setup
        void initializeMidiPorts();
        void constructPort(int portNumber, const std::string& portName, IOType ioType);
        void constructVirtualPort();
        void startObserver();

        // Observer functions
        static std::function<bool(const std::pair<std::string, int>&)> equals(const std::string& portName);
        bool checkPorts(PortMap& ports, Port& systemPort, IOType ioType);
        void addPorts(const std::map<std::string, int>& newPorts, IOType ioType);
        void removePorts(PortMap& ports, const std::list<std::string>& removedPorts);
        static std::vector<std::string> keyList(const MIDI::PortMap& map);

        // Callback method
        void input(const libremidi::message& message, const std::string& portName);

        void logPorts();

        void sendMIDIOut(const MessageOnInstrument& messageOnInstrument);

        ~IOManager();

    private:
        // Singleton logic
        IOManager() = default;

        static std::mutex instanceMutex;
        static IOManagerPointer instance;
    };
}

#endif //MIDIASSISTANT_IOMANAGER_H
