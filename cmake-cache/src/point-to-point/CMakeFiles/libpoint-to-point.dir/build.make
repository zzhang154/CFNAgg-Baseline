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
CMAKE_SOURCE_DIR = /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache

# Include any dependencies generated for this target.
include src/point-to-point/CMakeFiles/libpoint-to-point.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/point-to-point/CMakeFiles/libpoint-to-point.dir/compiler_depend.make

# Include the progress variables for this target.
include src/point-to-point/CMakeFiles/libpoint-to-point.dir/progress.make

# Include the compile flags for this target's objects.
include src/point-to-point/CMakeFiles/libpoint-to-point.dir/flags.make

# Object files for target libpoint-to-point
libpoint__to__point_OBJECTS =

# External object files for target libpoint-to-point
libpoint__to__point_EXTERNAL_OBJECTS = \
"/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/helper/point-to-point-helper.cc.o" \
"/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/point-to-point-channel.cc.o" \
"/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/point-to-point-net-device.cc.o" \
"/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/ppp-header.cc.o"

../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/helper/point-to-point-helper.cc.o
../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/point-to-point-channel.cc.o
../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/point-to-point-net-device.cc.o
../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point-obj.dir/model/ppp-header.cc.o
../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point.dir/build.make
../build/lib/libns3.42-point-to-point-default.so: /usr/lib/x86_64-linux-gnu/libgsl.so
../build/lib/libns3.42-point-to-point-default.so: /usr/lib/x86_64-linux-gnu/libgslcblas.so
../build/lib/libns3.42-point-to-point-default.so: /usr/lib/x86_64-linux-gnu/libsqlite3.so
../build/lib/libns3.42-point-to-point-default.so: src/point-to-point/CMakeFiles/libpoint-to-point.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX shared library ../../../build/lib/libns3.42-point-to-point-default.so"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libpoint-to-point.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/point-to-point/CMakeFiles/libpoint-to-point.dir/build: ../build/lib/libns3.42-point-to-point-default.so
.PHONY : src/point-to-point/CMakeFiles/libpoint-to-point.dir/build

src/point-to-point/CMakeFiles/libpoint-to-point.dir/clean:
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point && $(CMAKE_COMMAND) -P CMakeFiles/libpoint-to-point.dir/cmake_clean.cmake
.PHONY : src/point-to-point/CMakeFiles/libpoint-to-point.dir/clean

src/point-to-point/CMakeFiles/libpoint-to-point.dir/depend:
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42 /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/point-to-point /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/point-to-point/CMakeFiles/libpoint-to-point.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/point-to-point/CMakeFiles/libpoint-to-point.dir/depend

