//
// Created by Casper De Smet on 22/02/2022.
//

#ifndef MIDI_ASSISTANT_USER_INTERFACE_H
#define MIDI_ASSISTANT_USER_INTERFACE_H

#include <stack>
#include <libremidi/message.hpp>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "Font.h"
#include "KeyColor.h"
#include "MessageFilterCheckboxes.h"
#include "../MIDI/FileManager.h"
#include "../MIDI/IO/IOManager.h"
#include "../Sequencer/Sequencer.h"
#include "../Application/Application.h"

namespace UI
{
    class UserInterface
    {
    public:
        std::shared_ptr<State::Application> applicationState;
        std::shared_ptr<System::Sequencer> sequencer;

        MessageFilterCheckboxes messageMonitorFilter;
        ImGuiTableFlags midiMessageTableFlags;
        MIDI::MessagePointer previousLastMessage;

        MIDI::AudioPlayer pianoPlayer;
        MIDI::InstrumentPointer pianoInput;
        MIDI::InstrumentPointer pianoOutput;

        Music::TimeDivision quantizeDivision;

        FontPointer defaultFont;
        FontPointer mediumFont;
        FontPointer largeFont;

        float controlHeight;
        float trackListRatio;
        float sequencerRatio;
        float harmonyRatio;

        float scroll;
        float headerSize;
        float measureWidth;
        float divisionsPerBeat;
        float measureLength;
        float totalWidth;

        ImVec2 mainAreaPosition;
        ImVec2 mainAreaSize;

        float keyWidth;
        float keyLength;
        ImVec2 keyTopLeft;
        ImVec2 keySize;

        ImU32 mainBackground;
        ImU32 darkBackground;
        ImU32 mainBorder;
        ImU32 darkBorder;

        ImU32 harmonyHeaderColor;
        ImU32 tracklistHeaderColor;

        ImU32 divisionColor;
        ImU32 playingColor;
        ImU32 playingColorTransparent;
        ImU32 recordingColor;
        ImU32 recordingColorTransparent;

        ImU32 cycleColor;
        ImU32 metronomeColor;

        KeyColorPointer blackKey;
        KeyColorPointer whiteKey;
        KeyColorPointer primaryHighlight;
        KeyColorPointer secondaryHighlight;

        std::map<int, std::stack<KeyColorPointer>> keyColors;
        std::map<Music::Harmony::Type, KeyColorPointer> harmonyColors;

        UserInterface(const std::shared_ptr<State::Application>& applicationState,
                      const std::shared_ptr<System::Sequencer>& sequencer);

        void start();
        void renderUI();

        void renderMessageMonitor();
        void renderMainWindow();
        void renderControl(const ImVec2& controlPosition, const ImVec2& controlSize);
        void renderTrackList(const ImVec2& tracklistPosition, const ImVec2& tracklistSize);
        void renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize);
        void renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize);
        void renderPiano();

        void renderTrackControls(const ImVec2& trackControlsPosition, const ImVec2& trackControlsSize);

        void renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize);

        void renderSongData();
        void renderTime(const State::SongPointer& song);
        int renderPaddedTimeElement(const std::string& id, int value, int total);
        void renderSignature(const State::SongPointer& song);
        void adjustTime(int measures, int beats, int divisions, int ticks);
        void renderTempo();
        void renderKey(const Music::KeyPointer& key);

        void renderSequencerControls();
        bool renderPlayButton();
        bool renderColoredControlButton(const std::string& icon, bool colored, ImU32 color);

        void renderHarmonySource(const ImVec2& harmonySourcePosition, const ImVec2& harmonySourceSize);

        float computeXPosition(int ticks);
        float computeYPosition(const ImVec2& trackSize, const State::TrackPointer& track, int value);
        int computeTickDelta(float positionDelta);

        void renderGrid(ImDrawList* drawList);
        void renderSequencerTracks(ImDrawList* drawList);
        void renderNotes(ImDrawList* drawList, const ImVec2& trackPosition, const ImVec2& trackSize);
        float renderPlayhead(ImDrawList* drawList, const ImVec2& sequencerPosition);
        void sequencerEventButtons(float playheadLocation, const ImVec2& sequencerPosition,
                                   const ImVec2& sequencerSize);
        void sequencerKeyEvents();

        void renderTrackHarmonyModel(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                     const ImVec2& trackHarmonySize);
        void renderDefaultHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                      const ImVec2& trackHarmonySize);
        void renderSelectedHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                       const ImVec2& trackHarmonySize);
        void renderTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                const ImVec2& trackHarmonyPosition, const ImVec2& trackHarmonySize);
        void constructHarmony(Music::Harmony::Type type, const State::TrackPointer& track);
        void renderRandomHarmony(const Music::HarmonyPointer& harmony);
        void renderTranspositionHarmony(const Music::HarmonyPointer& harmony);
        void renderModulationHarmony(const Music::HarmonyPointer& harmony);
        void renderCanonHarmony(const Music::HarmonyPointer& harmony);
        void renderChoralHarmony(const Music::HarmonyPointer& harmony);

        int renderOctaves(int numberOfOctaves, int startOctave, const std::string& id);
        int renderPianoKeys(int numberOfKeys, int keyIndex);
        void updateKeyTopLeft(float adjustment);
        void updatePlayedKey(int& currentPlayedKey, int newPlayedKey);

        void playNote(int noteValue);
        void stopNote(int noteValue);
        void play();
        void record();

        void initializeKeyColors();
        MIDI::MessagePointer getLastMessage() const;
        void pushKeyColors(const std::set<int>& values, const KeyColorPointer& keyColor);
        void popKeyColors(const std::set<int>& values);

        void renderMIDIInstrument(const MIDI::InstrumentPointer& instrument, float availableWidth = 0);
        void renderTimeDivisionSelect(Music::TimeDivision& selectingTimeDivision);
    };
}

#endif //MIDI_ASSISTANT_USER_INTERFACE_H
