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
#include "../Director/Harmonies/SingleHarmonies/RandomHarmony.h"
#include "../Director/Harmonies/SingleHarmonies/ModulationHarmony.h"

namespace UI
{
    UserInterface::UserInterface(const std::shared_ptr<State::Application>& applicationState,
                                 const std::shared_ptr<System::Sequencer>& sequencer,
                                 const std::shared_ptr<Music::Quantizer>& quantizer):
                                applicationState(applicationState), sequencer(sequencer), quantizer(quantizer)
    {
        previousLastMessage = getLastMessage();
        pianoOutput = applicationState->selectInstrument(MIDI::Output);

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

        whiteKey = std::make_shared<KeyColor>(ImColor(255, 255, 255), ImColor(220, 220, 220), ImColor(0, 0, 0));
        blackKey = std::make_shared<KeyColor>(ImColor(0, 0, 0), ImColor(70, 70, 70), ImColor(255, 255, 255));
        highlightedColor = std::make_shared<KeyColor>(ImColor(255, 0, 0), ImColor(220, 0, 0), ImColor(0, 0, 0));

        harmonyColors = {{Music::Harmony::Random, ImColor(33, 72, 252)},
                         {Music::Harmony::Transposition, ImColor(240, 65, 23)},
                         {Music::Harmony::Modulation, ImColor(0, 0, 0)},
                         {Music::Harmony::Canon, ImColor(255, 238, 22)},
                         {Music::Harmony::Choral, ImColor(21, 173, 4)}};
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
            if (ImGui::BeginTable("messagesTable", 4, midiMessageTableFlags))
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

        drawList->AddRectFilled(controlPosition, controlPosition + controlSize, mainBackground);

        ImVec2 controlBarPosition = controlPosition + ImVec2(controlSize.x * trackListRatio, 0);
        ImVec2 controlBarSize = controlSize * ImVec2(sequencerRatio, 1);

        ImVec2 harmonySourcePosition = controlPosition + ImVec2(controlSize.x * (1 - harmonyRatio), 0);
        ImVec2 harmonySourceSize = controlSize * ImVec2(harmonyRatio, 1);

        renderControlBar(controlBarPosition, controlBarSize);
        renderHarmonySource(harmonySourcePosition, harmonySourceSize);
    }

