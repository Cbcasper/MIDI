//
// Created by Casper De Smet on 06/03/2022.
//

#ifndef MIDIASSISTANT_FILEMANAGER_H
#define MIDIASSISTANT_FILEMANAGER_H

#include "libremidi/reader.hpp"
#include "libremidi/writer.hpp"
#include "Processor.h"

namespace MIDI
{
    class FileManager;
    using FileManagerPointer = std::shared_ptr<FileManager>;

    class FileManager
    {
    public:
        std::shared_ptr<Processor> processor;

        libremidi::reader reader{};
        libremidi::writer writer{};

        static std::mutex instanceMutex;
        static FileManagerPointer instance;

        void initialize(const std::shared_ptr<Processor>& givenProcessor);
        static FileManagerPointer getInstance();

    private:
        FileManager() = default;
    };
}

#endif //MIDIASSISTANT_FILEMANAGER_H
