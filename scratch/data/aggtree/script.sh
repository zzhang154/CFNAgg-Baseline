#!/bin/bash
# filepath: /home/dd/GameTheoryCluster/data/aggtree/process_aggtree.sh
# This script processes all TXT files in the current directory.
# For each file, it deletes the first line and the last line if it is blank.

for file in *.txt; do
    echo "Processing $file..."
    # Use sed in-place:
    #   1d         - Delete the first line.
    #   $ { /^$/d } - For the last line, if it is blank, delete it.
    sed -i '1d;$ { /^$/d }' "$file"
done

echo "Processing complete."