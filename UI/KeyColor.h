//
// Created by Casper De Smet on 14/04/2022.
//

#ifndef MIDIASSISTANT_KEYCOLOR_H
#define MIDIASSISTANT_KEYCOLOR_H

#include <memory>

#include "../imgui/imgui.h"

namespace UI
{
    class KeyColor
    {
    public:
        ImU32 key;
        ImU32 pressed;
        ImU32 border;

        KeyColor(int red, int green, int blue, bool dark = false);

        operator ImU32();
    };

    using KeyColorPointer = std::shared_ptr<KeyColor>;
}


#endif //MIDIASSISTANT_KEYCOLOR_H
