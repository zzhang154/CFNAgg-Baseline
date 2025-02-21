#!/bin/bash
# filepath: ./data/generate.sh

# Process the "link" and "router" folders.
for folder in link router; do
    for file in "$folder"/*.txt; do
        # Extract the basename.
        base=$(basename "$file")
        # Insert "no-agg-" between the second dash and the number.
        # Eg: link-dcn-10.txt becomes link-dcn-no-agg-10.txt.
        newname=$(echo "$base" | sed -E 's/(^(link|router)-[^-]+)-([0-9]+\.txt)/\1-no-agg-\3/')
        cp "$file" "$folder/$newname"
    done
done

# Process the "aggtree" folder.
for file in aggtree/*.txt; do
    base=$(basename "$file")
    # Extract number from filename (e.g., "aggtree-dcn-10.txt" -> 10)
    number=$(echo "$base" | grep -oE '[0-9]+' )
    # Create the content: "con0:" then "pro0 pro1 ... pro(N-1)"
    line="con0:"
    for ((i=0; i<number; i++)); do
        line+=" pro$i"
    done
    # Create new file name by inserting "no-agg-" before the number.
    newname=$(echo "$base" | sed -E 's/(^(aggtree-[^-]+)-)([0-9]+\.txt)/\1no-agg-\3/')
    echo "$line" > "aggtree/$newname"
done