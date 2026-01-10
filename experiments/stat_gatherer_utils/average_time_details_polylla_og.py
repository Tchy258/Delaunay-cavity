import json
import re
from statistics import mean

# Values of j and s to process
vertex_amount = [10, 100, 1000, 10000, 100000, 1000000]
seeds = [14, 43, 68, 70, 139]

# Regex pattern to extract j and s from filenames like: points{j}.{s}.json
pattern = re.compile(r"points(\d+)\.(\d+)\.json")

results = {}

for j in vertex_amount:
    current = {}
    current[f'time_to_label_max_edges'] = []
    current[f'time_to_label_frontier_edges'] = []
    current[f'time_to_label_seed_edges'] = []
    current[f'time_to_label_total'] = []
    current[f'time_to_traversal_and_repair'] = []
    current[f'time_to_traversal'] = []
    current[f'time_to_repair'] = []
    current[f'time_to_generate_polygonal_mesh'] = []
    for s in seeds:
        filename = f"points{j}.{s}.json"
        try:
            with open(filename, "r") as f:
                data = json.load(f)
                for k, _ in current.items():
                    current[k].append(data[k])
        except FileNotFoundError:
            print(f"File not found: {filename}")
        except json.JSONDecodeError:
            print(f"Invalid JSON format in file: {filename}")
    for k, v in current.items():
        results[f'{k}_{j}'] = mean(v)

# Print results
for k, avg in results.items():
    j = k.split('_')[-1]
    print(f"j={j}: average {k} = {avg}")
