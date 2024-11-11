# Install script for directory: /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor

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
    set(CMAKE_INSTALL_CONFIG_NAME "default")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so"
         RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib:/usr/local/lib64:$ORIGIN/:$ORIGIN/../lib64")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/lib/libns3.42-flow-monitor-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so"
         OLD_RPATH "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/lib:::::::::::::::::"
         NEW_RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib:/usr/local/lib64:$ORIGIN/:$ORIGIN/../lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.42-flow-monitor-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/helper/flow-monitor-helper.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/flow-classifier.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/flow-monitor.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/flow-probe.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/ipv4-flow-classifier.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/ipv4-flow-probe.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/ipv6-flow-classifier.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/flow-monitor/model/ipv6-flow-probe.h"
    "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/include/ns3/flow-monitor-module.h"
    )
endif()

