//
// Created by Casper De Smet on 04/05/2022.
//

#ifndef MIDIASSISTANT_FONT_H
#define MIDIASSISTANT_FONT_H

#include <string>
#include <memory>

#include "../imgui/imgui.h"

namespace UI
{
    // Class that contains a font and a version of the same font with equally spaced glyphs, both fonts containing icons
    class Font
    {
    public:
        ImFontConfig config;
        ImFontConfig iconConfig;
        ImFontConfig equalSpacedConfig;

        ImFont* font;
        ImFont* equalSpacedFont;

        Font(const std::string& fileName, float size, float advance, float iconSize);

        void addIcons(float iconSize);
        // Castable so that the font can be used as the default font
        operator ImFont*() const;
    };

    using FontPointer = std::shared_ptr<Font>;
}

#endif //MIDIASSISTANT_FONT_H
