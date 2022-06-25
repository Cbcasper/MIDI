#include "UI/UserInterface.h"
#include "MIDI/IO/IOManager.h"
#include "Application/Application.h"
#include "Sequencer/Sequencer.h"
#include "Director/Director.h"
#include "MIDI/FileManager.h"

int main()
{
    std::shared_ptr<State::Application> applicationState = std::make_shared<State::Application>();

    System::TimerPointer timer = System::Timer::getInstance();
    timer->initialize(applicationState);

    // Create the three main components
    std::shared_ptr<Music::Director> director = std::make_shared<Music::Director>(applicationState);
    std::shared_ptr<MIDI::Processor> midiProcessor = std::make_shared<MIDI::Processor>(applicationState, director);
    std::shared_ptr<System::Sequencer> sequencer = std::make_shared<System::Sequencer>(applicationState, director);

    // Link IO Manager and File Manager to processor
    MIDI::IOManager::getInstance()->initialize(midiProcessor);
    MIDI::FileManager::getInstance()->initialize(midiProcessor);

    // Create initial track
    MIDI::InstrumentPointer input = applicationState->selectInstrument(MIDI::Input);
    MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
    State::TrackPointer initialTrack = std::make_shared<State::Track>(applicationState, input, output);
    applicationState->tracks.emplace_back(initialTrack);

    // Create and start ui
    std::shared_ptr<UI::UserInterface> ui = std::make_shared<UI::UserInterface>(applicationState, initialTrack, director, sequencer);
    ui->start();

    return 0;
}
