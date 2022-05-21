//
// Created by Casper De Smet on 25/03/2022.
//

#ifndef MIDIASSISTANT_UTILITIES_H
#define MIDIASSISTANT_UTILITIES_H

#include <vector>
#include <string>
#include <map>
#include <future>

#include "../MIDI/IO/Port.h"

namespace MIDI
{
    class Port;
    using PortMap = std::map<std::string, PortPointer>;
}

namespace Utilities
{
    std::vector<std::string> keyList(const MIDI::PortMap& map);
    std::future<void> makeThread(const std::function<void()>& threadFunction);
    float clamp(float value, float min, float max);
    int clamp(int value, int min, int max);
    int bottomClamp(int value, int min);
    int sum(const std::vector<int>& vector);
    int positiveModulo(int value, int modulo);
    int numberOfDigits(double value);
};


#endif //MIDIASSISTANT_UTILITIES_H
