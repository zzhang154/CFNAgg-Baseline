# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build

# Include any dependencies generated for this target.
include src/dsdv/CMakeFiles/libdsdv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/dsdv/CMakeFiles/libdsdv.dir/compiler_depend.make

# Include the progress variables for this target.
include src/dsdv/CMakeFiles/libdsdv.dir/progress.make

# Include the compile flags for this target's objects.
include src/dsdv/CMakeFiles/libdsdv.dir/flags.make

# Object files for target libdsdv
libdsdv_OBJECTS =

# External object files for target libdsdv
libdsdv_EXTERNAL_OBJECTS = \
"/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv-obj.dir/helper/dsdv-helper.cc.o" \
"/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-packet-queue.cc.o" \
"/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-packet.cc.o" \
"/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-routing-protocol.cc.o" \
"/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-rtable.cc.o"

lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv-obj.dir/helper/dsdv-helper.cc.o
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-packet-queue.cc.o
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-packet.cc.o
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-routing-protocol.cc.o
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv-obj.dir/model/dsdv-rtable.cc.o
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv.dir/build.make
lib/libns3.42-dsdv-debug.so: /usr/lib/x86_64-linux-gnu/libgsl.so
lib/libns3.42-dsdv-debug.so: /usr/lib/x86_64-linux-gnu/libgslcblas.so
lib/libns3.42-dsdv-debug.so: /usr/lib/x86_64-linux-gnu/libsqlite3.so
lib/libns3.42-dsdv-debug.so: src/dsdv/CMakeFiles/libdsdv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX shared library ../../lib/libns3.42-dsdv-debug.so"
	cd /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libdsdv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/dsdv/CMakeFiles/libdsdv.dir/build: lib/libns3.42-dsdv-debug.so
.PHONY : src/dsdv/CMakeFiles/libdsdv.dir/build

src/dsdv/CMakeFiles/libdsdv.dir/clean:
	cd /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv && $(CMAKE_COMMAND) -P CMakeFiles/libdsdv.dir/cmake_clean.cmake
.PHONY : src/dsdv/CMakeFiles/libdsdv.dir/clean

src/dsdv/CMakeFiles/libdsdv.dir/depend:
	cd /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42 /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/src/dsdv /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/build/src/dsdv/CMakeFiles/libdsdv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/dsdv/CMakeFiles/libdsdv.dir/depend

