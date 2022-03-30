//
// Created by Casper De Smet on 22/02/2022.
//

#include <sstream>
#include <iostream>
#include <cmath>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "../imgui/imgui_internal.h"

#include "../imgui/Renderer.h"
#include "UserInterface.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../MIDI/Messages/Messages/NoteOff.h"


namespace UI
{
    UserInterface::UserInterface(const std::shared_ptr<State::Application>& applicationState,
                                 const std::shared_ptr<System::Sequencer>& sequencer,
                                 const std::shared_ptr<Music::Quantizer>& quantizer):
                                applicationState(applicationState), sequencer(sequencer), quantizer(quantizer)
    {
        previousLastMessage = getLastMessage();
        divisionWidth = 60;
        scroll = 0;
        keyWidth = 10;
        keyLength = 50;
        pianoOutput = std::make_shared<MIDI::Instrument>("KOMPLETE KONTROL A25", 1);
    }

    void UserInterface::start()
    {
        Renderer renderer = Renderer([&](){ this->renderUI(); }, "MIDI Assistant");
        renderer.start();
    }

    void UserInterface::renderUI()
    {
        renderMessageMonitor();
        renderMainWindow();
        renderPiano();
        renderHarmonyModel();

        ImGui::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
    }

    MIDI::MessagePointer UserInterface::getLastMessage() const
    {
        return !applicationState->messages.empty() ? applicationState->messages.rbegin()->first : nullptr;
    }

    void UserInterface::renderMessageMonitor()
    {
        if (ImGui::Begin("MIDI Monitor"))
        {
            midiMessageTableFlags = ImGuiTableFlags_Borders |
                                    ImGuiTableFlags_RowBg |
                                    ImGuiTableFlags_ScrollY |
                                    ImGuiTableFlags_Resizable;
            ImGui::BeginGroup();
            bool filterChanged = messageMonitorFilter.renderCheckboxes();
            ImGui::SameLine();
            ImGui::BeginGroup();
            if (ImGui::Button("Clear"))
                applicationState->clearMessages();
            ImGui::EndGroup();
            ImGui::EndGroup();
            if (ImGui::BeginTable("table1", 4, midiMessageTableFlags))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Port", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Channel", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const auto& [message, instrument]: applicationState->messages)
                    if (messageMonitorFilter(message))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(instrument->port.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text("Channel %s", std::to_string(instrument->channel).c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(message->message().c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(message->value().c_str());
                    }
                if (getLastMessage() != previousLastMessage || filterChanged)
                    ImGui::SetScrollHereY(1);
                previousLastMessage = getLastMessage();
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void UserInterface::renderMainWindow()
    {
        MIDI::IOManagerPointer midiIOManager = MIDI::IOManager::getInstance();
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Sequencer"))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            ImVec2 canvasPosition = ImGui::GetCursorScreenPos();

            ImColor background = ImColor(50, 50, 50);
            ImColor borderColor = ImColor(255, 255, 255);

            ImVec2 controlSize = ImVec2(canvasSize.x, canvasSize.y / 5);
            ImVec2 tracklistSize = ImVec2(canvasSize.x / 5, canvasSize.y * 4 / 5);
            ImVec2 sequencerSize = ImVec2(canvasSize.x * 2 / 5, canvasSize.y * 4 / 5);
            ImVec2 harmonyModelSize = ImVec2(canvasSize.x * 2 / 5, canvasSize.y * 4 / 5);

            ImVec2 controlPosition = canvasPosition;
            ImVec2 tracklistPosition = canvasPosition + ImVec2(0, controlSize.y);
            ImVec2 sequencerPosition = tracklistPosition + ImVec2(tracklistSize.x, 0);
            ImVec2 harmonyPosition = sequencerPosition + ImVec2(sequencerSize.x, 0);

            drawList->AddRectFilled(controlPosition, controlPosition + controlSize, background);
            drawList->AddRectFilled(tracklistPosition, tracklistPosition + tracklistSize, background);
            drawList->AddRectFilled(sequencerPosition, sequencerPosition + sequencerSize, background);
            renderSequencer(sequencerPosition, sequencerSize);

            drawList->AddRectFilled(harmonyPosition, harmonyPosition + harmonyModelSize, background);

            drawList->AddRect(canvasPosition, canvasPosition + canvasSize, borderColor);
            drawList->AddLine(controlPosition + ImVec2(0, controlSize.y), controlPosition + controlSize, borderColor);
            drawList->AddLine(sequencerPosition, sequencerPosition + ImVec2(0, sequencerSize.y), borderColor);
            drawList->AddLine(sequencerPosition + ImVec2(sequencerSize.x, 0), sequencerPosition + sequencerSize, borderColor);
        }
        ImGui::End();
//        ImGui::PopStyleVar();
    }

    void UserInterface::renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImGui::PushClipRect(sequencerPosition, sequencerPosition + sequencerSize, false);

        int numberOfDivisions = 200;
        ImColor divisionColor = ImColor(255, 255, 255, 150);
        for (int i = 0; i < numberOfDivisions; ++i)
        {
            drawList->AddLine(sequencerPosition + ImVec2(scroll + i * divisionWidth, 0),
                              sequencerPosition + ImVec2(scroll + i * divisionWidth, sequencerSize.y),
                              divisionColor);
        }

        //        ImColor playheadColor = ImColor(255, 0, 0);
        //        drawList->AddLine(sequencerPosition + ImVec2(applicationState->currentTime * divisionWidth, 0),
        //                          sequencerPosition + ImVec2(applicationState->currentTime * divisionWidth, canvasSize.y),
        //                          playheadColor);

        int noteStart = 0;
        int noteEnd = 10;
        drawList->AddRectFilled(sequencerPosition + ImVec2(scroll + noteStart * divisionWidth, 50),
                                sequencerPosition + ImVec2(scroll + noteEnd * divisionWidth, 70),
                                ImColor(0, 0, 255));

        ImGui::SetCursorScreenPos(sequencerPosition);
        ImGui::InvisibleButton("canvas", sequencerSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        if (ImGui::IsItemHovered())
        {
            float totalSize = numberOfDivisions * divisionWidth - sequencerSize.x;
            float newScroll = scroll + ImGui::GetIO().MouseWheelH;
            scroll = Utilities::clamp(newScroll, -totalSize, 0);
        }
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) divisionWidth++;
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) divisionWidth--;
        ImGui::PopClipRect();
    }

