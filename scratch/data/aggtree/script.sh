for file in *; do
    new=$(echo "$file" | tr '[:upper:]' '[:lower:]')
    mv "$file" "aggtree-$new"
done