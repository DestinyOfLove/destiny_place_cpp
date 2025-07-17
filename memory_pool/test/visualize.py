import json
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import os
import shutil

def create_visualizations(data, output_dir):
    """Generates and saves plots from the benchmark data into the specified directory."""
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    df = pd.DataFrame(data['results'])

    # --- Plot 1: Throughput for Fixed-Size Pools (High Contention) ---
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(12, 8))
    
    fixed_df = df[df['mix'] == '32B'].copy()
    fixed_df['throughput'] /= 1000 # Convert to M-ops/sec

    sns.barplot(data=fixed_df, x='threads', y='throughput', hue='name', ax=ax, palette='viridis')

    ax.set_title('Fixed-Size Pool Throughput (High Contention)', fontsize=16)
    ax.set_xlabel('Number of Threads', fontsize=12)
    ax.set_ylabel('Throughput (Million ops/sec)', fontsize=12)
    ax.legend(title='Implementation')
    
    plt.tight_layout()
    throughput_filename = os.path.join(output_dir, 'benchmark_throughput.png')
    plt.savefig(throughput_filename)
    print(f"Saved throughput plot to {throughput_filename}")
    plt.close()

    # --- Plot 2: Latency for Fixed-Size Pools (High Contention) ---
    fig, ax = plt.subplots(figsize=(12, 8))

    sns.lineplot(data=fixed_df, x='threads', y='p99_latency', hue='name', marker='o', ax=ax, palette='viridis')

    ax.set_title('Fixed-Size Pool p99 Latency (High Contention)', fontsize=16)
    ax.set_xlabel('Number of Threads', fontsize=12)
    ax.set_ylabel('p99 Latency (nanoseconds)', fontsize=12)
    ax.set_yscale('log')
    ax.legend(title='Implementation')
    
    plt.tight_layout()
    latency_filename = os.path.join(output_dir, 'benchmark_latency.png')
    plt.savefig(latency_filename)
    print(f"Saved latency plot to {latency_filename}")
    plt.close()

    # --- Plot 3: General-Purpose vs. Malloc ---
    fig, ax = plt.subplots(figsize=(12, 8))

    general_df = df[df['mix'] == 'Mixed'].copy()
    general_df['throughput'] /= 1000

    sns.barplot(data=general_df, x='threads', y='throughput', hue='name', ax=ax, palette='plasma')

    ax.set_title('General-Purpose Pool vs. Malloc (Mixed Sizes, Medium Contention)', fontsize=16)
    ax.set_xlabel('Number of Threads', fontsize=12)
    ax.set_ylabel('Throughput (Million ops/sec)', fontsize=12)
    ax.legend(title='Implementation')

    plt.tight_layout()
    general_filename = os.path.join(output_dir, 'benchmark_general_vs_malloc.png')
    plt.savefig(general_filename)
    print(f"Saved general-purpose plot to {general_filename}")
    plt.close()

if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.abspath(__file__))
    json_path = os.path.join(script_dir, 'test_result.json')
    output_data_dir = os.path.join(script_dir, 'data')

    try:
        with open(json_path, 'r') as f:
            benchmark_data = json.load(f)
        
        # Create visualizations in the data directory
        create_visualizations(benchmark_data, output_data_dir)

        # Move the source json file into the data directory as well
        shutil.move(json_path, os.path.join(output_data_dir, 'test_result.json'))
        print(f"Moved source data to {os.path.join(output_data_dir, 'test_result.json')}")

    except FileNotFoundError:
        print(f"Error: {json_path} not found. Please run the benchmark first.")
    except Exception as e:
        print(f"An error occurred: {e}")