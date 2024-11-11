# Install script for directory: /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/examples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/channel-models/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/energy/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/error-model/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/ipv6/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/matrix-topology/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/naming/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/realtime/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/routing/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/socket/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/stats/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/tcp/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/traffic-control/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/tutorial/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/udp/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/udp-client-server/cmake_install.cmake")
  include("/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/examples/wireless/cmake_install.cmake")

endif()

