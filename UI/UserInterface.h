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
#include "ColorRange.h"
#include "MessageFilterCheckboxes.h"
#include "../MIDI/FileManager.h"
#include "../MIDI/IO/IOManager.h"
#include "../Director/Director.h"
#include "../Sequencer/Sequencer.h"
#include "../Application/Application.h"

namespace UI
{
    using SongEdit = std::function<void()>;
    using TimeEdit = std::function<int(float availableWidth)>;

    class UserInterface
    {
    public:
        enum SongDataType
        {
            Time,
            Signature,
            Tempo,
            Key
        };
        // Links to state and components
        State::ApplicationPointer applicationState;
        System::SequencerPointer sequencer;
        Music::DirectorPointer director;

        // State for message monitor
        MessageFilterCheckboxes messageMonitorFilter;
        ImGuiTableFlags midiMessageTableFlags;
        MIDI::MessagePointer previousLastMessage;

        bool showMessageMonitor;

        // IO for UI pianoPlayer
        MIDI::AudioPlayerPointer pianoPlayer;
        MIDI::InstrumentPointer pianoOutput;

        std::array<std::string, 128> presets;

        ImDrawList* drawList;

        // Fonts
        FontPointer defaultFont;
        FontPointer mediumFont;
        FontPointer largeFont;

        // General sizing
        float controlHeight;
        float trackListRatio;
        float sequencerRatio;
        float harmonyRatio;

        // Sequencer sizing and state
        float scroll;
        float headerSize;
        float measureWidth;
        float divisionsPerBeat;
        float measureLength;
        float totalSequencerWidth;

        ImVec2 mainAreaPosition;
        ImVec2 mainAreaSize;

        // Piano sizing and positioning
        std::stack<float> keyWidth;
        std::stack<float> keyHeight;
        ImVec2 keyTopLeft;
        ImVec2 keySize;

        // Colors
        // General colors
        ImU32 mainBackground;
        ImU32 darkBackground;
        ImU32 mainBorder;
        ImU32 darkBorder;

        // Headers
        ImU32 harmonyHeaderColor;
        ImU32 tracklistHeaderColor;

        // Sequencer colors
        ImU32 divisionColor;
        ImU32 playingColor;
        ImU32 playingColorTransparent;
        ImU32 recordingColor;
        ImU32 recordingColorTransparent;

        // Accent colors for various widgets
        ImU32 cycleColor;
        ImU32 metronomeColor;

        ColorRangePointer mutedColor;
        ColorRangePointer soloColor;

        ImU32 selectedTrackColor;
        ImU32 selectedTakeColor;

        // Piano colors
        ColorRangePointer blackKey;
        ColorRangePointer whiteKey;
        ColorRangePointer primaryHighlight;
        ColorRangePointer secondaryHighlight;

        // Current song editing state
        SongDataType songDataType;
        std::function<void()> songEdit;

        // Piano function stacks and colors
        std::stack<std::function<void(const MIDI::MessagePointer&)>> pianoAction;
        std::stack<std::function<bool(int)>> keyPressed;
        std::map<int, std::stack<ColorRangePointer>> keyColors;
        std::map<Music::Harmony::Type, ColorRangePointer> harmonyColors;

        State::TrackPointer selectedTrack;

        UserInterface(const State::ApplicationPointer& applicationState, const State::TrackPointer& initialTrack,
                      const Music::DirectorPointer& director, const System::SequencerPointer& sequencer);

        // Startup
        void start();
        void renderUI();
        void readPresets();

