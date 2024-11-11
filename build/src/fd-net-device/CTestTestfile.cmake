# CMake generated Testfile for 
# Source directory: /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device
# Build directory: /home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/src/fd-net-device
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ctest-raw-sock-creator "ns3.42-raw-sock-creator-debug")
set_tests_properties(ctest-raw-sock-creator PROPERTIES  WORKING_DIRECTORY "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/src/fd-net-device/" _BACKTRACE_TRIPLES "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;140;set_runtime_outputdirectory;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;156;build_exec;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;0;")
add_test(ctest-tap-device-creator "ns3.42-tap-device-creator-debug")
set_tests_properties(ctest-tap-device-creator PROPERTIES  WORKING_DIRECTORY "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/src/fd-net-device/" _BACKTRACE_TRIPLES "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;140;set_runtime_outputdirectory;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;194;build_exec;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;0;")
add_test(ctest-netmap-device-creator "ns3.42-netmap-device-creator-debug")
set_tests_properties(ctest-netmap-device-creator PROPERTIES  WORKING_DIRECTORY "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build/src/fd-net-device/" _BACKTRACE_TRIPLES "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/build-support/custom-modules/ns3-executables.cmake;140;set_runtime_outputdirectory;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;235;build_exec;/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/fd-net-device/CMakeLists.txt;0;")
subdirs("examples")
