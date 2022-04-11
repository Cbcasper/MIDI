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
        initializeMidiPorts();
        startObserver();
    }

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
        observing = false;
        observerCV.notify_one();
        observerThread.join();
    }

    void IOManager::initializeMidiPorts()
    {
        for (int portNumber = 0; portNumber < systemMIDIIn.portCount(); ++portNumber)
            constructPort(portNumber, systemMIDIIn.portName(portNumber), IOType::Input);
        for (int portNumber = 0; portNumber < systemMIDIOut.portCount(); ++portNumber)
            constructPort(portNumber, systemMIDIOut.portName(portNumber), IOType::Output);
        processor->initializePorts(Utilities::keyList(inputPorts), Utilities::keyList(outputPorts));
    }

    void IOManager::constructPort(int portNumber, const std::string& portName, IOType ioType)
    {
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
                if (inputsChanged || outputsChanged)
                    processor->updatePorts(Utilities::keyList(inputPorts),
                                           Utilities::keyList(outputPorts));
                observerCV.wait_for(lock, std::chrono::seconds(1));
            }
        });
    }

    std::function<bool(const std::pair<std::string, int>&)> IOManager::equals(const std::string& portName)
    {
        return [&](const std::pair<std::string, int>& port){ return port.first == portName; };;
    }

    bool IOManager::checkPorts(PortMap& ports, Port& systemPort, IOType ioType)
    {
        if (systemPort.portCount() == ports.size()) return false;

        std::map<std::string, int> newPorts;
        for (int portNumber = 0; portNumber < systemPort.portCount(); ++portNumber)
            newPorts[systemPort.portName(portNumber)] = portNumber;

        std::list<std::string> removedPorts;

        for (const auto& [portName, port]: ports)
            if (std::find_if(newPorts.begin(), newPorts.end(), equals(port->name)) == newPorts.end())
                removedPorts.emplace_back(port->name);
            else
                newPorts.erase(port->name);

        addPorts(newPorts, ioType);
        removePorts(ports, removedPorts);
        return true;
    }

    void IOManager::addPorts(const std::map<std::string, int>& newPorts, IOType ioType)
    {
        for (const auto& [portName, portNumber]: newPorts)
            constructPort(portNumber, portName, ioType);
    }

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

    void IOManager::sendMIDIOut(const MessageOnInstrument& messageOnInstrument)
    {
        const auto& [message, instrument] = messageOnInstrument;
        OutputPortPointer port = std::static_pointer_cast<OutputPort>(outputPorts[instrument->port]);

        libremidi::message rawMessage = message->rawMessage(instrument->channel);
        port->sendMessage(rawMessage);
    }
}