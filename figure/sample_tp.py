#!/usr/bin/env python3
import os
import sys
import matplotlib.pyplot as plt
from pathlib import Path

def read_file_data(filepath):
    """Read two-column data from a file and return a list of (time, value) tuples."""
    data = []
    with open(filepath, 'r') as file:
        for line in file:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            try:
                parts = line.split()
                if len(parts) < 2:
                    continue
                time_val, metric = float(parts[0]), float(parts[1])
                data.append((time_val, metric))
            except ValueError:
                continue
    return data

def plot_sample(data, output_file, ip_address):
    """Plot the sampled data similar to plot_rtt.py"""
    times = [pt[0] for pt in data]
    values = [pt[1] for pt in data]
    
    plt.figure(figsize=(10, 6))
    plt.plot(times, values, marker='o', linestyle='-', color='r')
    plt.xlabel('Time (ms)')
    plt.ylabel('Throughput (Mb/s)')
    plt.title(f'Throughput over Time for {ip_address}')
    plt.grid(True)
    plt.savefig(output_file)
    print(f"Plot saved to {output_file}")
    plt.close()

def process_file(file_path, output_dir):
    data = read_file_data(file_path)
    total = len(data)
    if total < 100:
        print(f"File {file_path} has less than 100 data points. Skipping.")
        return
    # Calculate region boundaries in line numbers.
    start_region = int(0.70 * total)
    end_region = int(0.80 * total)
    # Ensure there is space for 100 continuous records.
    if start_region + 100 <= end_region:
        sample_start = start_region
    else:
        # If the region is too narrow, simply take 100 points starting at start_region.
        sample_start = start_region
    sample_data = data[sample_start:sample_start+100]
    
    # Get ip_address from file name (assumes name like "con0_TP.txt", so ip_address is "con0")
    ip_address = file_path.name.split('_')[0]
    output_file = output_dir / file_path.with_suffix('.png').name
    plot_sample(sample_data, str(output_file), ip_address)

def main():
    script_dir = Path(__file__).resolve().parent  # figure folder
    ns_root = script_dir.parent  # ns-3.42 folder
    input_dir = ns_root / 'log' / 'tp'
    output_dir = script_dir / 'tp-sample'
    os.makedirs(output_dir, exist_ok=True)
    
    if not input_dir.exists():
        print(f"Input directory {input_dir} does not exist.")
        sys.exit(1)
    
    for file_name in os.listdir(input_dir):
        if file_name.endswith('_TP.txt'):
            file_path = input_dir / file_name
            print(f"Processing {file_path}")
            process_file(file_path, output_dir)
            
if __name__ == '__main__':
    main()