# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Peter\Documents\Console

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Peter\Documents\Console\build

# Include any dependencies generated for this target.
include CMakeFiles/Console.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Console.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Console.dir/flags.make

CMakeFiles/Console.dir/main.cpp.obj: CMakeFiles/Console.dir/flags.make
CMakeFiles/Console.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Peter\Documents\Console\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Console.dir/main.cpp.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1425~1.286\bin\Hostx64\x64\cl.exe  /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\Console.dir\main.cpp.obj /FdCMakeFiles\Console.dir/ /FS -c C:\Users\Peter\Documents\Console\main.cpp

CMakeFiles/Console.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Console.dir/main.cpp.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1425~1.286\bin\Hostx64\x64\cl.exe > CMakeFiles\Console.dir\main.cpp.i  /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Peter\Documents\Console\main.cpp

CMakeFiles/Console.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Console.dir/main.cpp.s"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1425~1.286\bin\Hostx64\x64\cl.exe  /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\Console.dir\main.cpp.s /c C:\Users\Peter\Documents\Console\main.cpp

# Object files for target Console
Console_OBJECTS = \
"CMakeFiles/Console.dir/main.cpp.obj"

# External object files for target Console
Console_EXTERNAL_OBJECTS =

Console.exe: CMakeFiles/Console.dir/main.cpp.obj
Console.exe: CMakeFiles/Console.dir/build.make
Console.exe: CMakeFiles/Console.dir/objects1.rsp
Console.exe: CMakeFiles/Console.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Peter\Documents\Console\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Console.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Console.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Console.dir/build: Console.exe

.PHONY : CMakeFiles/Console.dir/build

CMakeFiles/Console.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Console.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Console.dir/clean

CMakeFiles/Console.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Peter\Documents\Console C:\Users\Peter\Documents\Console C:\Users\Peter\Documents\Console\build C:\Users\Peter\Documents\Console\build C:\Users\Peter\Documents\Console\build\CMakeFiles\Console.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Console.dir/depend

