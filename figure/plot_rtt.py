import matplotlib.pyplot as plt
import numpy as np
import collections
import os
from pathlib import Path

def read_data(file_path):
    """Generic function to read time-value data from a file"""
    data = collections.defaultdict(list)
    with open(file_path, 'r') as file:
        for line in file:
            try:
                time, value = map(float, line.split())
                data[time].append(value)
            except ValueError:
                pass  # Skip invalid lines
    return data

def compute_average(data):
    """Generic function to compute average values for each timestamp"""
    return {time: np.mean(values) for time, values in data.items()}

def plot_metric(data_dict, output_file, ip_address, ylabel, title_suffix, color='b'):
    """Generic plotting function for different metrics"""
    times = sorted(data_dict.keys())
    values = [data_dict[time] for time in times]
    
    plt.figure(figsize=(10, 6))
    plt.plot(times, values, marker='o', linestyle='-', color=color)
    plt.xlabel('Time (ms)')
    plt.ylabel(ylabel)
    plt.title(f'{title_suffix} over Time for {ip_address}')
    plt.grid(True)
    
    plt.savefig(output_file)
    print(f"Plot saved to {output_file}")
    plt.close()

def process_metric(input_dir, output_dir, file_suffix, ylabel, title_suffix, color):
    """Process files for a specific metric"""
    # Create output directory if needed
    os.makedirs(output_dir, exist_ok=True)
    
    # Create input directory if it doesn't exist
    if not os.path.exists(input_dir):
        print(f"Warning: Input directory {input_dir} does not exist, skipping...")
        return
        
    for file_name in os.listdir(input_dir):
        if file_name.endswith(file_suffix):
            input_file = os.path.join(input_dir, file_name)
            ip_address = file_name.split('_')[0]
            output_file = os.path.join(output_dir, file_name.replace('.txt', '.png'))
            
            data = read_data(input_file)
            averaged_data = compute_average(data)
            plot_metric(averaged_data, output_file, ip_address, ylabel, title_suffix, color)

def main():
    # Get the correct root path
    script_dir = Path(__file__).resolve().parent  # Location of this script (figure/)
    root_path = script_dir.parent  # Go up one level to ns-3.42/
    
    # Configuration for different metrics
    metrics_config = [
        {
            'input_dir': root_path / 'log' / 'rtt',
            'output_dir': script_dir / 'rtt',
            'file_suffix': '_rtt.txt',
            'ylabel': 'Average RTT (ms)',
            'title_suffix': 'Average RTT',
            'color': 'b'
        },
        {
            'input_dir': root_path / 'log' / 'tp',
            'output_dir': script_dir / 'tp',
            'file_suffix': '_TP.txt',
            'ylabel': 'Throughput (Mb/s)',
            'title_suffix': 'Throughput',
            'color': 'r'
        }
    ]

    # Process all metrics
    for config in metrics_config:
        process_metric(
            str(config['input_dir']),
            str(config['output_dir']),
            config['file_suffix'],
            config['ylabel'],
            config['title_suffix'],
            config['color']
        )

if __name__ == '__main__':
    main()