//
// Created by Casper De Smet on 25/03/2022.
//

#include "Utilities.h"

std::vector<std::string> Utilities::keyList(const MIDI::PortMap& map)
{
    std::vector<std::string> keys;
    for (const auto& [key, value]: map)
        keys.emplace_back(key);
    return keys;
}

std::future<void> Utilities::makeThread(const std::function<void()>& threadFunction)
{
    std::packaged_task<void()> task(threadFunction);
    std::future<void> future = task.get_future();
    std::thread(std::move(task)).detach();
    return future;
}

float Utilities::clamp(float value, float min, float max)
{
    if (value < min)        return min;
    else if (value > max)   return max;
    else                    return value;
}
