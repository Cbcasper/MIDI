//
// Created by Casper De Smet on 27/02/2022.
//

#include "IOManager.h"

#include <utility>

namespace MIDI
{
    std::mutex IOManager::instanceMutex;
    IOManagerPointer IOManager::instance;

    void IOManager::initialize(const std::shared_ptr<Processor>& givenProcessor)
    {
        processor = givenProcessor;
        // Start initializing as soon as processor is linked
        initializeMidiPorts();
        startObserver();
    }

    // Singleton access
    IOManagerPointer IOManager::getInstance()
    {
        if (!instance)
        {
            std::unique_lock<std::mutex> lock(instanceMutex);
            if (!instance)
                instance = IOManagerPointer(new IOManager());
        }
        return instance;
    }

    IOManager::~IOManager()
    {
        // Clean up observer thread
        observing = false;
        observerCV.notify_one();
        observerThread.join();
    }

    void IOManager::initializeMidiPorts()
    {
        // Open all currently available input and output ports
        for (int portNumber = 0; portNumber < systemMIDIIn.portCount(); ++portNumber)
            constructPort(portNumber, systemMIDIIn.portName(portNumber), IOType::Input);
        for (int portNumber = 0; portNumber < systemMIDIOut.portCount(); ++portNumber)
            constructPort(portNumber, systemMIDIOut.portName(portNumber), IOType::Output);
        processor->updatePorts(keyList(inputPorts), keyList(outputPorts));
//        constructVirtualPort();
    }

    void IOManager::constructPort(int portNumber, const std::string& portName, IOType ioType)
    {
        // Wrap callback method in function object
        auto callback = [&](const libremidi::message& message, const std::string& messagePort)
                            { this->input(message, messagePort); };
        switch (ioType)
        {
            case Input:     inputPorts[portName] = std::make_unique<InputPort>(portNumber, callback);      break;
            case Output:    outputPorts[portName] = std::make_unique<OutputPort>(portNumber);              break;
        }
    }

    void IOManager::constructVirtualPort()
    {
        auto callback = [&](const libremidi::message& message, const std::string& messagePort)
                            { this->input(message, messagePort); };
        inputPorts["MIDI Assistant"] = std::make_unique<InputPort>(callback);
    }

    void IOManager::input(const libremidi::message& message, const std::string& portName)
    {
        processor->processMIDIMessage(message, portName);
    }

    // Thread that periodically check whether the ports have changed
    void IOManager::startObserver()
    {
        observing = true;
        observerThread = std::thread([&]
        {
            std::unique_lock<std::mutex> lock(observerMutex);
            while (observing)
            {
                bool inputsChanged = checkPorts(inputPorts, systemMIDIIn, IOType::Input);
                bool outputsChanged = checkPorts(outputPorts, systemMIDIOut, IOType::Output);
                // Only need to update when something has changed
                if (inputsChanged || outputsChanged)
                    processor->updatePorts(keyList(inputPorts), keyList(outputPorts));
                observerCV.wait_for(lock, std::chrono::seconds(1));
            }
        });
    }

    std::vector<std::string> IOManager::keyList(const MIDI::PortMap& map)
    {
        std::vector<std::string> keys;
        for (const auto& [key, value]: map)
            keys.emplace_back(key);
        return keys;
    }

    std::function<bool(const std::pair<std::string, int>&)> IOManager::equals(const std::string& portName)
    {
        // Function object used in find
        return [&](const std::pair<std::string, int>& port){ return port.first == portName; };;
    }

    // Find new ports and detect removed ports of a specific type
    bool IOManager::checkPorts(PortMap& ports, Port& systemPort, IOType ioType)
    {
        if (systemPort.portCount() == ports.size()) return false;

        // Find list of ports available
        std::map<std::string, int> newPorts;
        for (int portNumber = 0; portNumber < systemPort.portCount(); ++portNumber)
            newPorts[systemPort.portName(portNumber)] = portNumber;

        std::list<std::string> removedPorts;

        // Run through current ports, filtering the available ports to get the new ports and
        // collecting the current ports that are no longer available
        for (const auto& [portName, port]: ports)
            if (std::find_if(newPorts.begin(), newPorts.end(), equals(port->name)) == newPorts.end())
                removedPorts.emplace_back(port->name);
            else
                newPorts.erase(port->name);

        // Update the ports accordingly
        addPorts(newPorts, ioType);
        removePorts(ports, removedPorts);
        return true;
    }

    // Construct some ports and add them
    void IOManager::addPorts(const std::map<std::string, int>& newPorts, IOType ioType)
    {
        for (const auto& [portName, portNumber]: newPorts)
            constructPort(portNumber, portName, ioType);
    }

    // Remove a list of ports
    void IOManager::removePorts(PortMap& ports, const std::list<std::string>& removedPorts)
    {
        for (const std::string& removedPort: removedPorts)
            ports.erase(std::find_if(ports.begin(), ports.end(), [&](const PortMapPair& port){ return port.first == removedPort; }));
    }

    void IOManager::logPorts()
    {
        for (const auto& [portName, port]: inputPorts)
            std::cout << port->name << ", ";
        std::cout << "\n";
        for (const auto& [portName, port]: outputPorts)
            std::cout << port->name << ", ";
        std::cout << "\n";

    }

    // Send a message to an instrument
    void IOManager::sendMIDIOut(const MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        // Find out which ports the message should be sent to
        PortMap usedPorts;
        switch (instrument->portSpecificity)
        {
            case Instrument::All: usedPorts = outputPorts;                                                  break;
            case Instrument::None: usedPorts = PortMap();                                                   break;
            case Instrument::Specific: usedPorts = {{instrument->port, outputPorts[instrument->port]}};     break;
        }

        // For every one of those ports, figure out which channels it should be sent to
        for (const auto& [portName, port]: usedPorts)
        {
            OutputPortPointer outputPort = std::static_pointer_cast<OutputPort>(port);

            switch (instrument->channelSpecificity)
            {
                case Instrument::All:
                    for (int i = 0; i < 16; ++i)
                        outputPort->sendMessage(message->rawMessage());
                    break;
                case Instrument::None:
                    break;
                case Instrument::Specific:
                    outputPort->sendMessage(message->rawMessage());
                    break;
            }
        }
    }
}