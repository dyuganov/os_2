# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.2.4\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.2.4\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\user\CLionProjects\lab5_os5sem

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/lab5_os5sem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lab5_os5sem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lab5_os5sem.dir/flags.make

CMakeFiles/lab5_os5sem.dir/main.c.obj: CMakeFiles/lab5_os5sem.dir/flags.make
CMakeFiles/lab5_os5sem.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lab5_os5sem.dir/main.c.obj"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\lab5_os5sem.dir\main.c.obj -c C:\Users\user\CLionProjects\lab5_os5sem\main.c

CMakeFiles/lab5_os5sem.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab5_os5sem.dir/main.c.i"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\user\CLionProjects\lab5_os5sem\main.c > CMakeFiles\lab5_os5sem.dir\main.c.i

CMakeFiles/lab5_os5sem.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab5_os5sem.dir/main.c.s"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\user\CLionProjects\lab5_os5sem\main.c -o CMakeFiles\lab5_os5sem.dir\main.c.s

# Object files for target lab5_os5sem
lab5_os5sem_OBJECTS = \
"CMakeFiles/lab5_os5sem.dir/main.c.obj"

# External object files for target lab5_os5sem
lab5_os5sem_EXTERNAL_OBJECTS =

lab5_os5sem.exe: CMakeFiles/lab5_os5sem.dir/main.c.obj
lab5_os5sem.exe: CMakeFiles/lab5_os5sem.dir/build.make
lab5_os5sem.exe: CMakeFiles/lab5_os5sem.dir/linklibs.rsp
lab5_os5sem.exe: CMakeFiles/lab5_os5sem.dir/objects1.rsp
lab5_os5sem.exe: CMakeFiles/lab5_os5sem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lab5_os5sem.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\lab5_os5sem.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lab5_os5sem.dir/build: lab5_os5sem.exe

.PHONY : CMakeFiles/lab5_os5sem.dir/build

CMakeFiles/lab5_os5sem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\lab5_os5sem.dir\cmake_clean.cmake
.PHONY : CMakeFiles/lab5_os5sem.dir/clean

CMakeFiles/lab5_os5sem.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\user\CLionProjects\lab5_os5sem C:\Users\user\CLionProjects\lab5_os5sem C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug C:\Users\user\CLionProjects\lab5_os5sem\cmake-build-debug\CMakeFiles\lab5_os5sem.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lab5_os5sem.dir/depend

