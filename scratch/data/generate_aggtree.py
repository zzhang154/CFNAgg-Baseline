#!/usr/bin/env python3
import os
import sys

def is_forwarder(node):
    """Determine if the node is a forwarder by checking for 'forwarder' in its name (case-insensitive)."""
    return 'forwarder' in node.lower()

def main():
    if len(sys.argv) < 2:
        print("Usage: {} <link-file>".format(sys.argv[0]))
        sys.exit(1)
    
    link_file = sys.argv[1]
    if not os.path.isfile(link_file):
        print("File not found:", link_file)
        sys.exit(1)
    
    # Extract the postfix from the filename.
    # For example, given "link-1-fwd-2-agg.txt", postfix will be "-1-fwd-2-agg".
    basename = os.path.basename(link_file)
    if basename.startswith("link") and basename.endswith(".txt"):
        postfix = basename[len("link"):-len(".txt")]
    else:
        print("Filename does not match expected pattern 'link<postfix>.txt'")
        sys.exit(1)
    
    out_filename = "aggtree" + postfix + ".txt"
    
    # Process the link file to build the aggregation tree.
    # We simulate the chain:
    # For each link, if the source is not a forwarder, we update a "prev" node.
    # When the destination is not a forwarder, we record an edge: child = dst, parent = prev, then update prev.
    edges = []
    prev = None
    with open(link_file, "r") as f:
        for line in f:
            line = line.strip()
            # Skip blank lines and comments
            if not line or line.startswith("//"):
                continue
                
            parts = line.split()
            if len(parts) < 2:
                continue
            src, dst = parts[0], parts[1]
            
            # If the source is a non-forwarder, update the chain.
            if not is_forwarder(src):
                prev = src
            
            # If destination is non-forwarder, add an edge from previous non-forwarder to this node.
            if not is_forwarder(dst) and prev is not None:
                edges.append((dst, prev))
                prev = dst
    
    # Write the aggregation tree to the output file.
    with open(out_filename, "w") as out:
        for child, parent in edges:
            out.write(f"{child}: {parent}\n")
    
    print("Aggregation tree written to", out_filename)

if __name__ == "__main__":
    main()