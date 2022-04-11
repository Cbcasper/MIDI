#include "UI/UserInterface.h"
#include "MIDI/IO/IOManager.h"
#include "Application/Application.h"
#include "Sequencer/Sequencer.h"
#include "Director/Director.h"
#include "MIDI/FileManager.h"
#include "Director/Quantizer.h"

int main()
{
//    Music::IntervalSequence mode = Music::IntervalSequence(Music::Mode::Mixolydian);
//    Music::RootNotePointer rootNote = std::make_shared<Music::RootNote>(Music::RootNote::C, false);
//    mode.apply(rootNote);
//
//    const auto& [correctNote, correctOctaves] = mode.modulate(std::make_shared<Music::RootNote>(Music::RootNote::F, false), 6);
//    const auto& [wrongNote, wrongOctaves] = mode.modulate(std::make_shared<Music::RootNote>(Music::RootNote::F, true), -6);
//
//    std::cout << "Correct: " << correctNote->toString() << ", " << correctOctaves << "\n";
//    std::cout << "Wrong: " << wrongNote << ", " << wrongOctaves << "\n";

    std::shared_ptr<State::Application> applicationState = std::make_shared<State::Application>();

    std::shared_ptr<Music::Director> director = std::make_shared<Music::Director>(applicationState);
    std::shared_ptr<Music::Quantizer> quantizer = std::make_shared<Music::Quantizer>(applicationState);
    std::shared_ptr<MIDI::Processor> midiProcessor = std::make_shared<MIDI::Processor>(applicationState, director);
    std::shared_ptr<System::Sequencer> sequencer = std::make_shared<System::Sequencer>(applicationState, director);

    MIDI::IOManager::getInstance()->initialize(midiProcessor);
    MIDI::FileManager::getInstance()->initialize(midiProcessor);
    System::Timer::getInstance();

    std::shared_ptr<UI::UserInterface> ui = std::make_shared<UI::UserInterface>(applicationState, sequencer, quantizer);
    ui->start();

    return 0;
}
