# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build

# Include any dependencies generated for this target.
include 2024041606/CMakeFiles/06_texture_wrap.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include 2024041606/CMakeFiles/06_texture_wrap.dir/compiler_depend.make

# Include the progress variables for this target.
include 2024041606/CMakeFiles/06_texture_wrap.dir/progress.make

# Include the compile flags for this target's objects.
include 2024041606/CMakeFiles/06_texture_wrap.dir/flags.make

2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o: 2024041606/CMakeFiles/06_texture_wrap.dir/flags.make
2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o: /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606/06_texture_wrap.cpp
2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o: 2024041606/CMakeFiles/06_texture_wrap.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object 2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o"
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT 2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o -MF CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o.d -o CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o -c /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606/06_texture_wrap.cpp

2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.i"
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606/06_texture_wrap.cpp > CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.i

2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.s"
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606/06_texture_wrap.cpp -o CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.s

# Object files for target 06_texture_wrap
06_texture_wrap_OBJECTS = \
"CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o"

# External object files for target 06_texture_wrap
06_texture_wrap_EXTERNAL_OBJECTS =

2024041606/06_texture_wrap: 2024041606/CMakeFiles/06_texture_wrap.dir/06_texture_wrap.cpp.o
2024041606/06_texture_wrap: 2024041606/CMakeFiles/06_texture_wrap.dir/build.make
2024041606/06_texture_wrap: libglad.a
2024041606/06_texture_wrap: /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/lib/libglm.a
2024041606/06_texture_wrap: 2024041606/CMakeFiles/06_texture_wrap.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 06_texture_wrap"
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/06_texture_wrap.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
2024041606/CMakeFiles/06_texture_wrap.dir/build: 2024041606/06_texture_wrap
.PHONY : 2024041606/CMakeFiles/06_texture_wrap.dir/build

2024041606/CMakeFiles/06_texture_wrap.dir/clean:
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 && $(CMAKE_COMMAND) -P CMakeFiles/06_texture_wrap.dir/cmake_clean.cmake
.PHONY : 2024041606/CMakeFiles/06_texture_wrap.dir/clean

2024041606/CMakeFiles/06_texture_wrap.dir/depend:
	cd /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606 /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606 /home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/build/2024041606/CMakeFiles/06_texture_wrap.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : 2024041606/CMakeFiles/06_texture_wrap.dir/depend

