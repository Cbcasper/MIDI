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

        float divisionWidth;
        float scroll;
        float keyWidth;
        float keyLength;
        ImVec2 keyTopLeft;
        ImVec2 keySize;

        UserInterface(const std::shared_ptr<State::Application>& applicationState,
                      const std::shared_ptr<System::Sequencer>& sequencer,
                      const std::shared_ptr<Music::Quantizer>& quantizer);

        void start();
        void renderUI();

        void renderMessageMonitor();
        void renderMainWindow();
        void renderSequencer(const ImVec2& sequencerPosition, const ImVec2& sequencerSize);
        void renderPiano();
        void renderHarmonyModel();

        MIDI::MessagePointer getLastMessage() const;
        void drawKeys(int numberOfKeys, int keyIndex, const ImColor& keyColor,
                      const ImColor& pressedColor, const ImColor& borderColor);
        void updateKeyTopLeft(float adjustment);
        void playNote(int noteValue);

        void stopNote(int noteValue);
    };
}

#endif //MIDI_ASSISTANT_USER_INTERFACE_H
