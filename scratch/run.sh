#!/bin/bash                              
cd ..

# ./ns3 clean 2>&1 | sed -r "s/\x1B\[[0-9;]*[mK]//g" > log/ns3-clean-config.log
# ./ns3 configure --disable-werror 2>&1 | sed -r "s/\x1B\[[0-9;]*[mK]//g" >> log/ns3-clean-config.log
# ./ns3 build 2>&1 | sed -r "s/\x1B\[[0-9;]*[mK]//g" > log/ns3-build.log

CURRENT_DIR=$(pwd)

# Set the LD_LIBRARY_PATH environment variable
export LD_LIBRARY_PATH=$CURRENT_DIR/build/lib:$LD_LIBRARY_PATH

# export NS_LOG=*=level_all

# Print the LD_LIBRARY_PATH for verification
echo "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"

nohup ./ns3 run scratch/innetwork-test.cc -- --topotype=1 2>&1 | sed -r "s/\x1B\[[0-9;]*[mK]//g" > 2.22-new-log/48k-app30-topo500-Cubic &

# python trace_processor_all_pro.py
# python figure/plot_rtt.py