# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lsl/Desktop/code/project/mynetlib/mynetlib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lsl/Desktop/code/project/mynetlib/mynetlib/build

# Include any dependencies generated for this target.
include src/mysql/test/CMakeFiles/MysqlPoolTest.dir/depend.make

# Include the progress variables for this target.
include src/mysql/test/CMakeFiles/MysqlPoolTest.dir/progress.make

# Include the compile flags for this target's objects.
include src/mysql/test/CMakeFiles/MysqlPoolTest.dir/flags.make

src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o: src/mysql/test/CMakeFiles/MysqlPoolTest.dir/flags.make
src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o: ../src/mysql/test/MysqlPoolTest.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lsl/Desktop/code/project/mynetlib/mynetlib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o"
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o -c /home/lsl/Desktop/code/project/mynetlib/mynetlib/src/mysql/test/MysqlPoolTest.cc

src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.i"
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lsl/Desktop/code/project/mynetlib/mynetlib/src/mysql/test/MysqlPoolTest.cc > CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.i

src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.s"
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lsl/Desktop/code/project/mynetlib/mynetlib/src/mysql/test/MysqlPoolTest.cc -o CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.s

# Object files for target MysqlPoolTest
MysqlPoolTest_OBJECTS = \
"CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o"

# External object files for target MysqlPoolTest
MysqlPoolTest_EXTERNAL_OBJECTS =

../src/mysql/test/MysqlPoolTest: src/mysql/test/CMakeFiles/MysqlPoolTest.dir/MysqlPoolTest.o
../src/mysql/test/MysqlPoolTest: src/mysql/test/CMakeFiles/MysqlPoolTest.dir/build.make
../src/mysql/test/MysqlPoolTest: ../lib/libmynetlib.so
../src/mysql/test/MysqlPoolTest: src/mysql/test/CMakeFiles/MysqlPoolTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lsl/Desktop/code/project/mynetlib/mynetlib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../../src/mysql/test/MysqlPoolTest"
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MysqlPoolTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/mysql/test/CMakeFiles/MysqlPoolTest.dir/build: ../src/mysql/test/MysqlPoolTest

.PHONY : src/mysql/test/CMakeFiles/MysqlPoolTest.dir/build

src/mysql/test/CMakeFiles/MysqlPoolTest.dir/clean:
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test && $(CMAKE_COMMAND) -P CMakeFiles/MysqlPoolTest.dir/cmake_clean.cmake
.PHONY : src/mysql/test/CMakeFiles/MysqlPoolTest.dir/clean

src/mysql/test/CMakeFiles/MysqlPoolTest.dir/depend:
	cd /home/lsl/Desktop/code/project/mynetlib/mynetlib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lsl/Desktop/code/project/mynetlib/mynetlib /home/lsl/Desktop/code/project/mynetlib/mynetlib/src/mysql/test /home/lsl/Desktop/code/project/mynetlib/mynetlib/build /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test /home/lsl/Desktop/code/project/mynetlib/mynetlib/build/src/mysql/test/CMakeFiles/MysqlPoolTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/mysql/test/CMakeFiles/MysqlPoolTest.dir/depend