    void UserInterface::renderPiano()
    {
        bool open = true;
//        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
//        ImGui::SetNextWindowSize(ImVec2(140, 100));
        if (ImGui::Begin("Piano", &open))
        {
            if (ImGui::BeginCombo("Output Port", pianoOutput->port.c_str()))
            {
                for (const std::string& outputPort: applicationState->outputPorts)
                {
                    ImGui::PushID(("output port" + outputPort).c_str());
                    if (ImGui::Selectable(outputPort.c_str(), outputPort == pianoOutput->port))
                        pianoOutput->port = outputPort;
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
            ImGui::SliderInt("Channel", &pianoOutput->channel, 1, 16, "%d", sliderFlags);

            keyTopLeft = ImGui::GetCursorScreenPos();

            ImColor black = ImColor(0, 0, 0);
            ImColor white = ImColor(255, 255, 255);
            ImColor darkGrey = ImColor(70, 70, 70);
            ImColor lightGrey = ImColor(220, 220, 220);

            int numberOfOctaves = 5;
            for (int octave = 0; octave < numberOfOctaves; ++octave)
            {
                keySize = ImVec2(keyWidth, keyLength);
                drawKeys(3, octave * 12, white, lightGrey, black);
                drawKeys(4, octave * 12 + 5, white, lightGrey, black);

                updateKeyTopLeft(-keyWidth * 7 + keyWidth / 2);
                keySize = ImVec2(keyWidth, keyLength / 3 * 2);
                drawKeys(2, octave * 12 + 1, black, darkGrey, white);

                updateKeyTopLeft(keyWidth);
                drawKeys(3, octave * 12 + 6, black, darkGrey, white);
                updateKeyTopLeft(keyWidth / 2);
            }
            updateKeyTopLeft(-keyWidth * 7 * numberOfOctaves);
            ImGui::SetCursorScreenPos(keyTopLeft + ImVec2(0, keyLength + keyLength / 20));
        }
        ImGui::End();
    }

    void UserInterface::drawKeys(int numberOfKeys, int keyIndex, const ImColor& keyColor,
                                 const ImColor& pressedColor, const ImColor& borderColor)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        for (int i = 0; i < numberOfKeys; ++i)
        {
            ImGui::PushID(keyIndex + i * 2);
            ImVec2 keyBottomRight = keyTopLeft + keySize;

            ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
                                           ImGuiButtonFlags_AllowItemOverlap;
            ImGui::InvisibleButton("", keySize, buttonFlags);
            if (ImGui::IsItemActivated() && ImGui::IsItemHovered())   playNote(36 + keyIndex + i * 2);
            if (ImGui::IsItemDeactivated())                           stopNote(36 + keyIndex + i * 2);
            ImGui::SetItemAllowOverlap();

            bool active = ImGui::IsItemActive();
            ImVec2 pressedOffset = active ? ImVec2(0, keySize.y / 20) : ImVec2(0, 0);
            ImColor color = active ? pressedColor : keyColor;
            drawList->AddRectFilled(keyTopLeft, keyBottomRight + pressedOffset, color);
            drawList->AddLine(keyTopLeft, keyBottomRight + ImVec2(-keyWidth, 0) + pressedOffset, borderColor);
            drawList->AddLine(keyTopLeft + ImVec2(keyWidth, 0), keyBottomRight + pressedOffset, borderColor);

            updateKeyTopLeft(keyWidth);
            ImGui::PopID();
        }
    }

    void UserInterface::updateKeyTopLeft(float adjustment)
    {
        keyTopLeft.x += adjustment;
        ImGui::SetCursorScreenPos(keyTopLeft);
    }

    void UserInterface::playNote(int noteValue)
    {
        MIDI::NoteOnPointer noteOnMessage = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(noteValue), 64);
        pianoPlayer.processMIDIMessage(noteOnMessage->rawMessage(pianoOutput->channel));
        MIDI::IOManager::getInstance()->sendMIDIOut(std::make_pair(noteOnMessage, pianoOutput));
    }

    void UserInterface::stopNote(int noteValue)
    {
        MIDI::NoteOffPointer noteOffMessage = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(noteValue), 64);
        pianoPlayer.processMIDIMessage(noteOffMessage->rawMessage(pianoOutput->channel));
        MIDI::IOManager::getInstance()->sendMIDIOut(std::make_pair(noteOffMessage, pianoOutput));
    }

    void UserInterface::renderHarmonyModel()
    {
        if (ImGui::Begin("Harmony model"))
        {
            ImGui::InputInt("Modulation", &applicationState->modulationHarmony.modulationOffset);
            ImGui::Text("%d", applicationState->modulationHarmony.modulationOffset);
        }
        ImGui::End();
    }
}