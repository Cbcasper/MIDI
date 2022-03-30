//
// Created by Casper De Smet on 06/03/2022.
//

#include "../imgui/imgui.h"

#include "MessageFilterCheckboxes.h"

namespace UI
{
    bool MessageFilterCheckboxes::renderCheckboxes()
    {
        rowHeight = 0;
        bool pressed = false;
        for (MIDI::MessageType messageType: supportedTypes)
        {
            manageGroups();
            pressed = pressed || ImGui::Checkbox(typeNames[messageType].c_str(), &allowedTypes[messageType]);
        }
        return pressed;
    }

    void MessageFilterCheckboxes::manageGroups()
    {
        if (rowHeight % 3 == 0)
        {
            if (rowHeight != 0)
                ImGui::EndGroup();
            ImGui::SameLine();
            if (rowHeight != supportedTypes.size() - 1)
                ImGui::BeginGroup();
        }
        rowHeight++;
    }
}