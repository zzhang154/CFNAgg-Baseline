#!/bin/bash

# 运行 Generate-exchange-tree.py 脚本五次，输入分别为 20、30、40、50
for percentage in 20 30 40 50 100; do
    echo "Running Generate-exchange-tree.py with percentage: $percentage"
    echo $percentage | sudo python ./Generate-exchange-tree.py
done

# 运行 copy_script.sh
echo "Running copy_script.sh"
cd ./data
# Process the "link" and "router" folders.
for folder in link router; do
    for file in "$folder"/*.txt; do
        # Extract the basename.
        base=$(basename "$file")
        # Insert "exchange-<percentage>percent-" between the second dash and the number.
        # Eg: link-dcn-10.txt becomes link-dcn-exchange-20percent-10.txt.
        for percentage in 20 30 40 50 100; do
            newname=$(echo "$base" | sed -E "s/(^(link|router)-[^-]+)-([0-9]+\.txt)/\1-exchange-${percentage}percent-\3/")
            cp "$file" "$folder/$newname"
        done
    done
done