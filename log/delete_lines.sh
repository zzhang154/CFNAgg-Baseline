#!/bin/bash
# Usage: ./delete_lines.sh logfile.txt

if [ $# -ne 1 ]; then
    echo "Usage: $0 filename"
    exit 1
fi

# Delete lines containing either of the two patterns, backup original file
sed -i.bak '/isRetransmission true/d; /TcpSocketBase:IsValidTcpSegment()/d' "$1" && \
echo "Lines removed successfully. Original file backed up to $1.bak."