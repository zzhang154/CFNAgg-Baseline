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
include src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/compiler_depend.make

# Include the progress variables for this target.
include src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/progress.make

# Include the compile flags for this target's objects.
include src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/flags.make

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/flags.make
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o: ../src/tap-bridge/helper/tap-bridge-helper.cc
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx.gch
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -MD -MT src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o -MF CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o.d -o CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o -c /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/helper/tap-bridge-helper.cc

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.i"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -E /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/helper/tap-bridge-helper.cc > CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.i

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.s"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -S /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/helper/tap-bridge-helper.cc -o CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.s

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/flags.make
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o: ../src/tap-bridge/model/tap-bridge.cc
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx.gch
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -MD -MT src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o -MF CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o.d -o CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o -c /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-bridge.cc

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.i"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -E /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-bridge.cc > CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.i

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.s"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -S /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-bridge.cc -o CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.s

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/flags.make
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o: ../src/tap-bridge/model/tap-encode-decode.cc
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o: CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx.gch
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -MD -MT src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o -MF CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o.d -o CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o -c /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-encode-decode.cc

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.i"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -E /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-encode-decode.cc > CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.i

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.s"
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && /bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Winvalid-pch -include /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx -S /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge/model/tap-encode-decode.cc -o CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.s

libtap-bridge-obj: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/helper/tap-bridge-helper.cc.o
libtap-bridge-obj: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-bridge.cc.o
libtap-bridge-obj: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/model/tap-encode-decode.cc.o
libtap-bridge-obj: src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/build.make
.PHONY : libtap-bridge-obj

# Rule to build all files generated by this target.
src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/build: libtap-bridge-obj
.PHONY : src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/build

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/clean:
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge && $(CMAKE_COMMAND) -P CMakeFiles/libtap-bridge-obj.dir/cmake_clean.cmake
.PHONY : src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/clean

src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/depend:
	cd /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42 /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/tap-bridge /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/cmake-cache/src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/tap-bridge/CMakeFiles/libtap-bridge-obj.dir/depend

