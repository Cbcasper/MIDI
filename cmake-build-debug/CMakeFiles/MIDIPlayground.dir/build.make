# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/MIDIPlayground.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MIDIPlayground.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MIDIPlayground.dir/flags.make

CMakeFiles/MIDIPlayground.dir/main.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MIDIPlayground.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/main.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/main.cpp"

CMakeFiles/MIDIPlayground.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/main.cpp" > CMakeFiles/MIDIPlayground.dir/main.cpp.i

CMakeFiles/MIDIPlayground.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/main.cpp" -o CMakeFiles/MIDIPlayground.dir/main.cpp.s

CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o: ../MIDI/Manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Manager.cpp"

CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Manager.cpp" > CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.i

CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Manager.cpp" -o CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.s

CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o: ../MIDI/Processor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Processor.cpp"

CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Processor.cpp" > CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.i

CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MIDI/Processor.cpp" -o CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.s

CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o: ../Director/Director.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Director/Director.cpp"

CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Director/Director.cpp" > CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.i

CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Director/Director.cpp" -o CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.s

CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o: ../Sequencer/Sequencer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Sequencer/Sequencer.cpp"

CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Sequencer/Sequencer.cpp" > CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.i

CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/Sequencer/Sequencer.cpp" -o CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.s

CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o: ../MusicRules/Manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MusicRules/Manager.cpp"

CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MusicRules/Manager.cpp" > CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.i

CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/MusicRules/Manager.cpp" -o CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.s

CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o: CMakeFiles/MIDIPlayground.dir/flags.make
CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o: ../UI/Manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o -c "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/UI/Manager.cpp"

CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/UI/Manager.cpp" > CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.i

CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/UI/Manager.cpp" -o CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.s

# Object files for target MIDIPlayground
MIDIPlayground_OBJECTS = \
"CMakeFiles/MIDIPlayground.dir/main.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o" \
"CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o"

# External object files for target MIDIPlayground
MIDIPlayground_EXTERNAL_OBJECTS =

MIDIPlayground: CMakeFiles/MIDIPlayground.dir/main.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/MIDI/Manager.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/MIDI/Processor.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/Director/Director.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/Sequencer/Sequencer.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/MusicRules/Manager.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/UI/Manager.cpp.o
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/build.make
MIDIPlayground: CMakeFiles/MIDIPlayground.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable MIDIPlayground"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MIDIPlayground.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MIDIPlayground.dir/build: MIDIPlayground

.PHONY : CMakeFiles/MIDIPlayground.dir/build

CMakeFiles/MIDIPlayground.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MIDIPlayground.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MIDIPlayground.dir/clean

CMakeFiles/MIDIPlayground.dir/depend:
	cd "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant" "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant" "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug" "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug" "/Users/casperdesmet/Dropbox/ThirdYear/SecondSemester/IndividueelProject/MIDI Assistant/cmake-build-debug/CMakeFiles/MIDIPlayground.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/MIDIPlayground.dir/depend

