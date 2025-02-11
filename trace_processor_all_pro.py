#!/usr/bin/env python3
import sys
import re
from collections import OrderedDict

AGGTREE_FILE = "scratch/data/aggtree50.txt"
TRACE_LOG_FILE = "trace_record.log"

def parse_aggtree(filename):
    child_to_parent = {}
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or ":" not in line:
                continue
            parent, children = line.split(":", 1)
            parent = parent.strip()
            for child in children.strip().split():
                child_to_parent[child] = parent
    return child_to_parent

def parse_trace_log(filename):
    iterations = {}
    current_iter = None
    current_node = None
    with open(filename, "r") as f:
        for line in f:
            line = line.rstrip("\n")
            # Detect iteration headers
            m = re.match(r"iter\s+(\d+)", line)
            if m:
                current_iter = int(m.group(1))
                iterations[current_iter] = OrderedDict()
                current_node = None
                continue
            # Detect node headers (exact matches like "pro10", "agg2")
            if re.match(r"^(pro|agg|con|forwarder)\d+$", line.strip()):
                current_node = line.strip()
                if current_iter is not None:
                    iterations[current_iter][current_node] = []
                continue
            # Add details to current node
            if current_iter is not None and current_node is not None and line.strip():
                iterations[current_iter][current_node].append(line.strip())
    return iterations

def dedup_lines(lines):
    seen = set()
    return [l for l in lines if not (l in seen or seen.add(l))]

def build_upward_chain(child_to_parent, start):
    chain = [start]
    node = start
    while node in child_to_parent:
        parent = child_to_parent[node]
        if parent in chain:
            break
        chain.append(parent)
        node = parent
    return chain

def main():
    # Automatically process all iterations.
    child_to_parent = parse_aggtree(AGGTREE_FILE)
    iterations = parse_trace_log(TRACE_LOG_FILE)
    
    iterations_to_process = sorted(iterations.keys())
    
    out_filename = TRACE_LOG_FILE.replace(".log", "-aggtree50.log") if TRACE_LOG_FILE.endswith(".log") else TRACE_LOG_FILE + "-aggtree50"
    output_lines = []
    # First header line: include all iteration info
    if len(iterations_to_process) == 1:
        output_lines.append(f"path_trace for iter {iterations_to_process[0]}")
    else:
        its = ", ".join(str(it) for it in iterations_to_process)
        output_lines.append(f"path_trace for iters: {its}")
    
    # Process each iteration with a clear separator when more than one
    for it in iterations_to_process:
        output_lines.append("")  # separator blank line
        output_lines.append(f"----- Iteration {it} -----")
        block = iterations[it]
        for node in block.keys():
            if not node.startswith("pro"):
                continue
            chain = build_upward_chain(child_to_parent, node)
            if not chain or chain[-1] != "con0":
                continue
            output_lines.append("")
            output_lines.append(f"Tracing for producer: {node}")
            for n in chain:
                if n in block:
                    details = dedup_lines(block[n])
                    filtered = [d for d in details if d != n]
                    output_lines.extend(filtered or [n])
                else:
                    output_lines.append(n)
    
    with open(out_filename, "w") as outf:
        outf.write("\n".join(output_lines))
    print(f"Output written to {out_filename}")

if __name__ == "__main__":
    main()