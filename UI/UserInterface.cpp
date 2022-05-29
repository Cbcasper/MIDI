//
// Created by Casper De Smet on 22/02/2022.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <queue>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "../imgui/imgui_internal.h"
#include "../imgui/Renderer.h"
#include "../imgui/IconsFontAwesome6.h"

#include "UserInterface.h"
#include "../MIDI/Messages/Messages/NoteOn.h"
#include "../MIDI/Messages/Messages/NoteOff.h"
#include "../Theory/TimeDivision.h"
#include "../Director/Harmonies/SingleHarmonies/RandomHarmony.h"
#include "../Director/Harmonies/SingleHarmonies/ModulationHarmony.h"
#include "../Director/Harmonies/CanonHarmony.h"
#include "../Director/Harmonies/ChoralHarmony/ChoralHarmony.h"

namespace UI
{
    UserInterface::UserInterface(const std::shared_ptr<State::Application>& applicationState,
                                 const std::shared_ptr<Music::Director>& director,
                                 const std::shared_ptr<System::Sequencer>& sequencer):
                                 applicationState(applicationState), director(director), sequencer(sequencer)
    {
        previousLastMessage = getLastMessage();
        pianoOutput = applicationState->selectInstrument(MIDI::Output);

        controlHeight = 102;
        trackListRatio = 1.f / 5.f;
        sequencerRatio = 2.f / 5.f;
        harmonyRatio = 2.f / 5.f;

        measureWidth = 60;
        headerSize = 18;
        scroll = headerSize / 2;
        keyWidth = 10;
        keyLength = 50;

        quantizeDivision = Music::Eighth;

        ImU32 yellow = ImColor(255, 217, 0);

        mainBackground = ImColor(40, 40, 40);
        darkBackground = ImColor(35, 35, 35);
        mainBorder = ImColor(255, 255, 255);
        darkBorder = ImColor(25, 25, 25);
        harmonyHeaderColor = ImColor(15, 158, 206);
        tracklistHeaderColor = ImColor(226, 65, 86);

        divisionColor = ImColor(255, 255, 255, 150);
        playingColor = ImColor(255, 255, 255);
        playingColorTransparent = ImColor(255, 255, 255, 50);
        recordingColor = ImColor(232, 9, 9);
        recordingColorTransparent = ImColor(232, 9, 9, 50);

        cycleColor = yellow;
        metronomeColor = ImColor(122, 0, 255);

        selectedTakeColor = yellow;

        whiteKey = std::make_shared<KeyColor>(255, 255, 255);
        blackKey = std::make_shared<KeyColor>(0, 0, 0, true);
        primaryHighlight = std::make_shared<KeyColor>(240, 65, 23);
        secondaryHighlight = std::make_shared<KeyColor>(33, 72, 252);

        harmonyColors = {{Music::Harmony::Random, std::make_shared<KeyColor>(33, 72, 252)},
                         {Music::Harmony::Transposition, std::make_shared<KeyColor>(240, 65, 23)},
                         {Music::Harmony::Modulation, std::make_shared<KeyColor>(0, 0, 0, true)},
                         {Music::Harmony::Canon, std::make_shared<KeyColor>(255, 238, 22)},
                         {Music::Harmony::Choral, std::make_shared<KeyColor>(21, 173, 4)}};

        pianoAction.push([&](const MIDI::MessagePointer& message){ playPianoMessage(message); });
        keyPressed.push([](int noteValue){ return false; });
        initializeKeyColors();

        selectedTrack = applicationState->tracks[0];
    }

    void UserInterface::start()
    {
        Renderer renderer = Renderer([&](){ this->renderUI(); }, "MIDI Assistant");
        renderer.start(defaultFont, mediumFont, largeFont);
    }

