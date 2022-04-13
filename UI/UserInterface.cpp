//
// Created by Casper De Smet on 22/02/2022.
//

#include <sstream>
#include <iostream>
#include <cmath>
#include <queue>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "../imgui/imgui_internal.h"

#include "../imgui/Renderer.h"
#include "UserInterface.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../MIDI/Messages/Messages/NoteOff.h"
#include "../Theory/TimeDivision.h"

namespace UI
{
    UserInterface::UserInterface(const std::shared_ptr<State::Application>& applicationState,
                                 const std::shared_ptr<System::Sequencer>& sequencer,
                                 const std::shared_ptr<Music::Quantizer>& quantizer):
                                applicationState(applicationState), sequencer(sequencer), quantizer(quantizer)
    {
        previousLastMessage = getLastMessage();
        pianoOutput = std::make_shared<MIDI::Instrument>("KOMPLETE KONTROL A25", 1);

        controlHeight = 100;
        trackListRatio = 1.f / 5.f;
        sequencerRatio = 2.f / 5.f;
        harmonyRatio = 2.f / 5.f;

        measureWidth = 60;
        headerSize = 18;
        scroll = headerSize / 2;
        keyWidth = 10;
        keyLength = 50;

        mainBackground = ImColor(50, 50, 50);
        darkBackground = ImColor(35, 35, 35);

        divisionColor = ImColor(255, 255, 255, 150);
        playingColor = ImColor(255, 0, 0);
        playingColorTransparent = ImColor(255, 0, 0, 50);
        recordingColor = ImColor(255, 255, 255);
        recordingColorTransparent = ImColor(255, 255, 255, 50);
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

        ImGui::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
        ImGui::ShowStackToolWindow();
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
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Sequencer"))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            ImVec2 canvasPosition = ImGui::GetCursorScreenPos();

            ImU32 borderColor = ImColor(255, 255, 255);

            ImVec2 controlSize = ImVec2(canvasSize.x, controlHeight);
            ImVec2 tracklistSize = ImVec2(canvasSize.x * trackListRatio, canvasSize.y - controlHeight);
            ImVec2 sequencerSize = ImVec2(canvasSize.x * sequencerRatio, canvasSize.y - controlHeight);
            ImVec2 harmonyModelSize = ImVec2(canvasSize.x * harmonyRatio, canvasSize.y  - controlHeight);

            ImVec2 controlPosition = canvasPosition;
            ImVec2 tracklistPosition = canvasPosition + ImVec2(0, controlSize.y);
            ImVec2 sequencerPosition = tracklistPosition + ImVec2(tracklistSize.x, 0);
            ImVec2 harmonyPosition = sequencerPosition + ImVec2(sequencerSize.x, 0);

            computeMeasureLength();
            renderControl(controlPosition, controlSize);
            drawList->AddRectFilled(tracklistPosition, tracklistPosition + tracklistSize, mainBackground);
            renderSequencer(sequencerPosition, sequencerSize);
            renderHarmonyModel(harmonyPosition, harmonyModelSize);

            drawList->AddRect(canvasPosition, canvasPosition + canvasSize, borderColor);
            drawList->AddLine(controlPosition + ImVec2(0, controlSize.y), controlPosition + controlSize, borderColor);
            drawList->AddLine(sequencerPosition, sequencerPosition + ImVec2(0, sequencerSize.y), borderColor);
            drawList->AddLine(sequencerPosition + ImVec2(sequencerSize.x, 0), sequencerPosition + sequencerSize, borderColor);
        }
        ImGui::End();
//        ImGui::PopStyleVar();
    }

    void UserInterface::renderControl(const ImVec2& controlPosition, const ImVec2& controlSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        State::SongPointer song = applicationState->song;

        drawList->AddRectFilled(controlPosition, controlPosition + controlSize, mainBackground);

        ImVec2 generalControlPosition = controlPosition + ImVec2(controlSize.x * trackListRatio, 0);
        ImVec2 generalControlSize = controlSize * ImVec2(sequencerRatio, 1);

        ImGui::SetCursorScreenPos(generalControlPosition);
        ImGui::BeginChild("innerControl", generalControlSize, true, ImGuiWindowFlags_NoBackground);

//        ImGui::SetNextItemWidth(generalControlSize.x / 2);
        ImGui::SliderInt("##NumberOfBeats", &song->numberOfBeats, 1, 10);
//        ImGui::SetNextItemWidth(generalControlSize.x / 2);
        if (ImGui::BeginCombo("##TimeDivision", Music::allDivisions[song->timeDivision].c_str()))
        {
            for (const auto& [timeDivision, name]: Music::allDivisions)
            {
                ImGui::PushID(("time division" + name).c_str());
                if (ImGui::Selectable(name.c_str(), timeDivision == song->timeDivision))
                    song->timeDivision = timeDivision;
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
        displayTime(song);
        ImGui::SameLine();

        if (clickableItem("signature", [&]{ ImGui::Text("%d/%d", song->numberOfBeats, song->timeDivision); }))
            std::cout << "Signature clicked\n";
        ImGui::SameLine();

        std::stringstream tempoString;
        double tempo = 60.f * 1000.f * 1000.f / static_cast<double>(song->tempo);
        tempoString.precision(7);
        tempoString << tempo;
        if (clickableItem("tempo", [&]{ ImGui::TextUnformatted(tempoString.str().c_str()); }))
            std::cout << "Tempo clicked\n";
        ImGui::SameLine();
        ImGui::EndChild();
    }

    bool UserInterface::clickableItem(const std::string& id, const std::function<void()>& itemFunction)
    {
        ImVec2 position = ImGui::GetCursorScreenPos();
        itemFunction();
        ImGui::SetCursorScreenPos(position);
        return ImGui::InvisibleButton((id + "click").c_str(), ImGui::GetItemRectSize());
    }

    void UserInterface::displayTime(const State::SongPointer& song)
    {
        int currentTime = applicationState->currentTime + 1;
        float timeInMeasures = static_cast<float>(currentTime) / measureLength;

        int currentMeasures = static_cast<int>(floor(timeInMeasures));
        displayPaddedTimeElement(currentMeasures, 9999);

        int currentBeats = static_cast<int>(floor(timeInMeasures * static_cast<float>(song->numberOfBeats)));
        displayPaddedTimeElement(currentBeats, song->numberOfBeats);

        if (divisionsPerBeat > 1)
        {
            float numberOfDivisions = static_cast<float>(song->timeDivision * song->numberOfBeats);
            int currentDivisions = static_cast<int>(floor(timeInMeasures * numberOfDivisions));
            displayPaddedTimeElement(currentDivisions, static_cast<int>(divisionsPerBeat));
        }

        displayPaddedTimeElement(currentTime, applicationState->ticksPerDivision);
    }

    void UserInterface::displayPaddedTimeElement(int value, int total)
    {
        int displayValue = value % total + 1;
        int paddingLength = Utilities::numberOfDigits(total) - Utilities::numberOfDigits(displayValue);
        ImGui::TextDisabled("%s", std::string(paddingLength, '0').c_str());
        ImGui::SameLine(0.f, 0.f);
        ImGui::Text("%d", displayValue);
        ImGui::SameLine();
    }

    void UserInterface::computeMeasureLength()
    {
        divisionsPerBeat = static_cast<float>(Music::Sixteenth) / static_cast<float>(applicationState->song->timeDivision);
        measureLength = divisionsPerBeat * static_cast<float>(applicationState->song->numberOfBeats * applicationState->ticksPerDivision);
    }

    float UserInterface::computePosition(int ticks)
    {
        return scroll + static_cast<float>(ticks) / measureLength * measureWidth;
    }

    int UserInterface::computeTickDelta(float positionDelta)
    {
        return static_cast<int>(floor(positionDelta / measureWidth * measureLength));
    }

    void UserInterface::renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        mainAreaPosition = sequencerPosition + ImVec2(0, headerSize);
        mainAreaSize = sequencerSize - ImVec2(0, headerSize);

        totalWidth = static_cast<float>(applicationState->song->measures) * measureWidth;

        ImGui::PushClipRect(sequencerPosition, sequencerPosition + sequencerSize, false);

        drawList->AddRectFilled(sequencerPosition, sequencerPosition + sequencerSize, darkBackground);
        drawList->AddRectFilled(mainAreaPosition + ImVec2(scroll, 0),
                                mainAreaPosition + ImVec2(scroll + totalWidth, sequencerSize.y - headerSize),
                                mainBackground);

        renderGrid(drawList);
        renderNotes(drawList);
        float playheadLocation = renderPlayhead(drawList, sequencerPosition);
        sequencerEventButtons(playheadLocation, sequencerPosition, sequencerSize);
        sequencerKeyEvents();

        ImGui::PopClipRect();
    }

    void UserInterface::renderGrid(ImDrawList* drawList)
    {
        State::SongPointer song = applicationState->song;

        drawList->AddLine(mainAreaPosition + ImVec2(scroll, 0),
                          mainAreaPosition + ImVec2(scroll + totalWidth, 0), divisionColor);
        for (int i = 0; i < song->measures; ++i)
        {
            float measureOffset = scroll + static_cast<float>(i) * measureWidth;
            drawList->AddLine(mainAreaPosition + ImVec2(measureOffset, 0),
                              mainAreaPosition + ImVec2(measureOffset, -headerSize),
                              divisionColor);
            for (int j = 1; j < song->numberOfBeats; ++j)
            {
                float beatOffset = measureOffset + j * measureWidth / static_cast<float>(song->numberOfBeats);
                drawList->AddLine(mainAreaPosition + ImVec2(beatOffset, 0),
                                  mainAreaPosition + ImVec2(beatOffset, -headerSize / 2),
                                  divisionColor);
            }
            drawList->AddLine(mainAreaPosition + ImVec2(measureOffset, 0),
                              mainAreaPosition + ImVec2(measureOffset, mainAreaSize.y),
                              divisionColor);
        }
    }

    void UserInterface::renderNotes(ImDrawList* drawList)
    {
        for (const State::TrackPointer& track: applicationState->tracks)
        {
            std::unique_lock<std::mutex> lock(track->mutex);
            for (const auto& [tick, messages]: track->midiMessages)
                for (const MIDI::MessagePointer& message: messages)
                    if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
                    {
                        int end = noteOn->noteEnd ? noteOn->noteEnd->tick : applicationState->currentTime + 1;
                        float flippedValue = static_cast<float>(128 - noteOn->note->value);
                        drawList->AddRectFilled(mainAreaPosition + ImVec2(computePosition(tick), mainAreaSize.y / 128 * flippedValue),
                                                mainAreaPosition + ImVec2(computePosition(end), mainAreaSize.y / 128 * (flippedValue - 1)),
                                                ImColor(0, 0, 255));
                    }
        }
    }

    float UserInterface::renderPlayhead(ImDrawList* drawList, const ImVec2& sequencerPosition)
    {
        ImU32 playheadColor = applicationState->recording ? playingColor : recordingColor;
        ImU32 playheadFillColor = applicationState->recording ? playingColorTransparent : recordingColorTransparent;
        float playheadLocation = computePosition(applicationState->currentTime + 1);
        drawList->AddLine(mainAreaPosition + ImVec2(playheadLocation, 0),
                          mainAreaPosition + ImVec2(playheadLocation, mainAreaSize.y),
                          playheadColor);

        drawList->AddRectFilled(sequencerPosition + ImVec2(playheadLocation - headerSize / 2, 0),
                                sequencerPosition + ImVec2(playheadLocation + headerSize / 2, headerSize),
                                playheadFillColor, headerSize / 10);
        drawList->AddRect(sequencerPosition + ImVec2(playheadLocation - headerSize / 2, 0),
                          sequencerPosition + ImVec2(playheadLocation + headerSize / 2, headerSize),
                          playheadColor, headerSize / 10);
        return playheadLocation;
    }

    void UserInterface::sequencerEventButtons(float playheadLocation, const ImVec2& sequencerPosition, const ImVec2& sequencerSize)
    {
        ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_AllowItemOverlap;
        ImGui::SetCursorScreenPos(sequencerPosition + ImVec2(playheadLocation - headerSize / 2, 0));
        ImGui::InvisibleButton("playhead", ImVec2(headerSize, headerSize), buttonFlags);
        bool playheadActive = ImGui::IsItemActive();

        float positiveScroll = Utilities::clamp(scroll, 0, headerSize / 2);
        ImGui::SetCursorScreenPos(sequencerPosition + ImVec2(positiveScroll, 0));
        ImGui::InvisibleButton("header", ImVec2(sequencerSize.x - positiveScroll, headerSize), buttonFlags);
        bool headerActive = ImGui::IsItemActive();
        if (ImGui::IsItemActivated())
            applicationState->currentTime = computeTickDelta(-scroll + ImGui::GetMousePos().x - sequencerPosition.x);
        if (playheadActive || headerActive)
        {
            int tickDelta = computeTickDelta(ImGui::GetMouseDragDelta().x);
            applicationState->currentTime = std::max(applicationState->currentTime + tickDelta, -1);
            ImGui::ResetMouseDragDelta();
        }

        ImGui::SetCursorScreenPos(sequencerPosition);
        ImGui::InvisibleButton("canvas", sequencerSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        if (ImGui::IsItemHovered())
        {
            float newScroll = scroll + ImGui::GetIO().MouseWheelH;
            scroll = Utilities::clamp(newScroll, -totalWidth + mainAreaSize.x, headerSize / 2);
        }
    }

    void UserInterface::sequencerKeyEvents()
    {
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) measureWidth++;
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) measureWidth--;
        if (ImGui::IsKeyPressed(ImGuiKey_Space))
        {
            applicationState->stopRecording();
            if (sequencer->running)     sequencer->stop();
            else                        sequencer->start();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_R) && !sequencer->running)
        {
            applicationState->recording = true;
            sequencer->start();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) sequencer->reset();
    }

    void UserInterface::renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + harmonyModelSize, mainBackground);
        ImGui::SetCursorScreenPos(harmonyPosition);
        ImGui::Button("Add harmony");
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

            ImU32 black = ImColor(0, 0, 0);
            ImU32 white = ImColor(255, 255, 255);
            ImU32 darkGrey = ImColor(70, 70, 70);
            ImU32 lightGrey = ImColor(220, 220, 220);

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

    void UserInterface::drawKeys(int numberOfKeys, int keyIndex, const ImU32& keyColor,
                                 const ImU32& pressedColor, const ImU32& borderColor)
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
            ImU32 color = active ? pressedColor : keyColor;
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
}