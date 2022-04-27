//
// Created by Casper De Smet on 22/02/2022.
//

#ifndef MIDI_ASSISTANT_USER_INTERFACE_H
#define MIDI_ASSISTANT_USER_INTERFACE_H

#include <libremidi/message.hpp>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Application/Application.h"
#include "../MIDI/IO/IOManager.h"
#include "../Sequencer/Sequencer.h"
#include "MessageFilterCheckboxes.h"
#include "../MIDI/FileManager.h"
#include "../Director/Quantizer.h"
#include "KeyColor.h"

namespace UI
{
    class UserInterface
    {
    public:
        std::shared_ptr<State::Application> applicationState;
        std::shared_ptr<System::Sequencer> sequencer;
        std::shared_ptr<Music::Quantizer> quantizer;

        MessageFilterCheckboxes messageMonitorFilter;
        ImGuiTableFlags midiMessageTableFlags;
        MIDI::MessagePointer previousLastMessage;

        MIDI::AudioPlayer pianoPlayer;
        MIDI::InstrumentPointer pianoInput;
        MIDI::InstrumentPointer pianoOutput;

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

        ImU32 divisionColor;
        ImU32 playingColor;
        ImU32 playingColorTransparent;
        ImU32 recordingColor;
        ImU32 recordingColorTransparent;

        KeyColorPointer blackKey;
        KeyColorPointer whiteKey;
        KeyColorPointer highlightedColor;

        std::map<Music::Harmony::Type, ImU32> harmonyColors;

        UserInterface(const std::shared_ptr<State::Application>& applicationState,
                      const std::shared_ptr<System::Sequencer>& sequencer,
                      const std::shared_ptr<Music::Quantizer>& quantizer);

        void start();
        void renderUI();

        void renderMessageMonitor();
        void renderMainWindow();
        void renderControl(const ImVec2& controlPosition, const ImVec2& controlSize);
        void renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize);
        void renderHarmonyModel(const ImVec2& harmonyPosition, const ImVec2& harmonyModelSize);
        void renderPiano();

        void renderControlBar(const ImVec2& controlBarPosition, const ImVec2& controlBarSize);
        void renderTime(const State::SongPointer& song);
        int renderPaddedTimeElement(const std::string& id, int value, int total);
        void renderSignatureEdit(const State::SongPointer& song);
        void adjustTime(int measures, int beats, int divisions, int ticks);
        void renderTempo();
        void renderKey(const Music::KeyPointer& key);

        void renderHarmonySource(const ImVec2& harmonySourcePosition, const ImVec2& harmonySourceSize);
        void constructHarmony(Music::Harmony::Type type);

        void computeMeasureLength();
        float computePosition(int ticks);
        int computeTickDelta(float positionDelta);

        void renderGrid(ImDrawList* drawList);
        void renderNotes(ImDrawList* drawList);
        float renderPlayhead(ImDrawList* drawList, const ImVec2& sequencerPosition);
        void sequencerEventButtons(float playheadLocation, const ImVec2& sequencerPosition,
                                   const ImVec2& sequencerSize);
        void sequencerKeyEvents();

        MIDI::MessagePointer getLastMessage() const;

        int renderOctaves(int numberOfOctaves, int startOctave, const std::string& id,
                           const std::set<int>& highlightedKeys = {});
        int renderPianoKeys(int numberOfKeys, int keyIndex, const KeyColorPointer& keyColor,
                            const std::set<int>& highlightedKeys);
        void updateKeyTopLeft(float adjustment);
        void updatePlayedKey(int& currentPlayedKey, int newPlayedKey);

        void playNote(int noteValue);
        void stopNote(int noteValue);
    };
}

#endif //MIDI_ASSISTANT_USER_INTERFACE_H
