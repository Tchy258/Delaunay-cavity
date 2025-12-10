import json
import glob
import re
from statistics import mean

# Values of j and s to process
j_values = [10, 100, 1000, 10000, 100000, 1000000]
s_values = [14, 43, 68, 70, 139]

# Regex pattern to extract j and s from filenames like: points{j}.{s}.json
pattern = re.compile(r"points(\d+)\.(\d+)\.json")

results = {}

for j in j_values:
    n_polygons_values = []

    for s in s_values:
        filename = f"points{j}.{s}.json"
        try:
            with open(filename, "r") as f:
                data = json.load(f)
                if "n_polygons" in data:
                    n_polygons_values.append(data["n_polygons"])
                else:
                    print(f"Warning: 'n_polygons' missing in {filename}")
        except FileNotFoundError:
            print(f"File not found: {filename}")
        except json.JSONDecodeError:
            print(f"Invalid JSON format in file: {filename}")

    if n_polygons_values:
        results[j] = mean(n_polygons_values)
    else:
        results[j] = None

# Print results
for j, avg in results.items():
    print(f"j={j}: average n_polygons = {avg}")
