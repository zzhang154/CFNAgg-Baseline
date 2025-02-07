2025/02/05, for path tracing.
Todo:
1. packet parse.
tcp-socket-base class should be able to recognize the packet received and sent. This can be done via scaning the first 10 bytes of the packet and recognize the flag, then scratch out the iteration number.

2. iteration number for tag.
After recognize the current iteration number, find the record from the m_lastTraceTag list and inherit the tag info for the corresponding iteration number.

3. m_lastTraceTag management.
When sent packet for a specific iteration, we can delete the m_lastTraceTag record of that iteration. Since it has completed the process of this iteration and will never need this iteration info after then.

4. ?? How to do with the forwarder?
It seems that there is no code for controling the behavior of forwarders. How can we do this?-