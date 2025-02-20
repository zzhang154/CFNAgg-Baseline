1. Here are BUGs when the link loss is activated. The error can reproducer in 10 producers. So just use 10 producers to test it.
It seems that the process is stuck in some iterations.

(1) Open the log, to see stuck in which iteration.
(2) Check the TCPserver.cc 's application table, if neccessary, use wireshark to capture if there are packets not retransmited.
(3) If the problem still cannot be solved, check the send and receive buffer, TxBuffer and RxBuffer.

2. Can we design a stupid TCP AIMD such that we have more line to draw? We want to have a degrade TCP version.

It seems that now, the plain AIMD, NewReno, bbr, Qbic perform the same.

Ask deepseek if there is anyother different scheme that can be implemented as a baseline method.
Continuely testing for different cc strategy, find one that is better than now or is worse. Not let the two curve overlap with each other.

3. Baseline:
Without aggregation, config the aggregation tree just directly from consumer to producers. To see what is happening. The only thing to change is just to make a new aggregation tree.
But if we do this, we should set one bottleneck. Otherwise if there is no bottleneck, then the performance will be low? 

In this way, do we need to change the link cost in order to degrade the performance of this baseline? Since currently, we don't set such a bottleneck.

4. Write the config.ini file and pipeline all the test as soon as possible. Get the update dataset as soon as possible.

Maybe the baseline probe can leave Yixiang to do.



5. BUG FIX:
preivously, why the namespace cannot be used, the reason is because 
#ifndef MYCONFIG_H
#define MYCONFIG_H
// MYCONFIG_H
should be match with the namespace using