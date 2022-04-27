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

        KeyColor(ImU32 key,ImU32 pressed, ImU32 border);
    };

    using KeyColorPointer = std::shared_ptr<KeyColor>;
}


#endif //MIDIASSISTANT_KEYCOLOR_H
