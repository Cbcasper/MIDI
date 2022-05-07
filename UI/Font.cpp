//
// Created by Casper De Smet on 04/05/2022.
//

#include "Font.h"

#include "../imgui/IconsFontAwesome6.h"

namespace UI
{
    Font::Font(const std::string& fileName, float size, float advance, float iconSize)
    {
        ImGuiIO& io = ImGui::GetIO();

        config.OversampleH = 8;
        config.OversampleV = 8;

        equalSpacedConfig.OversampleH = 8;
        equalSpacedConfig.OversampleV = 8;
        equalSpacedConfig.GlyphMinAdvanceX = advance;

        iconConfig.MergeMode = true;
        iconConfig.GlyphMinAdvanceX = iconSize;
        iconConfig.PixelSnapH = true;

        font = io.Fonts->AddFontFromFileTTF(fileName.c_str(), size, &config);
        addIcons(iconSize);
        equalSpacedFont = io.Fonts->AddFontFromFileTTF(fileName.c_str(), size, &equalSpacedConfig);
        addIcons(iconSize);
    }

    void Font::addIcons(float iconSize)
    {
        ImGuiIO& io = ImGui::GetIO();
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        io.Fonts->AddFontFromFileTTF("./imgui/fonts/" FONT_ICON_FILE_NAME_FAS, iconSize, &iconConfig, icon_ranges);
    }

    Font::operator ImFont*() const
    {
        return font;
    }
}