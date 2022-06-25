//
// Created by Casper De Smet on 06/03/2022.
//

#include "FileManager.h"

namespace MIDI
{
    std::mutex FileManager::instanceMutex;
    FileManagerPointer FileManager::instance;

    // Link the processor, needed because it is a singleton
    void FileManager::initialize(const std::shared_ptr<Processor>& givenProcessor)
    {
        processor = givenProcessor;
    }

    // Singleton logic
    FileManagerPointer FileManager::getInstance()
    {
        if (!instance)
        {
            std::unique_lock<std::mutex> lock(instanceMutex);
            if (!instance)
                instance = FileManagerPointer(new FileManager());
        }
        return instance;
    }
}
