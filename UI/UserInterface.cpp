//
// Created by Casper De Smet on 22/02/2022.
//

#include <sstream>
#include <fstream>
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
    UserInterface::UserInterface(const State::ApplicationPointer& applicationState, const State::TrackPointer& initialTrack,
                                 const Music::DirectorPointer& director, const System::SequencerPointer& sequencer):
                                 applicationState(applicationState), director(director),
                                 sequencer(sequencer), selectedTrack(initialTrack)
    {
        readPresets();
        pianoPlayer = std::make_shared<MIDI::AudioPlayer>(applicationState);
        previousLastMessage = getLastMessage();
        pianoOutput = applicationState->selectInstrument(MIDI::Output);

        showMessageMonitor = false;

        controlHeight = 102;
        trackListRatio = 1.f / 5.f;
        sequencerRatio = 2.f / 5.f;
        harmonyRatio = 2.f / 5.f;

        measureWidth = 60;
        headerSize = 18;
        scroll = headerSize / 2;
        keyWidth.push(10);
        keyHeight.push(50);

        ImU32 yellow = ImColor(255, 217, 0);

        mainBackground = ImColor(40, 40, 40);                                   // Dark grey
        darkBackground = ImColor(35, 35, 35);                                   // Dark grey
        mainBorder = ImColor(255, 255, 255);                                    // White
        darkBorder = ImColor(25, 25, 25);                                       // Very dark grey
        harmonyHeaderColor = ImColor(15, 158, 206);                             // Light blue
        tracklistHeaderColor = ImColor(226, 65, 86);                            // Pinkish

        divisionColor = ImColor(255, 255, 255, 150);                         // Transparent white
        playingColor = ImColor(255, 255, 255);                                  // White
        playingColorTransparent = ImColor(255, 255, 255, 50);                // Transparent white
        recordingColor = ImColor(232, 9, 9);                                    // Red
        recordingColorTransparent = ImColor(232, 9, 9, 50);                  // Transparent red

        cycleColor = yellow;
        metronomeColor = ImColor(122, 0, 255);                                  // Purple

        mutedColor = std::make_shared<ColorRange>((ImU32) ImColor(255, 29, 0)); // Red
        soloColor = std::make_shared<ColorRange>(yellow);

        selectedTrackColor = yellow;
        selectedTakeColor = yellow;

        whiteKey = std::make_shared<ColorRange>(ImColor(255, 255, 255), ImColor(200, 200, 200),
                                                ColorRange::Light);
        blackKey = std::make_shared<ColorRange>(ImColor(0, 0, 0), ImColor(80, 80, 80),
                                                ColorRange::Dark);

        harmonyColors = {{Music::Harmony::Random, std::make_shared<ColorRange>(33, 72, 252)},           // Blue
                         {Music::Harmony::Transposition, std::make_shared<ColorRange>(240, 65, 23)},    // Red
                         {Music::Harmony::Modulation, std::make_shared<ColorRange>(109, 42, 170)},      // Purple
                         {Music::Harmony::Canon, std::make_shared<ColorRange>(yellow, ColorRange::Light)},
                         {Music::Harmony::Choral, std::make_shared<ColorRange>(21, 173, 4)}};           // Green

        primaryHighlight = harmonyColors[Music::Harmony::Transposition];
        secondaryHighlight = harmonyColors[Music::Harmony::Random];

        pianoAction.push([&](const MIDI::MessagePointer& message){ playPianoMessage(message); });
        keyPressed.push([](int noteValue){ return false; });
        initializeKeyColors();

        songDataType = Key;
    }

    // Start the ImGui backend
    void UserInterface::start()
    {
        Renderer renderer = Renderer([&](){ this->renderUI(); }, "MIDI Assistant");
        renderer.start(defaultFont, mediumFont, largeFont);
    }

    // Render windows
    void UserInterface::renderUI()
    {
        renderMessageMonitor();
        renderMainWindow();
    }

    // Read the piano presets from file
    void UserInterface::readPresets()
    {
        int index = 0;
        std::string preset;
        std::ifstream file("./Assets/presets.txt");
        while (std::getline(file, preset))
            presets[index++] = preset;
    }

    // Show all incoming MIDI messages
    void UserInterface::renderMessageMonitor()
    {
        if (!showMessageMonitor)
            return;

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

            // Compute positions and sizes
            ImVec2 controlSize = ImVec2(canvasSize.x, controlHeight);
            ImVec2 tracklistSize = ImVec2(canvasSize.x * trackListRatio, canvasSize.y - controlHeight);
            ImVec2 sequencerSize = ImVec2(canvasSize.x * sequencerRatio, canvasSize.y - controlHeight);
            ImVec2 harmonyModelSize = ImVec2(canvasSize.x * harmonyRatio, canvasSize.y  - controlHeight);

            ImVec2 controlPosition = canvasPosition;
            ImVec2 tracklistPosition = canvasPosition + ImVec2(0, controlSize.y);
            ImVec2 sequencerPosition = tracklistPosition + ImVec2(tracklistSize.x, 0);
            ImVec2 harmonyPosition = sequencerPosition + ImVec2(sequencerSize.x, 0);

            // Render all areas
            float ticksPerBeat;
            applicationState->song->computeMeasureLength(divisionsPerBeat, ticksPerBeat, measureLength);
            renderControl(controlPosition, controlSize);
            renderTrackList(tracklistPosition, tracklistSize);
            renderSequencer(sequencerPosition, sequencerSize);
            renderHarmonyModel(harmonyPosition, harmonyModelSize);

            // Resize buttons
            drawList->AddLine(controlPosition + ImVec2(0, controlSize.y), controlPosition + controlSize, mainBorder);
            ImGui::PushID("sequencerResize");
            float sequencerResize = renderResizableBorder(sequencerPosition, sequencerSize.y, false);
            ImGui::PopID();
            ImGui::PushID("harmonyResize");
            float harmonyResize = renderResizableBorder(sequencerPosition + ImVec2(sequencerSize.x, 0),
                                                        sequencerSize.y, false);
            ImGui::PopID();

            renderTracks(tracklistPosition, sequencerPosition, harmonyPosition,
                         tracklistSize.x, sequencerSize.x, harmonyModelSize.x);
            // Update sizes
            sequencerPosition.x += sequencerResize;
            harmonyPosition.x += harmonyResize;
            trackListRatio = sequencerPosition.x / canvasSize.x;
            sequencerRatio = (harmonyPosition.x - sequencerPosition.x) / canvasSize.x;
            harmonyRatio = 1 - harmonyPosition.x / canvasSize.x;
        }
        ImGui::End();
    }

    // Render full top control area
    void UserInterface::renderControl(const ImVec2& controlPosition, const ImVec2& controlSize)
    {
        drawList->AddRectFilled(controlPosition, controlPosition + controlSize, mainBackground);

        // Compute positions and sizes
        ImVec2 trackControlsPosition = controlPosition;
        ImVec2 controlBarPosition = controlPosition + ImVec2(controlSize.x * trackListRatio, 0);
        ImVec2 harmonySourcePosition = controlPosition + ImVec2(controlSize.x * (1 - harmonyRatio), 0);

        ImVec2 trackControlsSize = controlSize * ImVec2(trackListRatio, 1);
        ImVec2 controlBarSize = controlSize * ImVec2(sequencerRatio, 1);
        ImVec2 harmonySourceSize = controlSize * ImVec2(harmonyRatio, 1);

        // Render three areas
        renderTrackControls(trackControlsPosition, trackControlsSize);
        renderControlBar(controlBarPosition, controlBarSize);
        renderHarmonySource(harmonySourcePosition, harmonySourceSize);
    }

    // Render buttons for creating and deleting tracks, plus showing midi message monitor
    void UserInterface::renderTrackControls(const ImVec2& trackControlsPosition, const ImVec2& trackControlsSize)
    {
        ImGui::SetCursorScreenPos(trackControlsPosition);
        if (ImGui::BeginChild("trackControls", trackControlsSize, true, ImGuiWindowFlags_NoBackground))
        {
            if (ImGui::Button("Add Track"))
            {
                MIDI::InstrumentPointer input = applicationState->selectInstrument(MIDI::Input);
                MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
                State::TrackPointer track = std::make_shared<State::Track>(applicationState, input, output);
                applicationState->tracks.emplace_back(track);
                selectedTrack = track;
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Track"))
                selectedTrack = applicationState->deleteTrack(selectedTrack);
            ImGui::Checkbox("MIDI Message Monitor", &showMessageMonitor);
        }
        ImGui::EndChild();
    }

    // Render middle part of control, containing song data and sequencer controls
    void UserInterface::renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize)
    {
        ImGui::SetCursorScreenPos(controlBarPosition);
        if (ImGui::BeginChild("innerControl", ImVec2(controlBarSize.x, 102), true,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::BeginGroup();
            ImGui::PushFont(*largeFont);
            renderSongData();
            ImGui::Spacing();
            ImGui::PopFont();

            ImGui::PushFont(*mediumFont);
            renderSequencerControls();
            ImGui::PopFont();
            ImGui::EndGroup();

            ImGui::SameLine();
            if (ImGui::BeginChild("songEdit"))
                songEdit();
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }

    // Render all information of the song
    void UserInterface::renderSongData()
    {
        State::SongPointer song = applicationState->song;

        ImVec2 position = ImGui::GetCursorScreenPos();
        ImGui::BeginGroup();
        ImGui::Spacing();   ImGui::Spacing();   ImGui::Spacing();   ImGui::SameLine();

        // All parts of the song return a function which renders a widget in which this data can be edited
        ImGui::PushFont(largeFont->equalSpacedFont);
        SongEdit timeEdit = renderTime(song, applicationState->currentTime, applicationState->currentTime + 1, true);
        // If the display of this part of the song is selected (because it was clicked), the edit widget render function
        // is set to the function returned by the render function of this part of the song
        if (songDataType == Time) songEdit = timeEdit;
        ImGui::PopFont();
        ImGui::SameLine();

        SongEdit signatureEdit = renderSignature(song);
        if (songDataType == Signature) songEdit = signatureEdit;
        ImGui::SameLine();

        SongEdit tempoEdit = renderTempo();
        if (songDataType == Tempo) songEdit = tempoEdit;
        ImGui::SameLine();

        SongEdit keyEdit = renderKey(song->key);
        if (songDataType == Key) songEdit = keyEdit;
        ImGui::SameLine();

        ImGui::SameLine();  ImGui::Spacing();   ImGui::Spacing();   ImGui::Spacing();
        ImGui::EndGroup();
        drawList->AddRect(position, position + ImGui::GetItemRectSize(), darkBorder,
                          10, ImDrawFlags_None, 3);
    }

    // Render the current time
    SongEdit UserInterface::renderTime(const State::SongPointer& song, int& editTime, int displayTime, bool controlBar)
    {
        ImGui::BeginGroup();
        // Divide the current time up into measures, beats, divisions and ticks by dividing the total number of ticks
        float timeInMeasures = static_cast<float>(displayTime) / measureLength;

        // Every time element gets padded with leading zeroes
        // The render function returns a function which renders an input int widget
        int currentMeasures = static_cast<int>(floor(timeInMeasures));
        TimeEdit measureEdit = renderPaddedTimeElement("measures", currentMeasures, 9999, controlBar);

        int currentBeats = static_cast<int>(floor(timeInMeasures * static_cast<float>(song->numberOfBeats)));
        TimeEdit beatEdit = renderPaddedTimeElement("beats", currentBeats, song->numberOfBeats, controlBar);

        TimeEdit divisionEdit = [&](float availableWidth){ return 0; };
        if (divisionsPerBeat > 1)
        {
            float numberOfDivisions = static_cast<float>(song->timeDivision * song->numberOfBeats);
            int currentDivisions = static_cast<int>(floor(timeInMeasures * numberOfDivisions));
            divisionEdit = renderPaddedTimeElement("divisions", currentDivisions,
                                                   static_cast<int>(divisionsPerBeat), controlBar);
        }

        int ticksPerDivision = applicationState->song->ticksPerDivision;
        TimeEdit tickEdit = renderPaddedTimeElement("ticks", displayTime, ticksPerDivision, controlBar);
        ImGui::EndGroup();
        if (controlBar && ImGui::IsItemClicked(ImGuiMouseButton_Left))
            songDataType = Time;

        // All edit widgets get joined together into a single edit widget
        return [&, measureEdit, beatEdit, divisionEdit, tickEdit]()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
            float availableWidth = getSpacedAvailableWidth(2);
            int measures = measureEdit(availableWidth);
            ImGui::SameLine();
            int beats = beatEdit(availableWidth);
            int divisions = divisionEdit(availableWidth);
            ImGui::SameLine();
            int ticks = tickEdit(availableWidth);

            if (measures || beats || divisions || ticks)
                adjustTime(editTime, measures, beats, divisions, ticks);
            ImGui::PopStyleVar();
        };
    }

    // Add an offset to the current time based on how much the parts of the time were edited
    void UserInterface::adjustTime(int& currentTime, int measures, int beats, int divisions, int ticks)
    {
        int ticksPerDivision = applicationState->song->ticksPerDivision;
        int measureTicks = measures * static_cast<int>(measureLength);
        int beatTicks = beats * static_cast<int>(divisionsPerBeat) * ticksPerDivision;
        int divisionTicks = divisions * ticksPerDivision;
        int offset = measureTicks + beatTicks + divisionTicks + ticks;
        currentTime = Utilities::bottomClamp(applicationState->currentTime + offset, -1);
    }

    // Pad the value with a number of leading zeroes and return a function rendering an input int widget
    TimeEdit UserInterface::renderPaddedTimeElement(const std::string& id, int value, int total, bool controlBar)
    {
        int displayValue = value % total + (int) controlBar;
        int paddingLength = Utilities::numberOfDigits(total) - Utilities::numberOfDigits(displayValue);
        ImGui::SetNextItemWidth(100);
        ImGui::TextDisabled("%s", std::string(paddingLength, '0').c_str());
        ImGui::SameLine(0.f, 0.f);
        ImGui::Text("%d", displayValue);
        ImGui::SameLine();

        return [&, displayValue, id](float availableWidth)
        {
            bool entered = false;
            int editValue = displayValue;
            ImGui::BeginGroup();
            ImGui::TextUnformatted(id.c_str());
            ImGui::SetNextItemWidth(availableWidth);
            ImGui::InputInt(("##" + id).c_str(), &editValue);
            entered = ImGui::IsItemDeactivatedAfterEdit();
            ImGui::EndGroup();
            return entered ? editValue - displayValue : 0;
        };
    }

    // Render the time signature of the song and return a function rendering an edit widget
    SongEdit UserInterface::renderSignature(const State::SongPointer& song)
    {
        ImGui::Text("%d/%d", song->numberOfBeats, song->timeDivision);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            songDataType = Signature;
        return [&, song, this]()
        {
            ImGui::PushFont(*mediumFont);
            ImGui::SliderInt("##NumberOfBeats", &song->numberOfBeats, 1, 10);
            renderTimeDivisionSelect(song->timeDivision);
            ImGui::PopFont();
        };
    }

    // Render the tempo of the song and return a function rendering an edit widget
    SongEdit UserInterface::renderTempo()
    {
        std::stringstream tempoString;
        double tempo = round(60.f * 1000.f * 1000.f / static_cast<double>(applicationState->song->tempo) * 1000.f) / 1000.f;
        tempoString.precision(7);
        tempoString << tempo;
        ImGui::TextUnformatted(tempoString.str().c_str());
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            songDataType = Tempo;

        return [&, tempo, this]()
        {
            double editTempo = tempo;
            ImGui::PushFont(*mediumFont);
            ImGui::InputDouble("##tempo", &editTempo);
            if (ImGui::IsItemDeactivatedAfterEdit())
                applicationState->song->setTempo(static_cast<float>(editTempo));
            ImGui::PopFont();
        };
    }

    // Render the key of the song and return a function rendering an edit widget
    SongEdit UserInterface::renderKey(const Music::KeyPointer& key)
    {
        ImGui::BeginGroup();
        ImGui::TextUnformatted(key->rootNote->toString().c_str());
        ImGui::OpenPopupOnItemClick("keyEdit", ImGuiPopupFlags_MouseButtonLeft);
        ImGui::SameLine(0.f, 2.f);
        ImGui::TextUnformatted(Music::IntervalSequence::sequenceName(key->intervalSequence->type).c_str());
        ImGui::EndGroup();
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            songDataType = Key;

        return [&]()
        {
            ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders |
                                         ImGuiTableFlags_RowBg |
                                         ImGuiTableFlags_ScrollY;
            float width = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x - 7 * (keySize.x + 1);
            // Render a table containing the modes and scales
            if (ImGui::BeginTable("intervalSequenceTable", 2, tableFlags, ImVec2(width, 0)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
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
            ImGui::SameLine();

            // Render an octave highlighting the root note
            int startOctave = 5;
            const auto& [name, sharp] = *key->rootNote;
            int rootNoteValue = Music::RootNote::convert(name, sharp) + startOctave * 12;
            pushKeyColors({rootNoteValue}, primaryHighlight);
            int playedKey = renderOctaves(1, startOctave, "keyRootNoteSelection");
            popKeyColors({rootNoteValue});
            if (playedKey != -1)
                key->setRootNote(Music::RootNote::convert(playedKey % 12));
        };
    };

    // Render a number of buttons with which the sequencer can be controlled
    void UserInterface::renderSequencerControls()
    {
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(" " ICON_FA_BACKWARD " "))
            adjustTime(applicationState->currentTime, -1, 0, 0, 0);
        ImGui::SameLine(0.f, 1.f);

        if (ImGui::Button(" " ICON_FA_FORWARD " "))
            adjustTime(applicationState->currentTime, 1, 0, 0, 0);
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

    // Render a button with a pause icon when the sequencer is playing and a play button otherwise
    bool UserInterface::renderPlayButton()
    {
        if (sequencer->running) return ImGui::Button(" " ICON_FA_PAUSE " ");
        else                    return ImGui::Button(" " ICON_FA_PLAY " ");
    }

    // Render a button that is colored when active and white otherwise
    bool UserInterface::renderColoredControlButton(const std::string& icon, bool colored, ImU32 color)
    {
        if (colored) ImGui::PushStyleColor(ImGuiCol_Text, color);
        bool clicked = ImGui::Button((" " + icon + " ").c_str());
        if (colored) ImGui::PopStyleColor();
        return clicked;
    }

    // Render the possible harmonies which can be dragged to a harmony model in order to create it
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
                drawList->AddRectFilled(position, position + size, *harmonyColors[harmony]);

                ImGui::PushStyleColor(ImGuiCol_Text, harmonyColors[harmony]->text);
                ImGui::TextUnformatted(harmonyName.c_str());
                ImGui::PopStyleColor();
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

    // Render the colored bar above the tracklist
    void UserInterface::renderTrackList(const ImVec2& tracklistPosition, const ImVec2& tracklistSize)
    {
        drawList->AddRectFilled(tracklistPosition, tracklistPosition + tracklistSize, mainBackground);
        drawList->AddRectFilled(tracklistPosition, tracklistPosition + ImVec2(tracklistSize.x, headerSize),
                                tracklistHeaderColor);
        drawList->AddLine(tracklistPosition + ImVec2(0, headerSize),
                          tracklistPosition + ImVec2(tracklistSize.x, headerSize),
                          mainBorder);
    }

    // Compute the x position of a tick given the current scroll in the sequencer
    float UserInterface::computeXPosition(int ticks)
    {
        return scroll + static_cast<float>(ticks) / measureLength * measureWidth;
    }

    // Compute the y position of a note relative to the size of the track
    float UserInterface::computeYPosition(const ImVec2& trackSize, const State::TakePointer& take, int value)
    {
        int highestNote = take->highestNote == -1 ? 64 : take->highestNote;
        int lowestNote = take->lowestNote == 128 ? 64 : take->lowestNote;
        int range = Utilities::clamp(highestNote - lowestNote + 24, 0, 127 + 24);
        int rangedValue = value - lowestNote + 12;
        return trackSize.y / static_cast<float>(range) * static_cast<float>(range - rangedValue);
    }

    // Convert difference in position to difference in ticks
    int UserInterface::computeTickDelta(float positionDelta)
    {
        return static_cast<int>(floor(positionDelta / measureWidth * measureLength));
    }

    // Render everything that is part of the overall sequencer (grid, playhead, ...)
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

    // Render the vertical lines that make up the grid + the header containing the extra subdivisions
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

    // Render all notes of a take
    bool UserInterface::renderNotes(const State::TakePointer& take, const ImVec2& takePosition, const ImVec2& takeSize)
    {
        bool noteClicked = false;
        ImGui::PushID((void*)take.get());
        ImGui::PushClipRect(takePosition, takePosition + takeSize, false);
        std::unique_lock<std::mutex> lock(take->mutex);
        for (const auto& [tick, messages]: take->midiMessages)
            for (const MIDI::MessagePointer& message: messages)
                // Draw only the note on messages, connecting them with the note off messages
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
                    // Make the notes clickable
                    if (ImGui::InvisibleButton("", ImVec2(endX, endY) - ImVec2(startX, startY)))
                        take->selectNote(noteOn, ImGui::IsKeyDown(ImGuiKey_ModShift));
                    noteClicked = noteClicked || ImGui::IsItemClicked();
                    ImGui::PopID();
                }
        ImGui::PopClipRect();

        // Make the whole take clickable
        ImGui::SetCursorScreenPos(takePosition);
        ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_AllowItemOverlap;
        ImGui::InvisibleButton("takeButton", takeSize, flags);
        ImGui::SetItemAllowOverlap();
        ImGui::PopID();
        return noteClicked;
    }

    // Render the playhead in the current position
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

    // Render the necessary invisible buttons to make the sequencer interactible
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

    // Catch key events to interact with the sequencer
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

    // If notes are selected in the selected tracks selected take, delete them
    // If no notes are selected, delete the selected take of the selected track
    // If the selected track only has one take, delete the whole track
    void UserInterface::deleteItems()
    {
        if (!selectedTrack->recordingTake->selectedNotes.empty())
            selectedTrack->recordingTake->deleteSelectedNotes();
        else if (selectedTrack->takes.size() > 1)
            selectedTrack->deleteTake();
        else if (applicationState->tracks.size() > 1)
            selectedTrack = applicationState->deleteTrack(selectedTrack);
    }

    // Render the header above the harmony model
    void UserInterface::renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize)
    {
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + harmonyModelSize, mainBackground);
        drawList->AddRectFilled(harmonyPosition, harmonyPosition + ImVec2(harmonyModelSize.x, headerSize), harmonyHeaderColor);
        drawList->AddLine(harmonyPosition + ImVec2(0, headerSize),
                          harmonyPosition + ImVec2(harmonyModelSize.x, headerSize), mainBorder);
    }

    // Render the harmony model of a track, containing all harmonies
    void UserInterface::renderTrackHarmonyModel(const State::TrackPointer& track, const ImVec2& position,
                                                const ImVec2& size)
    {
        drawList->AddLine(position, position + ImVec2(size.x, 0), mainBorder);
        ImGui::PushID((void*)track.get());
        ImGui::SetCursorScreenPos(position);
        ImGui::InvisibleButton("harmonyModel", size);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Harmony", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                Music::Harmony::Type harmony = *(const Music::Harmony::Type*)payload->Data;
                constructHarmony(harmony, track);
            }
            ImGui::EndDragDropTarget();
        }

        // If no harmony is selected, render the harmonies side by side
        if (track->hasSelectedHarmonies() && track->harmonies.size() > 1)
            renderSelectedHarmonyView(track, position, size);
        else
            renderDefaultHarmonyView(track, position, size);
        ImGui::PopID();
    }

    // Render the harmonies side by side, equally sized
    void UserInterface::renderDefaultHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                                 const ImVec2& trackHarmonySize)
    {
        Music::HarmonyPointer deletedHarmony = nullptr;
        ImVec2 currentPosition = trackHarmonyPosition;
        for (const Music::HarmonyPointer& harmony: track->harmonies)
        {
            float numberOfHarmonies = static_cast<float>(track->harmonies.size());
            ImGui::PushID((void*)harmony.get());
            if (renderOpenTrackHarmony(track, harmony, currentPosition,
                                       trackHarmonySize / ImVec2(numberOfHarmonies, 1)))
                deletedHarmony = harmony;
            currentPosition.x += trackHarmonySize.x / numberOfHarmonies;
            ImGui::PopID();
        }
        if (deletedHarmony)
            track->deleteHarmony(deletedHarmony);
    }

    // Render the selected harmony full size and the rest in a smaller listing to the side
    void UserInterface::renderSelectedHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                                 const ImVec2& trackHarmonySize)
    {
        float selectedRatio = 2.f / 3.f;
        Music::HarmonyPointer deletedHarmony = nullptr;
        ImVec2 position = trackHarmonyPosition + ImVec2(trackHarmonySize.x * selectedRatio, 0);
        auto selected = [=](const Music::HarmonyPointer& harmony) { return harmony->selected; };
        Music::HarmonyPointer selectedHarmony = *std::find_if(track->harmonies.begin(), track->harmonies.end(), selected);
        float numberOfHarmonies = static_cast<float>(track->harmonies.size()) - 1;

        ImVec2 size = ImVec2(trackHarmonySize.x * (1 - selectedRatio), trackHarmonySize.y);
        ImGui::SetCursorScreenPos(position);
        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding / 2);
        float totalHeight = trackHarmonySize.y - windowPadding.y - (numberOfHarmonies - 1) * ImGui::GetStyle().ItemSpacing.y;
        if (ImGui::BeginChild("unselectedHarmonyList", size, true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::PopStyleVar();
            ImGui::PushClipRect(position, position + size, true);
            for (const Music::HarmonyPointer& harmony: track->harmonies)
            {
                ImGui::PushID((void*)harmony.get());
                if (harmony != selectedHarmony)
                {
                    // Ensure title is always readable
                    ImVec2 minimumSize = ImGui::CalcTextSize(harmony->typeName.c_str());
                    float harmonyHeight = totalHeight / numberOfHarmonies;
                    harmonyHeight = std::max(minimumSize.y + 2 * windowPadding.y, harmonyHeight);
                    if (renderClosedTrackHarmony(track, harmony, harmonyHeight))
                        deletedHarmony = harmony;
                }
                ImGui::PopID();
            }
            ImGui::PopClipRect();
        }
        ImGui::EndChild();

        // Render selected harmony
        ImVec2 selectedHarmonySize = trackHarmonySize * ImVec2(selectedRatio, 1);
        if (renderOpenTrackHarmony(track, selectedHarmony, trackHarmonyPosition, selectedHarmonySize))
            deletedHarmony = selectedHarmony;

        if (deletedHarmony)
            track->deleteHarmony(deletedHarmony);
    }

    // Render a full size harmony, showing parameters and input tabs
    bool UserInterface::renderOpenTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                               const ImVec2& trackHarmonyPosition, const ImVec2& trackHarmonySize)
    {
        bool deleted;
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        ImGui::SetCursorScreenPos(trackHarmonyPosition);
        if (ImGui::BeginChild("harmony", trackHarmonySize, true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            ImVec2 available = ImGui::GetContentRegionAvail();
            ImVec2 position = trackHarmonyPosition + padding / 2;
            ImVec2 size = available + padding;

            drawList->AddRect(position, position + size, *harmonyColors[harmony->type],
                                    0.f, ImDrawFlags_None, 2.f);
            ImGui::PushClipRect(position + padding / 2, position + size - padding / 2, true);
            deleted = renderTrackHarmonyName(track, harmony, ImGui::GetColorU32(ImGuiCol_Text));
            if (ImGui::BeginTabBar("harmonyTabBar", ImGuiTabBarFlags_None))
            {
                // Render harmony specific parameters
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
                if (ImGui::BeginTabItem("Output"))
                {
                    renderMIDIInstrument(harmony->output, "Output");
                    renderAudioPlayer(harmony->audioPlayer);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::PopClipRect();
        }
        ImGui::EndChild();
        return deleted;
    }

    // Render a filled in harmony, displaying the title
    bool UserInterface::renderClosedTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                                 float trackHarmonyHeight)
    {
        ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
        ImVec2 position = ImGui::GetCursorScreenPos();
        bool deleted = false;
        if (ImGui::BeginChild("harmony", ImVec2(0, trackHarmonyHeight),
                              true, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
            deleted = renderTrackHarmonyName(track, harmony, harmonyColors[harmony->type]->text);
        ImGui::EndChild();
        windowDrawList->AddRectFilled(position, position + ImGui::GetItemRectSize(), *harmonyColors[harmony->type]);
        return deleted;
    }

    // Render the name of the harmony and make it clickable to select it
    bool UserInterface::renderTrackHarmonyName(const State::TrackPointer& track, const Music::HarmonyPointer& harmony, ImU32 color)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(harmony->typeName.c_str());
        bool selected = harmony->selected;
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked())
        {
            track->clearSelectedHarmonies();
            harmony->selected = !selected;
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(ICON_FA_TRASH_CAN);
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::PopStyleColor();
        return ImGui::IsItemClicked();
    }

    // Construct a harmony of a specific type
    void UserInterface::constructHarmony(Music::Harmony::Type type, const State::TrackPointer& track)
    {
        Music::HarmonyPointer harmony = nullptr;
        MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
        switch (type)
        {
            case Music::Harmony::Random:
                harmony = std::make_shared<Music::RandomHarmony>(output, applicationState);
                break;
            case Music::Harmony::Transposition:
                harmony = std::make_shared<Music::TranspositionHarmony>(output, applicationState);
                break;
            case Music::Harmony::Modulation:
                harmony = std::make_shared<Music::ModulationHarmony>(output, applicationState);
                break;
            case Music::Harmony::Canon:
                harmony = std::make_shared<Music::CanonHarmony>(output, applicationState);
                break;
            case Music::Harmony::Choral:
                harmony = std::make_shared<Music::ChoralHarmony>(output, applicationState);
                break;
        }
        track->harmonies.push_back(harmony);
        track->clearSelectedHarmonies();
        harmony->selected = true;
    }

    // Random the parameters for the random harmony
    void UserInterface::renderRandomHarmony(const Music::HarmonyPointer& harmony)
    {
        // Cast the harmony to an object of the correct subtype in order to access the parameters
        Music::RandomHarmonyPointer randomHarmony = std::static_pointer_cast<Music::RandomHarmony>(harmony);
        // Absolute or relative note generation
        if (ImGui::RadioButton("Absolute", randomHarmony->absolute))
            randomHarmony->setAbsolute(true);
        ImGui::SameLine();
        if (ImGui::RadioButton("Relative", !randomHarmony->absolute))
            randomHarmony->setAbsolute(false);

        // Range in which random note/offset will be generated
        int minLow = randomHarmony->absolute ? 0 : -127;
        if (ImGui::DragIntRange2("Range", &randomHarmony->low, &randomHarmony->high,
                                 1.f, minLow, 127, "%d", nullptr, ImGuiSliderFlags_AlwaysClamp))
            randomHarmony->reinitialize();
    }

    // Render the parameters for the transposition harmony
    void UserInterface::renderTranspositionHarmony(const Music::HarmonyPointer& harmony)
    {
        // Cast the harmony to an object of the correct subtype in order to access the parameters
        Music::TranspositionHarmonyPointer transpositionHarmony = std::static_pointer_cast<Music::TranspositionHarmony>(harmony);
        // Offset for note generation
        ImGui::DragInt("Offset", &transpositionHarmony->transpositionOffset, 1, -127,
                       127, "%d semitones", ImGuiSliderFlags_AlwaysClamp);
    }

    // Render the parameters for the modulation harmony
    void UserInterface::renderModulationHarmony(const Music::HarmonyPointer& harmony)
    {
        // Cast the harmony to an object of the correct subtype in order to access the parameters
        Music::ModulationHarmonyPointer modulationHarmony = std::static_pointer_cast<Music::ModulationHarmony>(harmony);
        // Degree with which the note should be modulated
        std::string modulation = Music::Key::degreeName(modulationHarmony->modulation);
        ImGui::SliderInt("Modulation", reinterpret_cast<int*>(&modulationHarmony->modulation), 0, 7, modulation.c_str());
        // Number of octaves to modulate with
        ImGui::DragInt("Octaves", &modulationHarmony->octaves, 1, -11, 11);
        // Modulate up or down
        std::string direction = modulationHarmony->up ? "Up" : "Down";
        ImGui::SliderInt("Direction", reinterpret_cast<int*>(&modulationHarmony->up), 0, 1, direction.c_str());
    }

    // Render the parameters for the canon harmony
    void UserInterface::renderCanonHarmony(const Music::HarmonyPointer& harmony)
    {
        // Cast the harmony to an object of the correct subtype in order to access the parameters
        Music::CanonHarmonyPointer canonHarmony = std::static_pointer_cast<Music::CanonHarmony>(harmony);
        // Number of beats to wait before echoing note
        ImGui::SliderInt("##NumberOfBeats", &canonHarmony->numberOfDivisions, 1, 10);
        renderTimeDivisionSelect(canonHarmony->timeDivision);
    }

    // Render the parameters for the choral harmony
    void UserInterface::renderChoralHarmony(const Music::HarmonyPointer& harmony)
    {
        // Cast the harmony to an object of the correct subtype in order to access the parameters
        Music::ChoralHarmonyPointer choralHarmony = std::static_pointer_cast<Music::ChoralHarmony>(harmony);
        // Use the narrow or wide position
        std::string position = Music::ChoralHarmony::positionName(choralHarmony->position);
        ImGui::SliderInt("Position", reinterpret_cast<int*>(&choralHarmony->position), 1, 2, position.c_str());
    }

    // Render everything that is specific to the tracks across the track list, sequencer and harmony model
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

            float trackHeight = track->recordingTake == *track->takes.begin() ? track->height + keyHeight.top() : track->height;

            // Make the track resizable
            track->height += renderResizableBorder(currentTrackListPosition + ImVec2(0, trackHeight),
                                                   totalWidth, true);

            renderTrackListItem(track, currentTrackListPosition, ImVec2(trackListWidth, trackHeight));
            float height = renderTrackSequencer(track, currentSequencerPosition, ImVec2(sequencerWidth, trackHeight));
            renderTrackHarmonyModel(track, currentHarmonyModelPosition, ImVec2(harmonyModelWidth, trackHeight));

            currentTrackListPosition.y += height;
            currentSequencerPosition.y += height;
            currentHarmonyModelPosition.y += height;
            ImGui::PopID();
        }
    }

    // Render the controls for this specific track
    void UserInterface::renderTrackListItem(const State::TrackPointer& track, const ImVec2& position, const ImVec2& size)
    {
        float indicatorSize = 8;
        ImVec2 listItemSize = ImVec2(size.x - indicatorSize, size.y);
        ImGui::SetCursorScreenPos(position + ImVec2(indicatorSize, 0));
        drawList->AddLine(position, position + ImVec2(size.x, 0), mainBorder);
        drawList->AddLine(position + ImVec2(indicatorSize, 0),
                          position + ImVec2(indicatorSize, listItemSize.y),
                          mainBorder);
        if (track == selectedTrack)
            drawList->AddRectFilled(position + ImVec2(0, 1),
                                    position + ImVec2(indicatorSize, listItemSize.y),
                                    selectedTrackColor);
        if (ImGui::BeginChild("trackControls", listItemSize, true,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            // IO
            renderMIDIInstrument(track->input, "Input");
            renderMIDIInstrument(track->output, "Output");
            renderAudioPlayer(track->audioPlayer);
            ImGui::Spacing();

            // Take management
            float innerAvailableWidth = getSpacedAvailableWidth(2);
            ImGui::Spacing();
            if (ImGui::Button("Add Take", ImVec2(innerAvailableWidth, 0)))
            {
                track->addTake();
                selectedTrack = track;
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Take", ImVec2(innerAvailableWidth, 0)))
                track->deleteTake();
            ImGui::Spacing();

            // Quantization
            ImGui::Spacing();
            renderQuantization(track);
        }
        ImGui::EndChild();
        if (ImGui::IsItemClicked())
            selectedTrack = track;
    }

    // Render the controls needed to quantize the selected track
    void UserInterface::renderQuantization(const State::TrackPointer& track)
    {
        ImGui::SetNextItemWidth(getSpacedAvailableWidth());
        std::string preview = State::Track::quantizationName(track->quantization);
        if (ImGui::BeginCombo("##quantizationTypes", preview.c_str()))
        {
            for (int i = 0; i < 3; ++i)
            {
                std::string quantization = State::Track::quantizationName((State::Track::Quantization) i);
                if (ImGui::Selectable(quantization.c_str(), track->quantization == i))
                    track->quantization = (State::Track::Quantization) i;
            }
            ImGui::EndCombo();
        }
        switch (track->quantization)
        {
            case State::Track::Simple:
                ImGui::SetNextItemWidth(getSpacedAvailableWidth(2));
                renderTimeDivisionSelect(track->quantizeDivision);
                ImGui::SameLine();
                break;
            case State::Track::Structured:
                break;
            case State::Track::TimeWindow:
                ImGui::PushFont(*mediumFont);
                renderTime(applicationState->song, track->timeWindow, track->timeWindow, false);
                ImGui::PopFont();
                ImGui::SameLine();
                break;
        }
        if (ImGui::Button("Quantize", ImVec2(getSpacedAvailableWidth(), 0)))
            track->quantize();
    }

    // Render the takes of a track
    float UserInterface::renderTrackSequencer(const State::TrackPointer& track, const ImVec2& position, const ImVec2& size)
    {
        ImVec2 takeSize = ImVec2(mainAreaSize.x, track->height);
        ImVec2 takePosition = position;

        State::TakePointer newSelectedTake = nullptr;
        for (const State::TakePointer& take: track->takes)
            if (renderTakeSequencer(track, take, size.x, takePosition, takeSize))
                newSelectedTake = take;
        if (newSelectedTake) track->recordingTake = newSelectedTake;
        return takePosition.y - position.y;
    }

    // Render the take and the notes of that take
    bool UserInterface::renderTakeSequencer(const State::TrackPointer& track, const State::TakePointer& take,
                                            float sequencerWidth, ImVec2& takePosition, const ImVec2& takeSize)
    {
        bool noteClicked = renderNotes(take, takePosition, takeSize);
        bool takeClicked = ImGui::IsItemClicked();
        if (takeClicked || noteClicked)
        {
            if (!noteClicked && track->recordingTake == take)
                take->selectedNotes.clear();
            selectedTrack = track;
        }

        if (take == track->recordingTake)
        {
            renderTrackPiano(track, takePosition + ImVec2(1, track->height), sequencerWidth - 1);
            drawList->AddRect(takePosition,
                              takePosition + ImVec2(sequencerWidth + 1, track->height + keyHeight.top() + 1),
                              selectedTakeColor);
            takePosition.y += keyHeight.top();
        }
        else
            drawList->AddLine(takePosition + ImVec2(0, track->height),
                              takePosition + ImVec2(sequencerWidth, track->height),
                              mainBorder);
        takePosition.y += track->height;
        return takeClicked || noteClicked;
    }

    // Render the piano showing the generated and played notes under the selected take
    void UserInterface::renderTrackPiano(const State::TrackPointer& track, const ImVec2& position, float sequencerWidth)
    {
        int numberOfOctaves = 11;
        ImGui::SetCursorScreenPos(position + ImVec2(0, 1));
        ImGui::SetNextWindowContentSize(ImVec2(0, keyHeight.top() * 1.05f));
        if (ImGui::BeginChild("trackPianoWindow", ImVec2(sequencerWidth, keyHeight.top() * 1.05f),
                              false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar))
        {
            // Feed the played notes into the track like they would if they were coming from an external source
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
            float numberOfKeys = 7.f * static_cast<float>(numberOfOctaves);
            keyWidth.push(std::max(keyWidth.top(), (sequencerWidth - numberOfKeys) / numberOfKeys));
            renderOctaves(numberOfOctaves, 0, "trackPiano");
            keyWidth.pop();
            for (const Music::HarmonyPointer& harmony: track->harmonies)
                popKeyColors(harmonyNotes[harmony]);
            keyPressed.pop();
            pianoAction.pop();
        }
        ImGui::EndChild();
    }

    // Render a number of octaves of a piano and return the played note
    int UserInterface::renderOctaves(int numberOfOctaves, int startOctave, const std::string& id)
    {
        int playedKey = -1;
        ImGui::PushID(id.c_str());
        keyTopLeft = ImGui::GetCursorScreenPos();
        ImGui::GetStyle().AntiAliasedLines = false;
        ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
        for (int octave = startOctave; octave < startOctave + numberOfOctaves; ++octave)
        {
            // Bottom keys
            keySize = ImVec2(keyWidth.top(), keyHeight.top());
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12, true));
            windowDrawList->AddLine(keyTopLeft, keyTopLeft + ImVec2(0, keyHeight.top()), ImColor(0, 0, 0));
            updateKeyTopLeft(1);
            updatePlayedKey(playedKey, renderPianoKeys(4, octave * 12 + 5, true));

            windowDrawList->AddLine(keyTopLeft, keyTopLeft + ImVec2(0, keyHeight.top()), ImColor(0, 0, 0));
            updateKeyTopLeft(1);
            updateKeyTopLeft(-(keyWidth.top() + 1) * 7 + keyWidth.top() / 2);

            // Top keys
            keySize = ImVec2(keyWidth.top(), keyHeight.top() / 3 * 2);
            updatePlayedKey(playedKey, renderPianoKeys(2, octave * 12 + 1, false));

            updateKeyTopLeft(keyWidth.top() + 2);
            updatePlayedKey(playedKey, renderPianoKeys(3, octave * 12 + 6, false));
            updateKeyTopLeft(keyWidth.top() / 2 + 2);
        }
        updateKeyTopLeft(-keyWidth.top() * 7 * static_cast<float>(numberOfOctaves));
        ImGui::GetStyle().AntiAliasedLines = true;
        ImGui::SetCursorScreenPos(keyTopLeft + ImVec2(0, keyHeight.top() + keyHeight.top() / 20));
        ImGui::PopID();
        return playedKey;
    }

    // Render a set of keys next to each other in the current position
    int UserInterface::renderPianoKeys(int numberOfKeys, int keyIndex, bool top)
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

            // Handle events
            if (noteValue < 128)
            {
                if (ImGui::IsItemActivated() && ImGui::IsItemHovered()) playNote(noteValue);
                if (ImGui::IsItemDeactivated()) stopNote(noteValue);
                if (ImGui::IsItemClicked()) playedKey = noteValue;
            }
            bool active = ImGui::IsItemActive() || keyPressed.top()(noteValue);

            if (i != 0)
            {
                if (top) windowDrawList->AddLine(keyTopLeft, keyTopLeft + ImVec2(0, keySize.y - 1),
                                                 ImColor(0, 0, 0));
                updateKeyTopLeft(1);
            }

            // Draw key
            ColorRangePointer keyColor = keyColors[noteValue].top();
            ImU32 currentColor = active ? keyColor->defaultPressed : keyColor->defaultColor;
            ImVec2 currentKeySize = active ? keySize + ImVec2(0, keySize.y / 20) : keySize;
            ImVec2 keyBottomRight = keyTopLeft + currentKeySize;

            windowDrawList->AddRectFilled(keyTopLeft, keyBottomRight, currentColor);
            updateKeyTopLeft(keyWidth.top());

            ImGui::PopID();
        }
        return playedKey;
    }

    // Update the played key with a given new value
    void UserInterface::updatePlayedKey(int& currentPlayedKey, int newPlayedKey)
    {
        if (newPlayedKey != -1)
            currentPlayedKey = newPlayedKey;
    }

    // Move over the current position
    void UserInterface::updateKeyTopLeft(float adjustment)
    {
        keyTopLeft.x += adjustment;
        ImGui::SetCursorScreenPos(keyTopLeft);
    }

    // Generate a note on message and pass it to the piano action function
    void UserInterface::playNote(int noteValue)
    {
        MIDI::NoteOnPointer noteOnMessage = std::make_shared<MIDI::NoteOn>(Music::Note::getInstance(noteValue), 64);
        pianoAction.top()(noteOnMessage);
    }

    // Generate a note off message and pass it to the piano action function
    void UserInterface::stopNote(int noteValue)
    {
        MIDI::NoteOffPointer noteOffMessage = std::make_shared<MIDI::NoteOff>(Music::Note::getInstance(noteValue), 64);
        pianoAction.top()(noteOffMessage);
    }

    // Play a message using the ui piano player
    void UserInterface::playPianoMessage(const MIDI::MessagePointer& message)
    {
        pianoPlayer->processMIDIMessage(message->rawMessage());
        MIDI::IOManager::getInstance()->sendMIDIOut(std::make_pair(message, pianoOutput));
    }

    // Start or stop the sequencer
    void UserInterface::play()
    {
        applicationState->stopRecording();
        if (sequencer->running)     sequencer->stop();
        else                        sequencer->start();
    }

    // Start the recording
    void UserInterface::record()
    {
        if (!sequencer->running)
        {
            applicationState->recording = true;
            sequencer->start();
        }
    }

    // Calculate the width of a number of widgets equally spaced in the available space
    float UserInterface::getSpacedAvailableWidth(float numberOfWidgets)
    {
        return (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * (numberOfWidgets - 1)) / numberOfWidgets;
    }

    // Initialize all bottom keys to white and all top keys to black
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

    // Get the last incoming message
    MIDI::MessagePointer UserInterface::getLastMessage() const
    {
        return !applicationState->messages.empty() ? applicationState->messages.rbegin()->first : nullptr;
    }

    // Push a color on the stacks of a number of keys
    void UserInterface::pushKeyColors(const std::set<int>& values, const ColorRangePointer& keyColor)
    {
        for (int value: values)
            keyColors[value].push(keyColor);
    }

    // Pop a color from the stacks of a number of keys
    void UserInterface::popKeyColors(const std::set<int>& values)
    {
        for (int value: values)
            keyColors[value].pop();
    }

    // Render a border and add a button in order to be able to drag it
    float UserInterface::renderResizableBorder(const ImVec2& position, float length, bool horizontal)
    {
        float dragWidth = 7;
        float dragOffset = (dragWidth - 1) / 2;
        ImVec2 endPosition = position + (horizontal ? ImVec2(length, 0) : ImVec2(0, length));
        drawList->AddLine(position, endPosition, mainBorder, 1);
        ImGui::SetCursorScreenPos(position - (horizontal ? ImVec2(0, dragOffset) : ImVec2(dragOffset, 0)));
        ImGui::InvisibleButton("trackBorder", horizontal ? ImVec2(length, dragWidth) : ImVec2(dragWidth, length));
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(horizontal ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
        if (ImGui::IsItemActive())
        {
            float dragDelta = horizontal ? ImGui::GetMouseDragDelta().y : ImGui::GetMouseDragDelta().x;
            ImGui::ResetMouseDragDelta();
            return dragDelta;
        }
        return 0;
    }

    // Render a MIDI Instrument consisting of a dropdown containing all available ports and a slider for the channel
    void UserInterface::renderMIDIInstrument(const MIDI::InstrumentPointer& instrument, const std::string& label)
    {
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(label.c_str()).x + itemSpacing);
        ImGui::LabelText("##instrumentLabel", "%s", label.c_str());
        ImGui::SameLine();

        float width = getSpacedAvailableWidth(2);
        ImGui::SetNextItemWidth(width);
        const char* preview = instrument->port.c_str();
        if (instrument->portSpecificity == MIDI::Instrument::All) preview = "All";
        if (instrument->portSpecificity == MIDI::Instrument::None) preview = "None";
        if (ImGui::BeginCombo(("##Port" + label).c_str(), preview))
        {
            if (ImGui::Selectable("All", instrument->portSpecificity == MIDI::Instrument::All))
                instrument->portSpecificity = MIDI::Instrument::All;
            if (ImGui::Selectable("None", instrument->portSpecificity == MIDI::Instrument::None))
                instrument->portSpecificity = MIDI::Instrument::None;
            for (const std::string& outputPort: applicationState->outputPorts)
            {
                bool selected = outputPort == instrument->port && instrument->portSpecificity == MIDI::Instrument::Specific;
                if (ImGui::Selectable(outputPort.c_str(), selected))
                {
                    instrument->portSpecificity = MIDI::Instrument::Specific;
                    instrument->port = outputPort;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(width);
        ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
        const char* format = "Channel %d";
        if (instrument->channelSpecificity == MIDI::Instrument::All) format = "All Channels";
        if (instrument->channelSpecificity == MIDI::Instrument::None) format = "No Channels";
        if (ImGui::SliderInt(("##Channel" + label).c_str(), &instrument->channel,
                             1, 18, format, sliderFlags))
        {
            if (instrument->channel == 17)      instrument->channelSpecificity = MIDI::Instrument::All;
            else if (instrument->channel == 18) instrument->channelSpecificity = MIDI::Instrument::None;
            else                                instrument->channelSpecificity = MIDI::Instrument::Specific;
        }
    }

    // Render the parameters of an audio player consisting of a dropdown for the preset and buttons for mute and solo
    void UserInterface::renderAudioPlayer(const MIDI::AudioPlayerPointer& audioPlayer)
    {
        // Presets
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * .6f);
        if (ImGui::BeginCombo("##Presets", presets[audioPlayer->preset].c_str()))
        {
            for (int i = 0; i < 128; ++i)
                if (ImGui::Selectable(presets[i].c_str(), audioPlayer->preset == i))
                    audioPlayer->setPreset(i);
            ImGui::EndCombo();
        }
        // Mute button
        ImGui::SameLine();
        float width = getSpacedAvailableWidth(2);
        bool muted = audioPlayer->muted;
        if (muted)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, *mutedColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, *mutedColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, mutedColor->defaultPressed);
        }
        if (ImGui::Button("M", ImVec2(width, 0)))
            audioPlayer->muted = !audioPlayer->muted;
        if (muted) ImGui::PopStyleColor(3);
        ImGui::SameLine();

        // Solo button
        bool solo = applicationState->soloAudioPlayers.find(audioPlayer) != applicationState->soloAudioPlayers.end();
        if (solo)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, *soloColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, *soloColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, soloColor->defaultPressed);
        }
        if (ImGui::Button("S", ImVec2(width, 0)))
        {
            if (solo) applicationState->soloAudioPlayers.erase(audioPlayer);
            else      applicationState->soloAudioPlayers.insert(audioPlayer);
            audioPlayer->solo = !audioPlayer->solo;
        }
        if (solo) ImGui::PopStyleColor(3);
    }

    // Render a dropdown containing all possible time divisions
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