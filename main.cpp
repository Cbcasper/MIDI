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

//    unsigned long currentTicks = 0;
//    unsigned long startTicks = timer->ticks;
//    auto start = std::chrono::system_clock::now();
//
//    std::shared_lock<std::shared_mutex> lock(timer->signalMutex);
//    bool status = false;
//    timer->subscribe(&status);
//    timer->signalCV.wait(lock, [&]{ return status; });
//    while (currentTicks != 1200)
//    {
//        currentTicks++;
//        status = false;
//        timer->signalCV.wait(lock, [&]{ return status; });
//    }
//    auto end = std::chrono::system_clock::now();
//    std::chrono::duration<double> diff = end - start;
//    std::cout << diff.count() << "\n";
//    timer->unsubscribe(&status);

    std::shared_ptr<Music::Director> director = std::make_shared<Music::Director>(applicationState);
    std::shared_ptr<MIDI::Processor> midiProcessor = std::make_shared<MIDI::Processor>(applicationState, director);
    std::shared_ptr<System::Sequencer> sequencer = std::make_shared<System::Sequencer>(applicationState, director);

    MIDI::IOManager::getInstance()->initialize(midiProcessor);
    MIDI::FileManager::getInstance()->initialize(midiProcessor);

    MIDI::InstrumentPointer input = applicationState->selectInstrument(MIDI::Input);
    MIDI::InstrumentPointer output = applicationState->selectInstrument(MIDI::Output);
    applicationState->tracks.emplace_back(std::make_shared<State::Track>(applicationState, input, output));

    std::shared_ptr<UI::UserInterface> ui = std::make_shared<UI::UserInterface>(applicationState, director, sequencer);
    ui->start();

    return 0;
}
