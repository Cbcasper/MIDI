//
// Created by Casper De Smet on 25/03/2022.
//

#include "Utilities.h"

#include <cmath>

namespace Utilities
{

    std::vector<std::string> keyList(const MIDI::PortMap& map)
    {
        std::vector<std::string> keys;
        for (const auto& [key, value]: map)
            keys.emplace_back(key);
        return keys;
    }

    std::future<void> makeThread(const std::function<void()>& threadFunction)
    {
        std::packaged_task<void()> task(threadFunction);
        std::future<void> future = task.get_future();
        std::thread(std::move(task)).detach();
        return future;
    }

    float clamp(float value, float min, float max)
    {
        if (value < min)        return min;
        else if (value > max)   return max;
        else                    return value;
    }

    int clamp(int value, int min, int max)
    {
        if (value < min)        return min;
        else if (value > max)   return max;
        else                    return value;
    }

    int bottomClamp(int value, int min)
    {
        if (value < min)    return min;
        else                return value;
    }

    int sum(const std::vector<int>& vector)
    {
        int sum = 0;
        for (int value: vector)
            sum += value;
        return sum;
    }

    int positiveModulo(int value, int modulo)
    {
        return (value + modulo) % modulo;
    }

    int numberOfDigits(double value)
    {
        return static_cast<int>(ceil(log10(value + 1)));
    }
}
