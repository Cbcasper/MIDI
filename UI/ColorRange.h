//
// Created by Casper De Smet on 14/04/2022.
//

#ifndef MIDIASSISTANT_COLORRANGE_H
#define MIDIASSISTANT_COLORRANGE_H

#include <memory>

#include "../imgui/imgui.h"

namespace UI
{
    class ColorRange
    {
    public:
        enum ColorBrightness
        {
            Light,
            Normal,
            Dark
        };
        ImU32 defaultColor;
        ImU32 defaultPressed;
        ImU32 inverseColor;
        ImU32 inversePressed;
        ImU32 border;
        ImU32 text;

        explicit ColorRange(ImU32 color, ColorBrightness brightness = Normal);
        explicit ColorRange(const ImVec4& color, ColorBrightness brightness = Normal);
        ColorRange(int red, int green, int blue, ColorBrightness brightness = Normal);
        ColorRange(ImU32 color, ImU32 pressedColor, ColorBrightness brightness = Normal);

        operator ImU32();
    };

    using ColorRangePointer = std::shared_ptr<ColorRange>;
}


#endif //MIDIASSISTANT_COLORRANGE_H
