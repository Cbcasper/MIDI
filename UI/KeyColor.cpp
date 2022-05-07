//
// Created by Casper De Smet on 14/04/2022.
//

#include "KeyColor.h"

#include "../Utilities/Utilities.h"

namespace UI
{
    KeyColor::KeyColor(int red, int green, int blue, bool dark)
    {
        ImVec4 hsvColor = ImVec4(0, 0, 0, 1);
        ImVec4 color = ImGui::ColorConvertU32ToFloat4(IM_COL32(red, green, blue, 255));
        ImGui::ColorConvertRGBtoHSV(color.x, color.y, color.z, hsvColor.x, hsvColor.y, hsvColor.z);

        key = ImGui::ColorConvertFloat4ToU32(color);
        hsvColor.z = Utilities::clamp(hsvColor.z + (dark ? .3f : -.3f), 0.f, 1.f);
        ImGui::ColorConvertHSVtoRGB(hsvColor.x, hsvColor.y, hsvColor.z, hsvColor.x, hsvColor.y, hsvColor.z);
        pressed = ImGui::ColorConvertFloat4ToU32(hsvColor);
        border = dark ? ImColor(255, 255, 255) : ImColor(0, 0, 0);
    }

    KeyColor::operator ImU32()
    {
        return key;
    }
}