    void UserInterface::renderUI()
    {
        renderMessageMonitor();
        renderMainWindow();
//        renderPiano();

//        ImGui::ShowDemoWindow();
//        ImGui::ShowMetricsWindow();
//        ImGui::ShowStackToolWindow();
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
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        bool open;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (ImGui::Begin("Main window", &open, flags))
        {
            ImGui::PopStyleVar();
            drawList = ImGui::GetWindowDrawList();

            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            ImVec2 canvasPosition = ImGui::GetCursorScreenPos();

            ImVec2 controlSize = ImVec2(canvasSize.x, controlHeight);
            ImVec2 tracklistSize = ImVec2(canvasSize.x * trackListRatio, canvasSize.y - controlHeight);
            ImVec2 sequencerSize = ImVec2(canvasSize.x * sequencerRatio, canvasSize.y - controlHeight);
            ImVec2 harmonyModelSize = ImVec2(canvasSize.x * harmonyRatio, canvasSize.y  - controlHeight);

            ImVec2 controlPosition = canvasPosition;
            ImVec2 tracklistPosition = canvasPosition + ImVec2(0, controlSize.y);
            ImVec2 sequencerPosition = tracklistPosition + ImVec2(tracklistSize.x, 0);
            ImVec2 harmonyPosition = sequencerPosition + ImVec2(sequencerSize.x, 0);

            float ticksPerBeat;
            applicationState->song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, measureLength);
            renderTrackList(tracklistPosition, tracklistSize);
            renderSequencer(sequencerPosition, sequencerSize);
            renderHarmonyModel(harmonyPosition, harmonyModelSize);
            renderControl(controlPosition, controlSize);

            drawList->AddLine(controlPosition + ImVec2(0, controlSize.y), controlPosition + controlSize, mainBorder);
            drawList->AddLine(sequencerPosition, sequencerPosition + ImVec2(0, sequencerSize.y), mainBorder);
            drawList->AddLine(sequencerPosition + ImVec2(sequencerSize.x, 0), sequencerPosition + sequencerSize, mainBorder);

            renderTracks(tracklistPosition, sequencerPosition, harmonyPosition,
                         tracklistSize.x, sequencerSize.x, harmonyModelSize.x);
        }
        ImGui::End();
    }

    void UserInterface::renderControl(const ImVec2& controlPosition, const ImVec2& controlSize)
    {
        drawList->AddRectFilled(controlPosition, controlPosition + controlSize, mainBackground);

        ImVec2 trackControlsPosition = controlPosition;
        ImVec2 controlBarPosition = controlPosition + ImVec2(controlSize.x * trackListRatio, 0);
        ImVec2 harmonySourcePosition = controlPosition + ImVec2(controlSize.x * (1 - harmonyRatio), 0);

        ImVec2 trackControlsSize = controlSize * ImVec2(trackListRatio, 1);
        ImVec2 controlBarSize = controlSize * ImVec2(sequencerRatio, 1);
        ImVec2 harmonySourceSize = controlSize * ImVec2(harmonyRatio, 1);

        renderTrackControls(trackControlsPosition, trackControlsSize);
        renderControlBar(controlBarPosition, controlBarSize);
        renderHarmonySource(harmonySourcePosition, harmonySourceSize);
    }

    void UserInterface::renderTrackControls(const ImVec2& trackControlsPosition, const ImVec2& trackControlsSize)
    {
        ImGui::SetCursorScreenPos(trackControlsPosition);
        if (ImGui::BeginChild("trackControls", trackControlsSize, true, ImGuiWindowFlags_NoBackground))
        {
            if (ImGui::Button("Add Track"))
            {
                MIDI::InstrumentPointer input = applicationState->selectInstrument(MIDI::Input);
                MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
                applicationState->tracks.emplace_back(std::make_shared<State::Track>(applicationState, input, output));
            }

            renderTimeDivisionSelect(quantizeDivision);
            ImGui::SameLine();
            if (ImGui::Button("Quantize"))
                applicationState->tracks[0]->quantize();
        }
        ImGui::EndChild();
    }

    void UserInterface::renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize)
    {
        ImGui::SetCursorScreenPos(controlBarPosition);
        if (ImGui::BeginChild("innerControl", ImVec2(402.5, 102), true,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::PushFont(*largeFont);
            renderSongData();
            ImGui::Spacing();
            ImGui::PopFont();

            ImGui::PushFont(*mediumFont);
            renderSequencerControls();
            ImGui::PopFont();
        }
        ImGui::EndChild();
    }

    void UserInterface::renderSongData()
    {
        State::SongPointer song = applicationState->song;

        ImVec2 position = ImGui::GetCursorScreenPos();
        ImGui::BeginGroup();
        ImGui::Spacing();   ImGui::Spacing();   ImGui::Spacing();   ImGui::SameLine();

        ImGui::PushFont(largeFont->equalSpacedFont);
        renderTime(song);
        ImGui::PopFont();
        ImGui::SameLine();

        renderSignature(song);
        ImGui::SameLine();

        renderTempo();
        ImGui::SameLine();

        renderKey(song->key);
        ImGui::SameLine();

        ImGui::SameLine();  ImGui::Spacing();   ImGui::Spacing();   ImGui::Spacing();
        ImGui::EndGroup();
        drawList->AddRect(position, position + ImGui::GetItemRectSize(), darkBorder,
                          10, ImDrawFlags_None, 3);
    }

    void UserInterface::renderTime(const State::SongPointer& song)
    {
        if (ImGui::BeginPopup("timeEdit"))
        {
            ImGui::TextUnformatted("Current time");
            ImGui::Separator();
            ImGui::EndPopup();
        }

        ImGui::BeginGroup();
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

        int ticksPerDivision = applicationState->song->ticksPerDivision;
        int tickEdit = renderPaddedTimeElement("ticks", currentTime, ticksPerDivision);
        ImGui::EndGroup();
        ImGui::OpenPopupOnItemClick("timeEdit", ImGuiPopupFlags_MouseButtonLeft);

        if (measureEdit || beatEdit || divisionEdit || tickEdit)
            adjustTime(measureEdit, beatEdit, divisionEdit, tickEdit);
    }

    void UserInterface::adjustTime(int measures, int beats, int divisions, int ticks)
    {
        int ticksPerDivision = applicationState->song->ticksPerDivision;
        int measureTicks = measures * static_cast<int>(measureLength);
        int beatTicks = beats * static_cast<int>(divisionsPerBeat) * ticksPerDivision;
        int divisionTicks = divisions * ticksPerDivision;
        int offset = measureTicks + beatTicks + divisionTicks + ticks;
        applicationState->currentTime = Utilities::bottomClamp(applicationState->currentTime + offset, -1);
    }

    int UserInterface::renderPaddedTimeElement(const std::string& id, int value, int total)
    {
        int displayValue = value % total + 1;
        int paddingLength = Utilities::numberOfDigits(total) - Utilities::numberOfDigits(displayValue);
        ImGui::SetNextItemWidth(100);
        ImGui::TextDisabled("%s", std::string(paddingLength, '0').c_str());
        ImGui::SameLine(0.f, 0.f);
        ImGui::Text("%d", displayValue);
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

    void UserInterface::renderSignature(const State::SongPointer& song)
    {
        ImGui::Text("%d/%d", song->numberOfBeats, song->timeDivision);
        ImGui::OpenPopupOnItemClick("signatureEdit", ImGuiPopupFlags_MouseButtonLeft);
        if (ImGui::BeginPopup("signatureEdit"))
        {
            ImGui::Text("Signature: %d/%d", song->numberOfBeats, song->timeDivision);
            ImGui::Separator();
            ImGui::SliderInt("##NumberOfBeats", &song->numberOfBeats, 1, 10);
            renderTimeDivisionSelect(song->timeDivision);
            ImGui::EndPopup();
        }
    }

    void UserInterface::renderTempo()
    {
        std::stringstream tempoString;
        double tempo = round(60.f * 1000.f * 1000.f / static_cast<double>(applicationState->song->tempo) * 1000.f) / 1000.f;
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
        ImGui::SameLine(0.f, 0.5f);
        ImGui::TextUnformatted(Music::IntervalSequence::sequenceName(key->intervalSequence->type).c_str());
        ImGui::OpenPopupOnItemClick("keyEdit", ImGuiPopupFlags_MouseButtonLeft);

        if (ImGui::BeginPopup("keyEdit"))
        {
            int startOctave = 5;
            const auto& [name, sharp] = *key->rootNote;
            int rootNoteValue = Music::RootNote::convert(name, sharp) + startOctave * 12;
            pushKeyColors({rootNoteValue}, primaryHighlight);
            int playedKey = renderOctaves(1, startOctave, "keyRootNoteSelection");
            popKeyColors({rootNoteValue});
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
                        std::string scaleName = Music::Scale::sequenceName(scales[i]);
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

    void UserInterface::renderSequencerControls()
    {
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(" " ICON_FA_BACKWARD " "))
            adjustTime(-1, 0, 0, 0);
        ImGui::SameLine(0.f, 1.f);

        if (ImGui::Button(" " ICON_FA_FORWARD " "))
            adjustTime(1, 0, 0, 0);
        ImGui::SameLine(0.f, 1.f);
        ImGui::PopButtonRepeat();

        if (renderColoredControlButton(ICON_FA_CIRCLE, true, recordingColor))
            record();
        ImGui::SameLine(0.f, 1.f);

        if (renderPlayButton())
            play();
        ImGui::SameLine(0.f, 1.f);

        System::CyclePointer cycle = sequencer->cycle;
        if (renderColoredControlButton(ICON_FA_ARROWS_ROTATE, cycle->cycling, cycleColor))
            cycle->cycling = !cycle->cycling;
        ImGui::SameLine(0.f, 1.f);

        System::MetronomePointer metronome = sequencer->metronome;
        if (renderColoredControlButton(ICON_FA_BELL, metronome->clicking, metronomeColor))
            metronome->clicking = !metronome->clicking;
        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_CHEVRON_LEFT ICON_FA_CHEVRON_RIGHT))
            measureWidth /= .9;
        ImGui::SameLine(0.f, 1.f);
        if (ImGui::Button(ICON_FA_CHEVRON_RIGHT ICON_FA_CHEVRON_LEFT))
            measureWidth *= .9;
    }

    bool UserInterface::renderPlayButton()
    {
        if (sequencer->running) return ImGui::Button(" " ICON_FA_PAUSE " ");
        else                    return ImGui::Button(" " ICON_FA_PLAY " ");
    }

    bool UserInterface::renderColoredControlButton(const std::string& icon, bool colored, ImU32 color)
    {
        if (colored) ImGui::PushStyleColor(ImGuiCol_Text, color);
        bool clicked = ImGui::Button((" " + icon + " ").c_str());
        if (colored) ImGui::PopStyleColor();
        return clicked;
    }

    void UserInterface::renderHarmonySource(const ImVec2& harmonySourcePosition, const ImVec2& harmonySourceSize)
    {
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        ImVec2 currentPosition = harmonySourcePosition;

        ImGui::PushID("harmonySource");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding * 2);
        for (Music::Harmony::Type harmony: Music::Harmony::allHarmonies())
        {
            std::string harmonyName = Music::Harmony::harmonyName(harmony);
            ImVec2 position;
            ImVec2 size;

            ImGui::SetCursorScreenPos(currentPosition);
            bool open = ImGui::BeginChild(harmonyName.c_str(), harmonySourceSize / ImVec2(5, 1), true,
                                          ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
            if (open)
            {
                position = ImGui::GetCursorScreenPos() - padding;
                size = ImGui::GetContentRegionAvail() + padding * 2;
                drawList->AddRect(position, position + size, *harmonyColors[harmony], 0, ImDrawFlags_None, 2);

                ImGui::TextUnformatted(harmonyName.c_str());
                ImGui::Separator();
                ImGui::TextUnformatted("Parameters");
            }
            ImGui::EndChild();
            if (open)
            {
                ImGui::SetCursorScreenPos(position);
                ImGui::InvisibleButton(("button" + harmonyName).c_str(), size);
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("Harmony", &harmony, sizeof(Music::Harmony::Type));
                    ImGui::TextUnformatted(harmonyName.c_str());
                    ImGui::EndDragDropSource();
                }
            }
            currentPosition += ImVec2(harmonySourceSize.x / 5, 0);
        }
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void UserInterface::renderTrackList(const ImVec2& tracklistPosition, const ImVec2& tracklistSize)
    {
        drawList->AddRectFilled(tracklistPosition, tracklistPosition + tracklistSize, mainBackground);
        drawList->AddRectFilled(tracklistPosition, tracklistPosition + ImVec2(tracklistSize.x, headerSize),
                                tracklistHeaderColor);
        drawList->AddLine(tracklistPosition + ImVec2(0, headerSize),
                          tracklistPosition + ImVec2(tracklistSize.x, headerSize),
                          mainBorder);
    }

    float UserInterface::computeXPosition(int ticks)
    {
        return scroll + static_cast<float>(ticks) / measureLength * measureWidth;
    }

    float UserInterface::computeYPosition(const ImVec2& trackSize, const State::TakePointer& take, int value)
    {
        int highestNote = take->highestNote == -1 ? 64 : take->highestNote;
        int lowestNote = take->lowestNote == 128 ? 64 : take->lowestNote;
        int range = Utilities::clamp(highestNote - lowestNote + 24, 0, 127 + 24);
        int rangedValue = value - lowestNote + 12;
        return trackSize.y / static_cast<float>(range) * static_cast<float>(range - rangedValue);
    }

    int UserInterface::computeTickDelta(float positionDelta)
    {
        return static_cast<int>(floor(positionDelta / measureWidth * measureLength));
    }

    void UserInterface::renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize)
    {
        mainAreaPosition = sequencerPosition + ImVec2(0, headerSize);
        mainAreaSize = sequencerSize - ImVec2(0, headerSize);

        totalSequencerWidth = static_cast<float>(applicationState->song->measures) * measureWidth;

        ImGui::PushClipRect(sequencerPosition, sequencerPosition + sequencerSize, false);

        drawList->AddRectFilled(sequencerPosition, sequencerPosition + sequencerSize, darkBackground);
        drawList->AddRectFilled(mainAreaPosition + ImVec2(scroll, 0),
                                mainAreaPosition + ImVec2(scroll + totalSequencerWidth, sequencerSize.y - headerSize),
                                mainBackground);

        renderGrid();
        float playheadLocation = renderPlayhead(sequencerPosition);
        sequencerEventButtons(playheadLocation, sequencerPosition, sequencerSize);

        ImGui::PopClipRect();
    }

    void UserInterface::renderGrid()
    {
        State::SongPointer song = applicationState->song;

        ImGui::GetStyle().AntiAliasedLines = false;
        drawList->AddLine(mainAreaPosition + ImVec2(scroll, 0),
                          mainAreaPosition + ImVec2(scroll + totalSequencerWidth, 0), divisionColor);
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
        ImGui::GetStyle().AntiAliasedLines = true;
    }

    bool UserInterface::renderNotes(const State::TakePointer& take, const ImVec2& takePosition, const ImVec2& takeSize)
    {
        bool noteClicked = false;
        ImGui::PushID((void*)take.get());
        ImGui::PushClipRect(takePosition, takePosition + takeSize, false);
        std::unique_lock<std::mutex> lock(take->mutex);
        for (const auto& [tick, messages]: take->midiMessages)
            for (const MIDI::MessagePointer& message: messages)
                if (MIDI::NoteOnPointer noteOn = std::dynamic_pointer_cast<MIDI::NoteOn>(message))
                {
                    int end = noteOn->noteEnd ? noteOn->noteEnd->tick : applicationState->currentTime + 1;

                    ImVec4 noteColor = ImVec4(0, 0, 0, 1);
                    float mappedVelocity = ((static_cast<float>(noteOn->velocity) / 127.f) * 102 + 25) / 127.f;
                    ImGui::ColorConvertHSVtoRGB(1 - mappedVelocity, .75, .9,
                                                noteColor.x, noteColor.y, noteColor.z);

                    float startX = computeXPosition(tick);
                    float startY = computeYPosition(takeSize, take, noteOn->note->value + 1);
                    float endX = computeXPosition(end);
                    float endY = computeYPosition(takeSize, take, noteOn->note->value);
                    drawList->AddRectFilled(takePosition + ImVec2(startX, startY),
                                            takePosition + ImVec2(endX, endY),
                                            ImGui::ColorConvertFloat4ToU32(noteColor));
                    if (take->noteSelected(noteOn))
                        drawList->AddRectFilled(takePosition + ImVec2(startX + 2, startY + 2),
                                                takePosition + ImVec2(endX - 2, endY - 2),
                                                ImColor(255, 255, 255, 200));

                    ImGui::PushID((void*)noteOn.get());
                    ImGui::SetCursorScreenPos(takePosition + ImVec2(startX, startY));
                    if (ImGui::InvisibleButton("", ImVec2(endX, endY) - ImVec2(startX, startY)))
                        take->selectNote(noteOn, ImGui::IsKeyDown(ImGuiKey_ModShift));
                    noteClicked = noteClicked || ImGui::IsItemClicked();
                    ImGui::PopID();
                }
        ImGui::PopClipRect();

        ImGui::SetCursorScreenPos(takePosition);
        ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_AllowItemOverlap;
        ImGui::InvisibleButton("takeButton", takeSize, flags);
        ImGui::SetItemAllowOverlap();
        ImGui::PopID();
        return noteClicked;
    }

    float UserInterface::renderPlayhead(const ImVec2& sequencerPosition)
    {
        ImU32 playheadColor = applicationState->recording ? recordingColor : playingColor;
        ImU32 playheadFillColor = applicationState->recording ? recordingColorTransparent : playingColorTransparent;
        float playheadLocation = computeXPosition(applicationState->currentTime + 1);
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

        ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight;// | ImGuiButtonFlags_AllowItemOverlap;
        ImGui::SetCursorScreenPos(sequencerPosition);
        ImGui::InvisibleButton("canvas", sequencerSize, flags);
        ImGui::SetItemAllowOverlap();
        if (ImGui::IsItemHovered())
        {
            float newScroll = scroll + ImGui::GetIO().MouseWheelH;
            scroll = Utilities::clamp(newScroll, -totalSequencerWidth + mainAreaSize.x, headerSize / 2);
        }
        if (ImGui::IsItemClicked())
        {}
        sequencerKeyEvents();
    }

    void UserInterface::sequencerKeyEvents()
    {
        bool super = ImGui::IsKeyDown(ImGuiKey_ModSuper);
        if (super && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) measureWidth /= .75;
        if (super && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))  measureWidth *= .75;
        if (super && ImGui::IsKeyPressed(ImGuiKey_Q))          selectedTrack->recordingTake->selectAllNotes();
        if (ImGui::IsKeyPressed(ImGuiKey_Space))        play();
        if (ImGui::IsKeyPressed(ImGuiKey_R))            record();
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))        sequencer->reset();
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace))    deleteItems();
    }

    void UserInterface::deleteItems()
    {
        selectedTrack->recordingTake->deleteSelectedNotes();
    }

    void UserInterface::renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize)
    {
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + harmonyModelSize, mainBackground);
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + ImVec2(harmonyModelSize.x, headerSize), harmonyHeaderColor);
        drawList->AddLine(harmonyPosition + ImVec2(0, headerSize),
                          harmonyPosition + ImVec2(harmonyModelSize.x, headerSize), mainBorder);
    }

    void UserInterface::renderTrackHarmonyModel(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                                const ImVec2& trackHarmonySize)
    {
        ImGui::SetCursorScreenPos(trackHarmonyPosition);
        ImGui::InvisibleButton("harmonyModel", trackHarmonySize);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Harmony", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                Music::Harmony::Type harmony = *(const Music::Harmony::Type*)payload->Data;
                constructHarmony(harmony, track);
            }
            ImGui::EndDragDropTarget();
        }

        if (track->hasSelectedHarmonies() && track->harmonies.size() > 1)
            renderSelectedHarmonyView(track, trackHarmonyPosition, trackHarmonySize);
        else
            renderDefaultHarmonyView(track, trackHarmonyPosition, trackHarmonySize);
    }

    void UserInterface::renderDefaultHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                                 const ImVec2& trackHarmonySize)
    {
        ImVec2 currentPosition = trackHarmonyPosition;
        for (const Music::HarmonyPointer& harmony: track->harmonies)
        {
            float numberOfHarmonies = static_cast<float>(track->harmonies.size());
            ImGui::PushID((void*)harmony.get());
            renderTrackHarmony(track, harmony, currentPosition,
                               trackHarmonySize / ImVec2(numberOfHarmonies, 1));
            currentPosition.x += trackHarmonySize.x / numberOfHarmonies;
            ImGui::PopID();
        }
    }

    void UserInterface::renderSelectedHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                                 const ImVec2& trackHarmonySize)
    {
        float selectedRatio = 2.f / 3.f;
        ImVec2 currentPosition = trackHarmonyPosition + ImVec2(trackHarmonySize.x * selectedRatio, 0);
        for (const Music::HarmonyPointer& harmony: track->harmonies)
        {
            ImGui::PushID((void*)harmony.get());
            if (harmony->selected)
                renderTrackHarmony(track, harmony, trackHarmonyPosition,
                                   trackHarmonySize * ImVec2(selectedRatio, 1));
            else
            {
                float numberOfHarmonies = static_cast<float>(track->harmonies.size()) - 1;
                renderTrackHarmony(track, harmony, currentPosition,
                                   trackHarmonySize / ImVec2(3, numberOfHarmonies));
                currentPosition.y += trackHarmonySize.y / numberOfHarmonies;
            }
            ImGui::PopID();
        }
    }

    void UserInterface::renderTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                           const ImVec2& trackHarmonyPosition, const ImVec2& trackHarmonySize)
    {
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        ImGui::SetCursorScreenPos(trackHarmonyPosition);
        if (ImGui::BeginChild("harmony", trackHarmonySize, true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            ImVec2 position = trackHarmonyPosition + padding / 2;
            ImVec2 size = ImGui::GetContentRegionAvail() + padding;

            drawList->AddRect(position, position + size, *harmonyColors[harmony->type],
                              0.f, ImDrawFlags_None, 2.f);
            ImGui::PushClipRect(position + padding / 2, position + size - padding / 2, true);
            ImGui::TextUnformatted(harmony->name.c_str());
            bool selected = harmony->selected;
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsItemClicked())
            {
                track->clearSelectedHarmonies();
                harmony->selected = !selected;
            }
            if (selected)
            {
//                ImGui::Text("Hello world");
//                ImGui::SameLine();
                if (ImGui::BeginTabBar("harmonyTabBar", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Parameters"))
                    {
                        switch (harmony->type)
                        {
                            case Music::Harmony::Random:            renderRandomHarmony(harmony);           break;
                            case Music::Harmony::Transposition:     renderTranspositionHarmony(harmony);    break;
                            case Music::Harmony::Modulation:        renderModulationHarmony(harmony);       break;
                            case Music::Harmony::Canon:             renderCanonHarmony(harmony);            break;
                            case Music::Harmony::Choral:            renderChoralHarmony(harmony);           break;
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Input"))
                    {
                        std::set<int> rangeKeys;
                        for (int i = harmony->inputRange.low; i <= harmony->inputRange.high; ++i)
                            rangeKeys.insert(i);
                        pushKeyColors(rangeKeys, primaryHighlight);
                        int playedKey = renderOctaves(11, 0, "harmonyModelPiano");
                        popKeyColors(rangeKeys);
                        if (playedKey != -1)
                            harmony->inputRange.moveLow(playedKey);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Output"))
                    {
                        renderMIDIInstrument(harmony->output);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::PopClipRect();
        }
        ImGui::EndChild();
    }

    void UserInterface::constructHarmony(Music::Harmony::Type type, const State::TrackPointer& track)
    {
        Music::HarmonyPointer harmony = nullptr;
        MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
        switch (type)
        {
            case Music::Harmony::Random:
                harmony = std::make_shared<Music::RandomHarmony>(output);
                break;
            case Music::Harmony::Transposition:
                harmony = std::make_shared<Music::TranspositionHarmony>(output);
                break;
            case Music::Harmony::Modulation:
                harmony = std::make_shared<Music::ModulationHarmony>(output, applicationState->song->key);
                break;
            case Music::Harmony::Canon:
                harmony = std::make_shared<Music::CanonHarmony>(output, applicationState->song);
                break;
            case Music::Harmony::Choral:
                harmony = std::make_shared<Music::ChoralHarmony>(output, applicationState->song->key);
                break;
        }
        track->harmonies.push_back(harmony);
        track->clearSelectedHarmonies();
        harmony->selected = true;
    }

    void UserInterface::renderRandomHarmony(const Music::HarmonyPointer& harmony)
    {
        Music::RandomHarmonyPointer randomHarmony = std::static_pointer_cast<Music::RandomHarmony>(harmony);
        if (ImGui::RadioButton("Absolute", randomHarmony->absolute))
            randomHarmony->setAbsolute(true);
        ImGui::SameLine();
        if (ImGui::RadioButton("Relative", !randomHarmony->absolute))
            randomHarmony->setAbsolute(false);

        int minLow = randomHarmony->absolute ? 0 : -127;
        if (ImGui::DragIntRange2("Range", &randomHarmony->low, &randomHarmony->high,
                                 1.f, minLow, 127, "%d", nullptr, ImGuiSliderFlags_AlwaysClamp))
            randomHarmony->reinitialize();
    }

    void UserInterface::renderTranspositionHarmony(const Music::HarmonyPointer& harmony)
    {
        Music::TranspositionHarmonyPointer transpositionHarmony = std::static_pointer_cast<Music::TranspositionHarmony>(harmony);
        ImGui::DragInt("Offset", &transpositionHarmony->transpositionOffset, 1, -127,
                       127, "%d", ImGuiSliderFlags_AlwaysClamp);
    }

    void UserInterface::renderModulationHarmony(const Music::HarmonyPointer& harmony)
    {
        Music::ModulationHarmonyPointer modulationHarmony = std::static_pointer_cast<Music::ModulationHarmony>(harmony);
        ImGui::DragInt("Offset", &modulationHarmony->modulationOffset, 1, -90,
                       90, "%d", ImGuiSliderFlags_AlwaysClamp);
    }

    void UserInterface::renderCanonHarmony(const Music::HarmonyPointer& harmony)
    {
        Music::CanonHarmonyPointer canonHarmony = std::static_pointer_cast<Music::CanonHarmony>(harmony);
        ImGui::SliderInt("##NumberOfBeats", &canonHarmony->numberOfDivisions, 1, 10);
        renderTimeDivisionSelect(canonHarmony->timeDivision);
    }

    void UserInterface::renderChoralHarmony(const Music::HarmonyPointer& harmony)
    {
        Music::ChoralHarmonyPointer choralHarmony = std::static_pointer_cast<Music::ChoralHarmony>(harmony);
        std::string preview = Music::ChoralHarmony::positionName(choralHarmony->position);
        if (ImGui::BeginCombo("##ChoralPosition", preview.c_str()))
        {
            if (ImGui::Selectable("Narrow", choralHarmony->position == Music::ChoralHarmony::Narrow))
                choralHarmony->position = Music::ChoralHarmony::Narrow;
            if (ImGui::Selectable("Wide", choralHarmony->position == Music::ChoralHarmony::Wide))
                choralHarmony->position = Music::ChoralHarmony::Wide;
            ImGui::EndCombo();
        }
    }

    void UserInterface::renderTracks(const ImVec2& trackListPosition, const ImVec2& sequencerPosition,
                                     const ImVec2& harmonyModelPosition, float trackListWidth, float sequencerWidth,
                                     float harmonyModelWidth)
    {
        ImVec2 currentTrackListPosition = trackListPosition + ImVec2(0, headerSize);
        ImVec2 currentSequencerPosition = sequencerPosition + ImVec2(0, headerSize);
        ImVec2 currentHarmonyModelPosition = harmonyModelPosition + ImVec2(0, headerSize);
        float totalWidth = trackListWidth + sequencerWidth + harmonyModelWidth;
        for (const State::TrackPointer& track: applicationState->tracks)
        {
            ImGui::PushID((void*)track.get());

            drawList->AddLine(currentTrackListPosition + ImVec2(0, track->height),
                              currentTrackListPosition + ImVec2(totalWidth, track->height),
                              mainBorder, 1);
            ImGui::SetCursorScreenPos(currentTrackListPosition + ImVec2(0, track->height));
            ImGui::InvisibleButton("trackBorder", ImVec2(totalWidth, 3));
            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (ImGui::IsItemActive())
                track->height = ImGui::GetMousePos().y - currentTrackListPosition.y;

            // TODO: Even out parameter names
            renderTrackListItem(track, currentTrackListPosition, trackListWidth);
            float height = renderTrackSequencer(track, currentSequencerPosition, sequencerWidth);
            renderTrackHarmonyModel(track, currentHarmonyModelPosition,
                                    ImVec2(harmonyModelWidth, track->height));

            currentTrackListPosition.y += height;
            currentSequencerPosition.y += height;
            currentHarmonyModelPosition.y += height;
            ImGui::PopID();
        }
    }

    void UserInterface::renderTrackListItem(const State::TrackPointer& track, const ImVec2& currentPosition,
                                            float availableWidth)
    {
        ImVec2 controlSize = ImVec2(availableWidth, track->height - keyLength - 10);
        ImGui::SetCursorScreenPos(currentPosition);
        if (ImGui::BeginChild("trackControls", controlSize, true,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            renderMIDIInstrument(track->input);
            if (ImGui::Button("Add Take"))
                track->addTake();
        }
        ImGui::EndChild();

        ImGui::SetCursorScreenPos(currentPosition + ImVec2(0, track->height - keyLength - 10));
        ImGui::SetNextWindowContentSize(ImVec2(0, keyLength * 1.05f));
        if (ImGui::BeginChild("trackPianoWindow", ImVec2(availableWidth, keyLength * 1.05f),
                              false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            pianoAction.push([=](const MIDI::MessagePointer& message)
            {
                track->incomingMIDIMessage(std::make_pair(message, track->input));
                director->inputMIDIMessage(std::make_pair(message, track->input));
            });
            keyPressed.push([=](int noteValue){ return track->soundingNotes[noteValue] > 0; });
            // Keep track of which notes have been colored to be able to remove the color later
            std::map<Music::HarmonyPointer, std::set<int>> harmonyNotes;
            for (const Music::HarmonyPointer& harmony: track->harmonies)
            {
                harmony->getSoundingNotes(harmonyNotes[harmony]);
                pushKeyColors(harmonyNotes[harmony], harmonyColors[harmony->type]);
            }
            renderOctaves(11, 0, "trackPiano");
            for (const Music::HarmonyPointer& harmony: track->harmonies)
                popKeyColors(harmonyNotes[harmony]);
            keyPressed.pop();
            pianoAction.pop();
        }
        ImGui::EndChild();
    }

    float UserInterface::renderTrackSequencer(const State::TrackPointer& track, const ImVec2& currentPosition,
                                              float sequencerWidth)
    {
        ImVec2 takeSize = ImVec2(mainAreaSize.x, track->height);
        ImVec2 takePosition = currentPosition;

        renderTakeSequencer(track, track->mainTake, sequencerWidth, takePosition, takeSize);
        for (const State::TakePointer& take: track->takes)
            renderTakeSequencer(track, take, sequencerWidth, takePosition, takeSize);
        return takePosition.y - currentPosition.y;
    }

    void UserInterface::renderTakeSequencer(const State::TrackPointer& track, const State::TakePointer& take,
                                            float sequencerWidth, ImVec2& takePosition, const ImVec2& takeSize)
    {
        bool noteClicked = renderNotes(take, takePosition, takeSize);
        if (ImGui::IsItemClicked() || noteClicked)
        {
//            std::cout << "take button clicked\n";
            if (!noteClicked && track->recordingTake == take)
                take->selectedNotes.clear();
            track->recordingTake = take;
        }
//        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
//        {
//            ImGui::SetCursorScreenPos(takePosition);
//            if (ImGui::Button(" " ICON_FA_TRASH_CAN " "))
//            {
//                std::cout << "delete button clicked\n";
//                track->takes.erase(std::find(track->takes.begin(), track->takes.end(), take));
//            }
//        }

        drawList->AddLine(takePosition + ImVec2(0, track->height),
                          takePosition + ImVec2(sequencerWidth, track->height),
                          mainBorder);
        if (take == track->recordingTake)
            drawList->AddRect(takePosition,
                              takePosition + ImVec2(sequencerWidth + 1, track->height + 1),
                              selectedTakeColor);
        takePosition.y += track->height;
    }

    void UserInterface::renderPiano()
    {
        bool open = true;
//        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
//        ImGui::SetNextWindowSize(ImVec2(140, 100));
        if (ImGui::Begin("Piano", &open))
        {
            renderMIDIInstrument(pianoOutput);
            renderOctaves(5, 3, "pianoKeys");
        }
        ImGui::End();
    }

    int UserInterface::renderOctaves(int numberOfOctaves, int startOctave, const std::string& id)
    {
        int playedKey = -1;
        ImGui::PushID(id.c_str());
        keyTopLeft = ImGui::GetCursorScreenPos();
        for (int octave = startOctave; octave < startOctave + numberOfOctaves; ++octave)
        {
            keySize = ImVec2(keyWidth, keyLength);
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12));
            updatePlayedKey(playedKey, renderPianoKeys(4, octave * 12 + 5));

            updateKeyTopLeft(-keyWidth * 7 + keyWidth / 2);
            keySize = ImVec2(keyWidth, keyLength / 3 * 2);
            updatePlayedKey(playedKey, renderPianoKeys(2, octave * 12 + 1));

            updateKeyTopLeft(keyWidth);
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12 + 6));
            updateKeyTopLeft(keyWidth / 2);
        }
        updateKeyTopLeft(-keyWidth * 7 * static_cast<float>(numberOfOctaves));
        ImGui::SetCursorScreenPos(keyTopLeft + ImVec2(0, keyLength + keyLength / 20));
        ImGui::PopID();
        return playedKey;
    }

    int UserInterface::renderPianoKeys(int numberOfKeys, int keyIndex)
    {
        int playedKey = -1;
        ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
        ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
                                       ImGuiButtonFlags_AllowItemOverlap;
        for (int i = 0; i < numberOfKeys; ++i)
        {
            int noteValue = keyIndex + i * 2;
            ImGui::PushID(noteValue);

            ImGui::InvisibleButton("", keySize, buttonFlags);
            ImGui::SetItemAllowOverlap();

            if (ImGui::IsItemActivated() && ImGui::IsItemHovered())   playNote(noteValue);
            if (ImGui::IsItemDeactivated())                           stopNote(noteValue);
            if (ImGui::IsItemClicked())                               playedKey = noteValue;
            bool active = ImGui::IsItemActive() || keyPressed.top()(noteValue);

            KeyColorPointer keyColor = keyColors[noteValue].top();
            ImU32 currentColor = active ? keyColor->pressed : keyColor->key;
            ImVec2 currentKeySize = active ? keySize + ImVec2(0, keySize.y / 20) : keySize;
            ImVec2 keyBottomRight = keyTopLeft + currentKeySize;

            windowDrawList->AddRectFilled(keyTopLeft, keyBottomRight, currentColor);
            windowDrawList->AddLine(keyTopLeft, keyTopLeft + ImVec2(0, currentKeySize.y), keyColor->border);
            windowDrawList->AddLine(keyTopLeft + ImVec2(currentKeySize.x, 0), keyBottomRight, keyColor->border);

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
        pianoAction.top()(noteOnMessage);
    }

    void UserInterface::stopNote(int noteValue)
    {
        MIDI::NoteOffPointer noteOffMessage = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(noteValue), 64);
        pianoAction.top()(noteOffMessage);
    }

    void UserInterface::playPianoMessage(const MIDI::MessagePointer& message)
    {
        pianoPlayer.processMIDIMessage(message->rawMessage(pianoOutput->channel));
        MIDI::IOManager::getInstance()->sendMIDIOut(std::make_pair(message, pianoOutput));
    }

    void UserInterface::play()
    {
        applicationState->stopRecording();
        if (sequencer->running)     sequencer->stop();
        else                        sequencer->start();
    }

    void UserInterface::record()
    {
        if (!sequencer->running)
        {
            applicationState->recording = true;
            sequencer->start();
        }
    }

    void UserInterface::initializeKeyColors()
    {
        for (int i = 0; i < 11; ++i)
        {
            for (int localValue: {0, 2, 4, 5, 7, 9, 11})
                keyColors[i * 12 + localValue].push(whiteKey);
            for (int localValue: {1, 3, 6, 8, 10})
                keyColors[i * 12 + localValue].push(blackKey);
        }
    }

    MIDI::MessagePointer UserInterface::getLastMessage() const
    {
        return !applicationState->messages.empty() ? applicationState->messages.rbegin()->first : nullptr;
    }

    void UserInterface::pushKeyColors(const std::set<int>& values, const KeyColorPointer& keyColor)
    {
        for (int value: values)
            keyColors[value].push(keyColor);
    }

    void UserInterface::popKeyColors(const std::set<int>& values)
    {
        for (int value: values)
            keyColors[value].pop();
    }

    void UserInterface::renderMIDIInstrument(const MIDI::InstrumentPointer& instrument)
    {
        ImGui::Button(ICON_FA_ARROW_RIGHT_TO_BRACKET " ");
        ImGui::SameLine();
        float availableWidthContent = ImGui::GetContentRegionAvail().x;
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
//        ImGui::BeginGroup();
//        ImGui::BeginGroup();
//        if (availableWidth != 0)
            ImGui::SetNextItemWidth(availableWidthContent * .5f - itemSpacing / 2);
        const char* preview = instrument->port.c_str();
        if (instrument->allPorts) preview = "All";
        if (instrument->noPorts)  preview = "None";
        if (ImGui::BeginCombo("##Port", preview))
        {
            if (ImGui::Selectable("All", instrument->allPorts))
                instrument->setAllPorts();
            if (ImGui::Selectable("None", instrument->noPorts))
                instrument->setNoPorts();
            for (const std::string& outputPort: applicationState->outputPorts)
                if (ImGui::Selectable(outputPort.c_str(), outputPort == instrument->port && instrument->specificPort()))
                    instrument->setPort(outputPort);
            ImGui::EndCombo();
        }
        ImGui::SameLine();

//        if (availableWidth != 0)
            ImGui::SetNextItemWidth(availableWidthContent * .5f - itemSpacing / 2);
        ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
        const char* format = "%d";
        if (instrument->allChannels()) format = "All";
        if (instrument->noChannels())  format = "None";
        ImGui::SliderInt("##Channel", &instrument->channel, 1, CHANNELS_NONE, format, sliderFlags);
//        ImGui::EndGroup();
//        ImVec2 size = ImGui::GetItemRectSize();

        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::BeginCombo("Preset", "Piano"))
        {
            ImGui::EndCombo();
        }
//        ImGui::EndGroup();
//
//        if (availableWidth != 0)
//        {
//            ImGui::SameLine();
//            ImGui::BeginGroup();
//            ImGui::Button("Mute", ImVec2(size.y, size.y));
//            ImGui::SameLine();
//            ImGui::Button("Solo", ImVec2(size.y, size.y));
//            ImGui::Spacing();
//            ImGui::Spacing();
//            ImGui::Button("Mute", ImVec2(size.y, 0));
//            ImGui::SameLine();
//            ImGui::Button("Solo", ImVec2(size.y, 0));
//            ImGui::EndGroup();
//        }
    }

    void UserInterface::renderTimeDivisionSelect(Music::TimeDivision& selectingTimeDivision)
    {
        if (ImGui::BeginCombo("##TimeDivision", Music::allDivisions[selectingTimeDivision].c_str()))
        {
            for (const auto& [timeDivision, name]: Music::allDivisions)
            {
                ImGui::PushID(("time division" + name).c_str());
                if (ImGui::Selectable(name.c_str(), timeDivision == selectingTimeDivision))
                    selectingTimeDivision = timeDivision;
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }
}