cmake_minimum_required(VERSION 3.9)
project(MIDIAssistant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-ObjC++ -fobjc-weak -fobjc-arc")

include_directories(include)
link_directories(lib)

find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)

# libremidi
find_library(COREMIDI_LIBRARY CoreMIDI)
find_library(COREAUDIO_LIBRARY CoreAudio)
find_library(COREFOUNDATION_LIBRARY CoreFoundation)

# imgui
find_library(METAL_LIBRARY Metal)
find_library(METALKIT_LIBRARY MetalKit)
find_library(COCOA_LIBRARY Cocoa)
find_library(IOKIT_LIBRARY IOKit)
find_library(COREVIDEO_LIBRARY CoreVideo)
find_library(QUARTZCORE_LIBRARY QuartzCore)

find_package(glfw3 REQUIRED)

set(SOURCES
        MIDI/IO/IOManager.cpp MIDI/IO/IOManager.h
        MIDI/Processor.cpp MIDI/Processor.h
        Director/Director.cpp Director/Director.h
        Sequencer/Sequencer.cpp Sequencer/Sequencer.h
        Theory/Theory.cpp Theory/Theory.h
        UI/UserInterface.cpp UI/UserInterface.h
        MIDI/AudioPlayer.cpp MIDI/AudioPlayer.h
        MIDI/Messages/Message.cpp MIDI/Messages/Message.h
        MIDI/Messages/Messages/NoteOn.cpp MIDI/Messages/Messages/NoteOn.h
        MIDI/Messages/Messages/NoteOff.cpp MIDI/Messages/Messages/NoteOff.h
        MIDI/Messages/Messages/Aftertouch.cpp MIDI/Messages/Messages/Aftertouch.h
        MIDI/Messages/Messages/ProgramChange.cpp MIDI/Messages/Messages/ProgramChange.h
        MIDI/Messages/Messages/ChannelPressure.cpp MIDI/Messages/Messages/ChannelPressure.h
        MIDI/Messages/Messages/PitchWheel.cpp MIDI/Messages/Messages/PitchWheel.h
        MIDI/Messages/Messages/Controller.cpp MIDI/Messages/Messages/Controller.h
        Theory/Note/Note.cpp Theory/Note/Note.h
        Application/Application.cpp Application/Application.h
        MIDI/MessageFilter.cpp MIDI/MessageFilter.h
        UI/MessageFilterCheckboxes.cpp UI/MessageFilterCheckboxes.h
        MIDI/FileManager.cpp MIDI/FileManager.h
        MIDI/IO/Port.cpp MIDI/IO/Port.h
        MIDI/IO/InputPort.cpp MIDI/IO/InputPort.h
        MIDI/IO/OutputPort.cpp MIDI/IO/OutputPort.h
        MIDI/Messages/Messages/NoteMessage.cpp MIDI/Messages/Messages/NoteMessage.h
        Director/Harmonies/Harmony.cpp Director/Harmonies/Harmony.h
        Theory/Note/NoteFilter.cpp Theory/Note/NoteFilter.h
        Application/Track.cpp Application/Track.h
        Director/Harmonies/SingleHarmonies/TranspositionHarmony.cpp Director/Harmonies/SingleHarmonies/TranspositionHarmony.h
        Utilities/Utilities.cpp Utilities/Utilities.h
        Sequencer/Timer.cpp Sequencer/Timer.h
        Application/Song.cpp Application/Song.h
        Theory/TimeDivision.h Theory/TimeDivision.cpp
        Theory/Note/RootNote.cpp Theory/Note/RootNote.h
        Theory/Key/Key.cpp Theory/Key/Key.h
        Theory/Key/IntervalSequence.cpp Theory/Key/IntervalSequence.h
        Theory/Key/MelodicMinor.cpp Theory/Key/MelodicMinor.h
        Director/Harmonies/SingleHarmonies/ModulationHarmony.cpp Director/Harmonies/SingleHarmonies/ModulationHarmony.h
        Director/Harmonies/SingleHarmonies/SingleHarmony.cpp Director/Harmonies/SingleHarmonies/SingleHarmony.h
        Director/Harmonies/SingleHarmonies/RandomHarmony.cpp Director/Harmonies/SingleHarmonies/RandomHarmony.h
        UI/ColorRange.cpp UI/ColorRange.h
        MIDI/IO/IOType.h
        Sequencer/Metronome.cpp Sequencer/Metronome.h
        MIDI/Messages/Instrument.cpp MIDI/Messages/Instrument.h
        UI/Font.cpp UI/Font.h
        Director/Harmonies/CanonHarmony.cpp Director/Harmonies/CanonHarmony.h
        Sequencer/Cycle.cpp Sequencer/Cycle.h
        Application/Take.cpp Application/Take.h
        Director/Harmonies/ChoralHarmony/ChoralHarmony.cpp Director/Harmonies/ChoralHarmony/ChoralHarmony.h
        Director/Harmonies/ChoralHarmony/Tessiture.cpp Director/Harmonies/ChoralHarmony/Tessiture.h
        Theory/Chord/Chord.cpp Theory/Chord/Chord.h
        Theory/Chord/ChordNote.cpp Theory/Chord/ChordNote.h
        Director/Harmonies/ChoralHarmony/Voices.cpp Director/Harmonies/ChoralHarmony/Voices.h
        Utilities/Zip.h
        Utilities/Average.cpp Utilities/Average.h)

set(IMGUI
        imgui/imgui.cpp imgui/imgui.h
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_impl_glfw.cpp imgui/imgui_impl_glfw.h
        imgui/imgui_impl_metal.mm imgui/imgui_impl_metal.h
        imgui/Renderer.mm imgui/Renderer.h
        imgui/IconsFontAwesome6.h)

add_executable(MIDIAssistant
        main.cpp ${SOURCES} ${IMGUI})
target_link_libraries(MIDIAssistant
        liblibremidi.a
        ${AUDIOTOOLBOX_LIBRARY}
        ${COREMIDI_LIBRARY} ${COREAUDIO_LIBRARY} ${COREFOUNDATION_LIBRARY}
        ${METAL_LIBRARY} ${METALKIT_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${QUARTZCORE_LIBRARY}
        glfw)
