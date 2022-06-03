//
// Created by Casper De Smet on 14/04/2022.
//

#include "ColorRange.h"

#include <iostream>

#include "../Utilities/Utilities.h"

namespace UI
{
    ColorRange::ColorRange(ImU32 color, ColorBrightness brightness):
                ColorRange(ImGui::ColorConvertU32ToFloat4(color), brightness)
    {}

    ColorRange::ColorRange(const ImVec4& color, ColorBrightness brightness):
                ColorRange(color.x * 255, color.y * 255, color.z * 255, brightness)
    {}

    ColorRange::ColorRange(int red, int green, int blue, ColorBrightness brightness)
    {
        defaultColor = IM_COL32(red, green, blue, 255);

        ImVec4 hsvColor = ImVec4(0, 0, 0, 1);
        ImVec4 color = ImGui::ColorConvertU32ToFloat4(IM_COL32(red, green, blue, 255));
        ImGui::ColorConvertRGBtoHSV(color.x, color.y, color.z, hsvColor.x, hsvColor.y, hsvColor.z);

        ImVec4 hsvDefaultPressed = hsvColor;
        ImVec4 hsvInverseColor = hsvColor;
        ImVec4 hsvInversePressed = hsvColor;

        hsvDefaultPressed.y = Utilities::clamp(hsvDefaultPressed.y + (brightness == Dark ? .3f : -.3f), 0.f, 1.f);
        ImGui::ColorConvertHSVtoRGB(hsvDefaultPressed.x, hsvDefaultPressed.y, hsvDefaultPressed.z,
                                    hsvDefaultPressed.x, hsvDefaultPressed.y, hsvDefaultPressed.z);
        defaultPressed = ImGui::ColorConvertFloat4ToU32(hsvDefaultPressed);

        hsvInverseColor.z = Utilities::clamp(hsvInverseColor.z + (brightness == Dark ? .3f : -.3f), 0.f, 1.f);
        ImGui::ColorConvertHSVtoRGB(hsvInverseColor.x, hsvInverseColor.y, hsvInverseColor.z,
                                    hsvInverseColor.x, hsvInverseColor.y, hsvInverseColor.z);
        inverseColor = ImGui::ColorConvertFloat4ToU32(hsvInverseColor);

        hsvInversePressed.y = Utilities::clamp(hsvInversePressed.y + (brightness == Dark ? .3f : -.3f), 0.f, 1.f);
        hsvInversePressed.z = Utilities::clamp(hsvInversePressed.z + (brightness == Dark ? .3f : -.3f), 0.f, 1.f);
        ImGui::ColorConvertHSVtoRGB(hsvInversePressed.x, hsvInversePressed.y, hsvInversePressed.z,
                                    hsvInversePressed.x, hsvInversePressed.y, hsvInversePressed.z);
        inversePressed = ImGui::ColorConvertFloat4ToU32(hsvInversePressed);

//        std::swap(defaultColor, inverseColor);
//        std::swap(defaultPressed, inversePressed);

        border = brightness == Dark ? ImColor(255, 255, 255) : ImColor(0, 0, 0);
        text = brightness == Light ? ImColor(0, 0, 0) : ImColor(255, 255, 255);
    }

    ColorRange::ColorRange(ImU32 color, ImU32 pressedColor, ColorBrightness brightness):
                           defaultColor(color), defaultPressed(pressedColor),
                           inverseColor(color), inversePressed(pressedColor)
    {
        switch (brightness)
        {
            case Light:
                border = ImColor(0, 0, 0);
                text = ImColor(0, 0, 0);
                break;
            case Normal:
                border = ImColor(0, 0, 0);
                text = ImColor(255, 255, 255);
                break;
            case Dark:
                border = ImColor(255, 255, 255);
                text = ImColor(255, 255, 255);
                break;
        }
    }

    ColorRange::operator ImU32()
    {
        return defaultColor;
    }
}