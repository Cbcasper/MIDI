//
// Created by Casper De Smet on 14/04/2022.
//

#ifndef MIDIASSISTANT_COLORRANGE_H
#define MIDIASSISTANT_COLORRANGE_H

#include <memory>

#include "../imgui/imgui.h"

namespace UI
{
    // Class that contains a range of colors based on a single color, used to color piano keys and harmonies
    class ColorRange
    {
    public:
        enum ColorBrightness
        {
            Light,
            Normal,
            Dark
        };
        // The color the range is based on
        ImU32 defaultColor;
        // That color when pressed
        ImU32 defaultPressed;
        // The same color, but with the opposite brightness
        ImU32 inverseColor;
        // That color when pressed
        ImU32 inversePressed;
        // Peripherals
        ImU32 border;
        ImU32 text;

        // Multiple ways of passing the color the range is based on
        explicit ColorRange(ImU32 color, ColorBrightness brightness = Normal);
        explicit ColorRange(const ImVec4& color, ColorBrightness brightness = Normal);
        ColorRange(int red, int green, int blue, ColorBrightness brightness = Normal);
        ColorRange(ImU32 color, ImU32 pressedColor, ColorBrightness brightness = Normal);

        // Make the color castable so that it can be used as the default color
        operator ImU32();
    };

    using ColorRangePointer = std::shared_ptr<ColorRange>;
}


#endif //MIDIASSISTANT_COLORRANGE_H
