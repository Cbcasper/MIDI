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

class Utilities
{
public:
    static std::vector<std::string> keyList(const MIDI::PortMap& map);
    static std::future<void> makeThread(const std::function<void()>& threadFunction);
    static float clamp(float value, float min, float max);
    static int clamp(int value, int min, int max);
    static int sum(const std::vector<int>& vector);
    static int positiveModulo(int value, int modulo);
    static int numberOfDigits(double value);
};


#endif //MIDIASSISTANT_UTILITIES_H