        // Main windows and areas
        void renderMessageMonitor();
        void renderMainWindow();
        void renderControl(const ImVec2& controlPosition, const ImVec2& controlSize);
        void renderTrackList(const ImVec2& tracklistPosition, const ImVec2& tracklistSize);
        void renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize);
        void renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize);
        void renderTracks(const ImVec2& trackListPosition, const ImVec2& sequencerPosition,
                          const ImVec2& harmonyModelPosition, float trackListWidth, float sequencerWidth,
                          float harmonyModelWidth);

        //////////////////////////////////////
        //              Control             //
        //////////////////////////////////////
        void renderTrackControls(const ImVec2& trackControlsPosition, const ImVec2& trackControlsSize);

        void renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize);
        // Song data
        void renderSongData();
        SongEdit renderTime(const State::SongPointer& song, int& editTime, int displayTime, bool controlBar);
        TimeEdit renderPaddedTimeElement(const std::string& id, int value, int total, bool controlBar);
        void adjustTime(int& currentTime, int measures, int beats, int divisions, int ticks);
        SongEdit renderSignature(const State::SongPointer& song);
        SongEdit renderTempo();
        SongEdit renderKey(const Music::KeyPointer& key);

        // Controls
        void renderSequencerControls();
        bool renderPlayButton();
        bool renderColoredControlButton(const std::string& icon, bool colored, ImU32 color);

        // Harmony source
        void renderHarmonySource(const ImVec2& harmonySourcePosition, const ImVec2& harmonySourceSize);

        //////////////////////////////////////
        //            Sequencer             //
        //////////////////////////////////////
        // Utilities for sequencer computation
        float computeXPosition(int ticks);
        float computeYPosition(const ImVec2& trackSize, const State::TakePointer& take, int value);
        int computeTickDelta(float positionDelta);

        // Sequencer
        void renderGrid();
        bool renderNotes(const State::TakePointer& take, const ImVec2& takePosition, const ImVec2& takeSize);
        float renderPlayhead(const ImVec2& sequencerPosition);
        void sequencerEventButtons(float playheadLocation, const ImVec2& sequencerPosition,
                                   const ImVec2& sequencerSize);
        void sequencerKeyEvents();

        // Delete any selected items
        void deleteItems();

        //////////////////////////////////////
        //          Harmony Model           //
        //////////////////////////////////////
        void renderTrackHarmonyModel(const State::TrackPointer& track, const ImVec2& position, const ImVec2& size);
        void renderDefaultHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                      const ImVec2& trackHarmonySize);
        void renderSelectedHarmonyView(const State::TrackPointer& track, const ImVec2& trackHarmonyPosition,
                                       const ImVec2& trackHarmonySize);
        bool renderOpenTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                    const ImVec2& trackHarmonyPosition, const ImVec2& trackHarmonySize);
        bool renderClosedTrackHarmony(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                      float trackHarmonyHeight);
        bool renderTrackHarmonyName(const State::TrackPointer& track, const Music::HarmonyPointer& harmony,
                                    ImU32 color);
        // Specific harmonies
        void constructHarmony(Music::Harmony::Type type, const State::TrackPointer& track);
        void renderRandomHarmony(const Music::HarmonyPointer& harmony);
        void renderTranspositionHarmony(const Music::HarmonyPointer& harmony);
        void renderModulationHarmony(const Music::HarmonyPointer& harmony);
        void renderCanonHarmony(const Music::HarmonyPointer& harmony);
        void renderChoralHarmony(const Music::HarmonyPointer& harmony);

        //////////////////////////////////////
        //            Track List            //
        //////////////////////////////////////
        void renderTrackListItem(const State::TrackPointer& track, const ImVec2& position, const ImVec2& size);
        void renderQuantization(const State::TrackPointer& track);
        float renderTrackSequencer(const State::TrackPointer& track, const ImVec2& position, const ImVec2& size);
        bool renderTakeSequencer(const State::TrackPointer& track, const State::TakePointer& take,
                                 float sequencerWidth, ImVec2& takePosition, const ImVec2& takeSize);
        void renderTrackPiano(const State::TrackPointer& track, const ImVec2& position, float sequencerWidth);

        // Piano
        int renderOctaves(int numberOfOctaves, int startOctave, const std::string& id);
        int renderPianoKeys(int numberOfKeys, int keyIndex, bool top);
        void updateKeyTopLeft(float adjustment);
        void updatePlayedKey(int& currentPlayedKey, int newPlayedKey);

        // Utilities for piano
        void playNote(int noteValue);
        void stopNote(int noteValue);
        void playPianoMessage(const MIDI::MessagePointer& message);

        // Utilities for sequencer
        void play();
        void record();

        // General utilities
        float getSpacedAvailableWidth(float numberOfWidgets = 1);
        void initializeKeyColors();
        MIDI::MessagePointer getLastMessage() const;
        void pushKeyColors(const std::set<int>& values, const ColorRangePointer& keyColor);
        void popKeyColors(const std::set<int>& values);

        // Render utility widgets
        float renderResizableBorder(const ImVec2& position, float length, bool horizontal);
        void renderMIDIInstrument(const MIDI::InstrumentPointer& instrument, const std::string& label);
        void renderAudioPlayer(const MIDI::AudioPlayerPointer& audioPlayer);
        void renderTimeDivisionSelect(Music::TimeDivision& selectingTimeDivision);
    };
}

#endif //MIDI_ASSISTANT_USER_INTERFACE_H