    void UserInterface::renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize)
    {
        State::SongPointer song = applicationState->song;
        ImGui::SetCursorScreenPos(controlBarPosition);
        if (ImGui::BeginChild("innerControl", controlBarSize, true, ImGuiWindowFlags_NoBackground))
        {
            renderTime(song);
            ImGui::SameLine();

            ImGui::Text("%d/%d", song->numberOfBeats, song->timeDivision);
            ImGui::OpenPopupOnItemClick("signatureEdit", ImGuiPopupFlags_MouseButtonLeft);
            renderSignatureEdit(song);
            ImGui::SameLine();

            renderTempo();
            ImGui::SameLine();

            renderKey(song->key);
            ImGui::SameLine();
        }
        ImGui::EndChild();
    }

    void UserInterface::renderTime(const State::SongPointer& song)
    {
        if (ImGui::BeginPopup("timeEdit"))
        {
            ImGui::TextUnformatted("Current time");
            ImGui::Separator();
            ImGui::EndPopup();
        }

        int currentTime = applicationState->currentTime + 1;
        float timeInMeasures = static_cast<float>(currentTime) / measureLength;

        int currentMeasures = static_cast<int>(floor(timeInMeasures));
        int measureEdit = renderPaddedTimeElement("measures", currentMeasures, 9999);

        int currentBeats = static_cast<int>(floor(timeInMeasures * static_cast<float>(song->numberOfBeats)));
        int beatEdit = renderPaddedTimeElement("beats", currentBeats, song->numberOfBeats);

        int divisionEdit = 0;
        if (divisionsPerBeat > 1)
        {
            float numberOfDivisions = static_cast<float>(song->timeDivision * song->numberOfBeats);
            int currentDivisions = static_cast<int>(floor(timeInMeasures * numberOfDivisions));
            divisionEdit = renderPaddedTimeElement("divisions", currentDivisions, static_cast<int>(divisionsPerBeat));
        }

        int tickEdit = renderPaddedTimeElement("ticks", currentTime, applicationState->ticksPerDivision);
        if (measureEdit || beatEdit || divisionEdit || tickEdit)
            adjustTime(measureEdit, beatEdit, divisionEdit, tickEdit);
    }

    void UserInterface::adjustTime(int measures, int beats, int divisions, int ticks)
    {
        int measureTicks = measures * static_cast<int>(measureLength);
        int beatTicks = beats * static_cast<int>(divisionsPerBeat) * applicationState->ticksPerDivision;
        int divisionTicks = divisions * applicationState->ticksPerDivision;
        applicationState->currentTime += measureTicks + beatTicks + divisionTicks + ticks;
    }

    int UserInterface::renderPaddedTimeElement(const std::string& id, int value, int total)
    {
        int displayValue = value % total + 1;
        int paddingLength = Utilities::numberOfDigits(total) - Utilities::numberOfDigits(displayValue);
        ImGui::TextDisabled("%s", std::string(paddingLength, '0').c_str());
        ImGui::OpenPopupOnItemClick("timeEdit", ImGuiPopupFlags_MouseButtonLeft);
        ImGui::SameLine(0.f, 0.f);
        ImGui::Text("%d", displayValue);
        ImGui::OpenPopupOnItemClick("timeEdit", ImGuiPopupFlags_MouseButtonLeft);
        ImGui::SameLine();

        bool entered = false;
        int editValue = displayValue;
        if (ImGui::BeginPopup("timeEdit"))
        {
            ImGui::InputInt(id.c_str(), &editValue);
            entered = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::EndPopup();
        }
        return entered ? editValue - displayValue : 0;
    }

    void UserInterface::renderSignatureEdit(const State::SongPointer& song)
    {
        if (ImGui::BeginPopup("signatureEdit"))
        {
            ImGui::Text("Signature: %d/%d", song->numberOfBeats, song->timeDivision);
            ImGui::Separator();
            ImGui::SliderInt("##NumberOfBeats", &song->numberOfBeats, 1, 10);
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
            ImGui::EndPopup();
        }
    }

    void UserInterface::renderTempo()
    {
        std::stringstream tempoString;
        double tempo = 60.f * 1000.f * 1000.f / static_cast<double>(applicationState->song->tempo);
        tempoString.precision(7);
        tempoString << tempo;
        ImGui::TextUnformatted(tempoString.str().c_str());
        ImGui::OpenPopupOnItemClick("tempoEdit", ImGuiPopupFlags_MouseButtonLeft);

        if (ImGui::BeginPopup("tempoEdit"))
        {
            ImGui::TextUnformatted(tempoString.str().c_str());
            ImGui::Separator();
            ImGui::InputDouble("##tempo", &tempo);
            if (ImGui::IsItemDeactivatedAfterEdit())
                applicationState->song->setTempo(static_cast<int>(round(60.f * 1000.f * 1000.f / tempo)));

            ImGui::EndPopup();
        }
    }

    void UserInterface::renderKey(const Music::KeyPointer& key)
    {
        ImGui::TextUnformatted(key->rootNote->toString().c_str());
        ImGui::OpenPopupOnItemClick("keyEdit", ImGuiPopupFlags_MouseButtonLeft);
        ImGui::SameLine(0.f, 0.f);
        ImGui::TextUnformatted(Music::IntervalSequence::sequenceName(key->intervalSequence->type).c_str());
        ImGui::OpenPopupOnItemClick("keyEdit", ImGuiPopupFlags_MouseButtonLeft);

        if (ImGui::BeginPopup("keyEdit"))
        {
            int startOctave = 5;
            const auto& [name, sharp] = *key->rootNote;
            int playedKey = renderOctaves(1, startOctave, "keyRootNoteSelection",
                                          {startOctave * 12 + Music::RootNote::convert(name, sharp)});
            if (playedKey != -1)
                key->setRootNote(Music::RootNote::convert(playedKey % 12));

            ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders;
            if (ImGui::BeginTable("intervalSequenceTable", 2, tableFlags))
            {
                ImGui::TableSetupColumn("Modes", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Scales", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                std::vector<Music::Mode::Type> modes = Music::Mode::allModes();
                std::vector<Music::Scale::Type> scales = Music::Scale::allScales();
                for (int i = 0; i < modes.size(); ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    std::string modeName = Music::Mode::sequenceName(modes[i]);
                    if (ImGui::Selectable(modeName.c_str(), key->intervalSequence->ofType(modes[i]),
                                          ImGuiSelectableFlags_DontClosePopups))
                        key->setIntervalSequence(modes[i]);
                    ImGui::TableNextColumn();
                    if (i < scales.size())
                    {
                        std::string scaleName = Music::Mode::sequenceName(scales[i]);
                        if (ImGui::Selectable(scaleName.c_str(), key->intervalSequence->ofType(scales[i]),
                                              ImGuiSelectableFlags_DontClosePopups))
                            key->setIntervalSequence(scales[i]);
                    }
                }

                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }
    };

    void UserInterface::renderHarmonySource(const ImVec2& harmonySourcePosition, const ImVec2& harmonySourceSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        ImVec2 currentPosition = harmonySourcePosition;

        ImGui::PushID("harmonySource");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding * 2);
        for (Music::Harmony::Type harmony: Music::Harmony::allHarmonies())
        {
            std::string harmonyName = Music::Harmony::harmonyName(harmony);
            ImGui::SetCursorScreenPos(currentPosition);
            if (ImGui::BeginChild(harmonyName.c_str(), harmonySourceSize / ImVec2(5, 1), true, ImGuiWindowFlags_NoBackground))
            {
                ImVec2 position = ImGui::GetCursorScreenPos() - padding;
                ImVec2 size = ImGui::GetContentRegionAvail() + padding;
                drawList->AddRect(position, position + size, harmonyColors[harmony], 0, ImDrawFlags_None, 2);

                ImGui::TextUnformatted(harmonyName.c_str());

                ImGui::SetCursorScreenPos(position);
                ImGui::InvisibleButton(("button" + harmonyName).c_str(), size);
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("Harmony", &harmony, sizeof(Music::Harmony::Type));

                    ImGui::TextUnformatted(harmonyName.c_str());
                    ImGui::EndDragDropSource();
                }
            }
            ImGui::EndChild();

            currentPosition += ImVec2(harmonySourceSize.x / 5, 0);
        }
        ImGui::PopStyleVar();
        ImGui::PopID();
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

        float positiveScroll = Utilities::clamp(scroll, 0.f, headerSize / 2);
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
        sequencerKeyEvents();
    }

    void UserInterface::sequencerKeyEvents()
    {
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) measureWidth++;
        if (ImGui::IsKeyDown(ImGuiKey_ModSuper) && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) measureWidth--;
        if (ImGui::IsItemFocused())
        {
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
    }

    void UserInterface::renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + harmonyModelSize, mainBackground);
        ImGui::SetCursorScreenPos(harmonyPosition);
        ImGui::InvisibleButton("harmonyModel", harmonyModelSize);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Harmony", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                Music::Harmony::Type harmony = *(const Music::Harmony::Type*)payload->Data;
                constructHarmony(harmony);
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorScreenPos(harmonyPosition);
        if (ImGui::BeginChild("harmonyModelTrack", harmonyModelSize, true, ImGuiWindowFlags_NoBackground))
        {
            for (const Music::HarmonyPointer& harmony: applicationState->tracks[0]->harmonies)
                ImGui::TextUnformatted(harmony->name.c_str());
        }
        ImGui::EndChild();
    }

    void UserInterface::constructHarmony(Music::Harmony::Type type)
    {
        Music::HarmonyPointer harmony = nullptr;
        switch (type)
        {
            case Music::Harmony::Random:
                harmony = std::make_shared<Music::RandomHarmony>(applicationState->selectInstrument(MIDI::Input));
                break;
            case Music::Harmony::Transposition:
                harmony = std::make_shared<Music::TranspositionHarmony>(applicationState->selectInstrument(MIDI::Input));
                break;
            case Music::Harmony::Modulation:
                harmony = std::make_shared<Music::ModulationHarmony>(applicationState->selectInstrument(MIDI::Input), applicationState->song->key);
                break;
            case Music::Harmony::Canon:
                break;
            case Music::Harmony::Choral:
                break;
        }
        applicationState->tracks[0]->harmonies.push_back(harmony);
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

            renderOctaves(5, 3, "pianoKeys");
        }
        ImGui::End();
    }

    int UserInterface::renderOctaves(int numberOfOctaves, int startOctave, const std::string& id,
                                      const std::set<int>& highlightedKeys)
    {
        int playedKey = -1;
        ImGui::PushID(id.c_str());
        keyTopLeft = ImGui::GetCursorScreenPos();
        for (int octave = startOctave; octave < startOctave + numberOfOctaves; ++octave)
        {
            keySize = ImVec2(keyWidth, keyLength);
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12, whiteKey, highlightedKeys));
            updatePlayedKey(playedKey, renderPianoKeys(4, octave * 12 + 5, whiteKey, highlightedKeys));

            updateKeyTopLeft(-keyWidth * 7 + keyWidth / 2);
            keySize = ImVec2(keyWidth, keyLength / 3 * 2);
            updatePlayedKey(playedKey, renderPianoKeys(2, octave * 12 + 1, blackKey, highlightedKeys));

            updateKeyTopLeft(keyWidth);
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12 + 6, blackKey, highlightedKeys));
            updateKeyTopLeft(keyWidth / 2);
        }
        updateKeyTopLeft(-keyWidth * 7 * static_cast<float>(numberOfOctaves));
        ImGui::SetCursorScreenPos(keyTopLeft + ImVec2(0, keyLength + keyLength / 20));
        ImGui::PopID();
        return playedKey;
    }

    int UserInterface::renderPianoKeys(int numberOfKeys, int keyIndex, const KeyColorPointer& keyColor, const std::set<int>& highlightedKeys)
    {
        int playedKey = -1;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
                                       ImGuiButtonFlags_AllowItemOverlap;
        for (int i = 0; i < numberOfKeys; ++i)
        {
            int noteValue = keyIndex + i * 2;
            ImGui::PushID(noteValue);
            ImVec2 keyBottomRight = keyTopLeft + keySize;

            ImGui::InvisibleButton("", keySize, buttonFlags);
            ImGui::SetItemAllowOverlap();

            if (ImGui::IsItemActivated() && ImGui::IsItemHovered())   playNote(noteValue);
            if (ImGui::IsItemDeactivated())                           stopNote(noteValue);
            if (ImGui::IsItemClicked())                               playedKey = noteValue;
            bool active = ImGui::IsItemActive();

            KeyColorPointer usedColor = highlightedKeys.find(noteValue) != highlightedKeys.end() ? highlightedColor : keyColor;
            ImU32 currentColor = active ? usedColor->pressed : usedColor->key;
            ImVec2 pressedOffset = active ? ImVec2(0, keySize.y / 20) : ImVec2(0, 0);

            drawList->AddRectFilled(keyTopLeft, keyBottomRight + pressedOffset, currentColor);
            drawList->AddLine(keyTopLeft, keyBottomRight + ImVec2(-keyWidth, 0) + pressedOffset, usedColor->border);
            drawList->AddLine(keyTopLeft + ImVec2(keyWidth, 0), keyBottomRight + pressedOffset, usedColor->border);

            updateKeyTopLeft(keyWidth);
            ImGui::PopID();
        }
        return playedKey;
    }

    void UserInterface::updatePlayedKey(int& currentPlayedKey, int newPlayedKey)
    {
        if (newPlayedKey != -1)
            currentPlayedKey = newPlayedKey;
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