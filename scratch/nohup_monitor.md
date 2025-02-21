ps aux | grep '[n]s3'
pkill -f "ns3.42-innetwork-test-default --topotype=1"
pkill -f "ns3 run scratch/innetwork-test.cc -- --topotype=1"

tail -f log/IWQoS-small-topo.log
chmod 777 /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/scratch/run.sh