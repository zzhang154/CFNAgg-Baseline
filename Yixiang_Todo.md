Hi, Yixiang!

Todo: TCP congestion control baseline method probe.

1. How to run the programme?
run
scratch/IWQos.ipynb

You first run the first block to configure and build the project, and then run the second block to run the project.

2. How to config, or select the parameter of the project?
look
src/innetwork-task/config-file/config.ini

In order to test it, you can just set the iterationNumbers to a small number, and only run in one tology. I recommand the following config for you to fast debug.

[Simulation]
iterationNumbers = 30
; Comma-separated list of buffer sizes (in bytes).
bufferSizes = 16384
lossRate = 0.000
; Congestion control algorithm (e.g., TcpBbr, TcpNewReno, TcpCubic, etc.). To extent, see "src/internet/model/tcp-congestion-ops.h"
congestionControl = ns3::TcpAIMD
; Each vector is a uint64_t, there for each TCP packet size is vectorSize*8 bits.
vectorSize = 150

[Topology]
topoTypes = isp
topoScales = 50

3. What you should do?
As you can see the comment in the config Congestion control algorithm (e.g., TcpBbr, TcpNewReno, TcpCubic, etc.). To extent, see "src/internet/model/tcp-congestion-ops.h"
Now, I hope you to let the DeepSeek to design a worse tcp method (such that we can draw another line). Since currently, ICN-AIMD have not yet finished, so we only have 2 lines in the graph. And this is insufficient.
So, I hope you to find out a worse TCP congestion control algorithm (run longer than current TCP)

4. How to design a DIY TCP cc algorithm and use it?
As in the file, you can see I write a DIY cc class "class TcpAIMD : public TcpNewReno".
You can refer to this and design a new algorithm. But remember that its runtime should slower than the current TcpAIMD.

5. What is the performance of the current TCP CC method.
Now TcpBbr, TcpNewReno, TcpCubic and TcpAIMD. It is strange that even with the simple AIMD method, it achieves the same runtime to that of TcpBbr and TcpCubic.

Please help me do this, thanks!