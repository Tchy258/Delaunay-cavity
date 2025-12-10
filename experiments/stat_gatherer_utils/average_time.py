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
    t_total_values = []

    for s in s_values:
        filename = f"points{j}.{s}.json"
        try:
            with open(filename, "r") as f:
                data = json.load(f)
                if "t_total" in data:
                    t_total_values.append(data["t_total"])
                else:
                    print(f"Warning: 't_total' missing in {filename}")
        except FileNotFoundError:
            print(f"File not found: {filename}")
        except json.JSONDecodeError:
            print(f"Invalid JSON format in file: {filename}")

    if t_total_values:
        results[j] = mean(t_total_values)
    else:
        results[j] = None

# Print results
for j, avg in results.items():
    print(f"j={j}: average t_total = {avg}")
