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
        operator ImFont*() const;
    };

    using FontPointer = std::shared_ptr<Font>;
}

#endif //MIDIASSISTANT_FONT_H
