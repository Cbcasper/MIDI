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

        ImColor mainBackground;
        ImColor darkBackground;

        ImColor divisionColor;
        ImColor playingColor;
        ImColor playingColorTransparent;
        ImColor recordingColor;
        ImColor recordingColorTransparent;

        UserInterface(const std::shared_ptr<State::Application>& applicationState,
                      const std::shared_ptr<System::Sequencer>& sequencer,
                      const std::shared_ptr<Music::Quantizer>& quantizer);

        void start();
        void renderUI();

        void renderMessageMonitor();
        void renderMainWindow();
        void renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize);
        void renderControl(const ImVec2& controlPosition, const ImVec2& controlSize);
        void renderPiano();
        void renderHarmonyModel();

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

        void drawKeys(int numberOfKeys, int keyIndex, const ImColor& keyColor,
                      const ImColor& pressedColor, const ImColor& borderColor);
        void updateKeyTopLeft(float adjustment);

        void playNote(int noteValue);
        void stopNote(int noteValue);
    };
}

#endif //MIDI_ASSISTANT_USER_INTERFACE_H
