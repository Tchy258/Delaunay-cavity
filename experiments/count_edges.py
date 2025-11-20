#import matplotlib
#matplotlib.use('Agg')  # Non-interactive backend
#import matplotlib.pyplot as plt
from collections import defaultdict, Counter
import subprocess
import os
import sys
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
import csv
#from statistics import median, mean, multimode
from math import atan2, degrees
working_dir = os.path.dirname(os.path.realpath(__file__)) 
global_input_seed = 138
# -------------------------
# Utilities
# -------------------------
def is_convex_polygon(points):
    n = len(points)
    if n < 4:
        return True
    sign = None
    for i in range(n):
        x1, y1 = points[i]
        x2, y2 = points[(i + 1) % n]
        x3, y3 = points[(i + 2) % n]
        dx1, dy1 = x2 - x1, y2 - y1
        dx2, dy2 = x3 - x2, y3 - y2
        cross = dx1 * dy2 - dy1 * dx2
        if cross != 0:
            current_sign = cross > 0
            if sign is None:
                sign = current_sign
            elif sign != current_sign:
                return False
    return True

def angle_between(p1, p2, p3):
    x1, y1 = p1
    x2, y2 = p2
    x3, y3 = p3
    deg1 = (360 + degrees(atan2(x1 - x2, y1 - y2))) % 360
    deg2 = (360 + degrees(atan2(x3 - x2, y3 - y2))) % 360
    return deg2 - deg1 if deg1 <= deg2 else 360 - (deg1 - deg2)


def min_max_angle_polygon(poly, vertices):
    min_angle = 360
    max_angle = 0
    for i in range(0, len(poly)):   
        p1 = vertices[poly[i]]
        p2 = vertices[poly[(i + 1) % len(poly)]]
        p3 = vertices[poly[(i + 2) % len(poly)]]
        angle = angle_between(p1, p2, p3)
        if angle < min_angle:
            min_angle = angle
        if angle > max_angle:
            max_angle = angle
    return min_angle, max_angle

def min_max_angle_mesh(regions, vertices):
    min_angle = 360
    max_angle = 0
    for i in regions:
        min_angle_poly, max_angle_poly = min_max_angle_polygon(i, vertices)
        if min_angle_poly < min_angle:
            min_angle = min_angle_poly
        if max_angle_poly > max_angle:
            max_angle = max_angle_poly
    return min_angle, max_angle

def round8(x):
    if isinstance(x, int):
        return x
    return round(float(x), 8)

# -------------------------
# OFF Reader
# -------------------------
def read_off(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    lines = [line.strip() for line in lines if line.strip() and not line.startswith('#')]
    if not lines:
        raise ValueError(f"Empty or invalid OFF file: {filename}")
    if lines[0] != 'OFF':
        raise ValueError("Not a valid OFF file")
    n_vertices, n_faces, _ = map(int, lines[1].split())
    vertices = []
    for i in range(2, 2 + n_vertices):
        parts = list(map(float, lines[i].split()))
        x, y = parts[0], parts[1]
        vertices.append((x, y))
    faces = []
    for i in range(2 + n_vertices, 2 + n_vertices + n_faces):
        parts = list(map(int, lines[i].split()))
        n_edges = parts[0]
        face_vertices = parts[1:]
        if n_edges != len(face_vertices):
            raise ValueError(f"Face line does not match number of vertices: {lines[i]}")
        faces.append(face_vertices)
    return vertices, faces

# -------------------------
# Counting helper
# -------------------------
def count_polygons_by_edges_and_convexity(vertices, faces):
    total_edge_count = defaultdict(int)
    convex_by_edges = defaultdict(int)
    concave_by_edges = defaultdict(int)
    convex_total = 0
    concave_total = 0
    min_angle, max_angle = min_max_angle_mesh(faces, vertices)
    for face in faces:
        n_edges = len(face)
        total_edge_count[n_edges] += 1
        if n_edges >= 4:
            pts = [vertices[idx] for idx in face]
            if is_convex_polygon(pts):
                convex_by_edges[n_edges] += 1
                convex_total += 1
            else:
                concave_by_edges[n_edges] += 1
                concave_total += 1
    return dict(total_edge_count), dict(convex_by_edges), dict(concave_by_edges), convex_total, concave_total, min_angle, max_angle

# -------------------------
# Per-mesh processing
# -------------------------
def process_pointset(name, refiner, mesh_type, comparator, merging_strategy, refinement_criterion, criterion_arg, ascending_or_seed="", sort_key="" , skip_exec=False,
                     input_dir="../../delaunay-cavity-data/data",
                     exec_dir="../build"):
    """
    Process a single mesh: optionally run external executable (unless skip_exec),
    read .off, compute counts and convex/concave breakdowns, produce per-mesh stacked plot,
    and return a dict with all per-mesh stats (no JSON writing here).
    """
    output_dir = Path("../../delaunay-cavity-data/experiments/" + refiner) / mesh_type / comparator / ascending_or_seed / sort_key / merging_strategy / refinement_criterion
    plot_dir = Path("../../delaunay-cavity-data/experiments/plots") / refiner / mesh_type / comparator / ascending_or_seed / sort_key / merging_strategy / refinement_criterion
    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(plot_dir, exist_ok=True)

    output_basename = os.path.join(output_dir, name)
    off_file = output_basename + ".off"

    # ---- Run executable if not skipped ----
    if not skip_exec:
        exec_name = f'{refiner}-{mesh_type}'
        if refiner == "delaunay_cavity_refiner":
            if comparator != "null_comparator" and comparator != "random_comparator":
                exec_name += f'-{ascending_or_seed}_{sort_key}_{comparator}'
            else:
                exec_name += f'-{comparator}'
            exec_name += f"-{merging_strategy}-{refinement_criterion}"
        if "win" in sys.platform:
            exec_name += ".exe"
        node_file  = os.path.join(input_dir, name + ".node")
        ele_file   = os.path.join(input_dir, name + ".ele")
        neigh_file = os.path.join(input_dir, name + ".neigh")
        exec_path = os.path.join(exec_dir, exec_name)
        cmd_list = [exec_path, "--input1", node_file, "--input2", ele_file, "--input3", neigh_file, "--off-output", "--json-output", "--ale-output", "--output" , output_basename]
        if comparator == "random_comparator":
            cmd_list.append("--seed")
            cmd_list.append(ascending_or_seed)
        if refinement_criterion != "null_refinement_criterion":
            cmd_list.append("--threshold")
            cmd_list.append(criterion_arg)
        subprocess.run(cmd_list, check=True, cwd=working_dir)

    # ---- Read OFF and compute stats ----
    vertices, faces = read_off(off_file)
    total_edge_count, convex_by_edges, concave_by_edges, convex_total, concave_total, min_angle, max_angle = \
        count_polygons_by_edges_and_convexity(vertices, faces)

    # ---- Compute per-edge percents (triangles considered convex) ----
    convex_percent_by_edges = {}
    concave_percent_by_edges = {}
    all_edge_keys = set(total_edge_count.keys()) | set(convex_by_edges.keys()) | set(concave_by_edges.keys())
    for e in all_edge_keys:
        total = total_edge_count.get(e, 0)
        cv = convex_by_edges.get(e, 0)
        cc = concave_by_edges.get(e, 0)
        if e < 4:
            cv = total
            cc = 0
        if total > 0:
            convex_percent_by_edges[e] = round8(cv / total)
            concave_percent_by_edges[e] = round8(cc / total)
        else:
            convex_percent_by_edges[e] = round8(0.0)
            concave_percent_by_edges[e] = round8(0.0)

    # ---- Prepare per-mesh result (returned to batch_process) ----
    per_mesh = {
        "mesh": name,
        "edge_count": {int(k): int(v) for k, v in total_edge_count.items()},
        "convex_total": int(convex_total),
        "concave_total": int(concave_total),
        "convex_by_edges": {int(k): int(v) for k, v in convex_by_edges.items()},
        "concave_by_edges": {int(k): int(v) for k, v in concave_by_edges.items()},
        "convex_percent_by_edges": {int(k): float(v) for k, v in convex_percent_by_edges.items()},
        "concave_percent_by_edges": {int(k): float(v) for k, v in concave_percent_by_edges.items()},
        "min_angle": min_angle,
        "max_angle": max_angle
    }

    # # ---- Per-mesh stacked bar plot ----
    # convex_color = "#1f77b4"
    # concave_color = "#ff7f0e"
    # edges_sorted = sorted(all_edge_keys)
    # convex_counts_plot, concave_counts_plot, bottoms = [], [], []

    # for e in edges_sorted:
    #     total = total_edge_count.get(e, 0)
    #     if e < 4:
    #         convex_counts_plot.append(total)
    #         concave_counts_plot.append(0)
    #         bottoms.append(0)
    #     else:
    #         cv = convex_by_edges.get(e, 0)
    #         cc = concave_by_edges.get(e, 0)
    #         convex_counts_plot.append(cv)
    #         concave_counts_plot.append(cc)
    #         bottoms.append(cv)

    # fig, ax = plt.subplots(figsize=(10, 6))
    # ax.bar(edges_sorted, convex_counts_plot, color=convex_color, label='Convex', edgecolor='black')
    # ax.bar(edges_sorted, concave_counts_plot, bottom=bottoms, color=concave_color, label='Concave', edgecolor='black')

    # ax.set_xlabel("Number of edges")
    # ax.set_ylabel("Number of polygons")
    # ax.set_title(f"Polygon distribution (convex vs concave) for {name}")
    # ax.set_xticks(edges_sorted)
    # ax.legend()

    # # Add labels (per segment and total above)
    # for idx, e in enumerate(edges_sorted):
    #     cv = convex_counts_plot[idx]
    #     cc = concave_counts_plot[idx]
    #     if cv > 0:
    #         ax.text(e, cv / 2, str(cv), ha='center', va='center',
    #                 color='white' if cv >= 6 else 'black', fontsize=9, fontweight='bold')
    #     if cc > 0:
    #         ax.text(e, bottoms[idx] + cc / 2, str(cc), ha='center', va='center',
    #                 color='white' if cc >= 6 else 'black', fontsize=9, fontweight='bold')
    #     ax.text(e, cv + cc + 0.5, str(total_edge_count.get(e, 0)), ha='center', va='bottom', fontsize=8)

    # plt.tight_layout()
    # plt.savefig(os.path.join(plot_dir, name + ".pdf"), format="pdf")
    # plt.close(fig)
    # del fig, ax

    return per_mesh

# -------------------------
# Pool wrapper
# -------------------------
def _process_one(args):
    i, refiner, mesh_type, comparator, merging_strategy, refinement_criterion, criterion_arg, ascending_or_seed, sort_key, skip_exec = args
    name = f"points{i}.{global_input_seed}"
    return process_pointset(name=name, refiner=refiner, mesh_type=mesh_type, comparator=comparator, merging_strategy=merging_strategy,
                            refinement_criterion=refinement_criterion, criterion_arg=criterion_arg, ascending_or_seed=ascending_or_seed,
                            sort_key=sort_key, skip_exec=skip_exec)

# -------------------------
# Batch processing (collect results and write wide CSV)
# -------------------------
def batch_process(refiner, mesh_type, comparator, merging_strategy, refinement_criterion, criterion_arg, start, end, ascending_or_seed="", sort_key="" , skip_exec=False, max_workers=8):
    os.makedirs("../../delaunay-cavity-data/experiments/summaries", exist_ok=True)

    results = []  # collect per-mesh dicts
    per_edge_total_counts = defaultdict(list)
    per_edge_convex_counts = defaultdict(list)
    per_edge_concave_counts = defaultdict(list)
    mesh_count = 0

    # --- Process all meshes in parallel ---
    val = start
    indices = []
    while val <= end:
        indices.append(val)
        val *= 10
    with ProcessPoolExecutor(max_workers=max_workers) as executor:
        futures = {
            executor.submit(
                _process_one,
                (i, refiner, mesh_type, comparator, merging_strategy,
                refinement_criterion, criterion_arg, ascending_or_seed,
                sort_key, skip_exec)
            ): i for i in indices
        }

        for future in as_completed(futures):
            result = future.result()
            results.append(result)
            mesh_count += 1

    # --- Determine global set of edge keys across all meshes ---
    global_edges = set()
    for r in results:
        global_edges.update(r["edge_count"].keys())
    edges_sorted = sorted(global_edges)

    # --- Fill per-edge lists and prepare CSV rows (wide format) ---
    csv_rows = []
    for r in results:
        mesh_name = r["mesh"]
        total_edge_count = r["edge_count"]
        convex_by_edges = r["convex_by_edges"]
        concave_by_edges = r["concave_by_edges"]
        convex_pct = r["convex_percent_by_edges"]
        concave_pct = r["concave_percent_by_edges"]
        min_angle = r["min_angle"]
        max_angle = r["max_angle"]
        # compute total polygons for this mesh (sum of total_edge_count values)
        total_polygons = sum(total_edge_count.values())

        # Build row dict
        row = {
            "num_points": int(mesh_name.replace("points", "").split(".")[0]),
            "convex_total": r["convex_total"],
            "concave_total": r["concave_total"],
            "total_polygons": total_polygons
        }

        # For each edge in global set, fill zeros if missing
        for e in edges_sorted:
            row[f"e{e}"] = int(total_edge_count.get(e, 0))
            row[f"convex_e{e}"] = int(convex_by_edges.get(e, 0)) if e >= 4 else int(total_edge_count.get(e, 0))
            row[f"concave_e{e}"] = int(concave_by_edges.get(e, 0)) if e >= 4 else 0
            row[f"convex_pct_e{e}"] = round8(convex_pct.get(e, 0.0)) if e in convex_pct else 0.0
            row[f"concave_pct_e{e}"] = round8(concave_pct.get(e, 0.0)) if e in concave_pct else 0.0

            # collect for combined stats
            per_edge_total_counts[e].append(int(total_edge_count.get(e, 0)))
            per_edge_convex_counts[e].append(int(convex_by_edges.get(e, 0)) if e >= 4 else int(total_edge_count.get(e, 0)))
            per_edge_concave_counts[e].append(int(concave_by_edges.get(e, 0)) if e >= 4 else 0)
        row["min_angle"] = min_angle
        row["max_angle"] = max_angle
        csv_rows.append(row)

    # --- Write wide CSV ---
    # CSV columns: mesh, convex_total, concave_total, total_polygons, then eN, convex_eN, concave_eN, convex_pct_eN, concave_pct_eN for each edge
    summary_dir = Path("../../delaunay-cavity-data/experiments/summaries") / refiner / mesh_type
    if refiner == "delaunay_cavity_refiner":
        summary_dir = summary_dir / comparator / merging_strategy
        if comparator != "null_comparator":
            summary_dir = summary_dir / ascending_or_seed
            if comparator != "random_comparator":
                summary_dir = summary_dir / sort_key
    if refinement_criterion != "null_refinement_criterion":
        summary_dir = summary_dir / f"{refinement_criterion}_{criterion_arg}"
    os.makedirs(summary_dir, exist_ok=True)
    csv_path = summary_dir / f"results_{start}_{end}.csv"

    # Build header
    header = ["num_points", "convex_total", "concave_total", "total_polygons"]
    for e in edges_sorted:
        header.extend([f"e{e}", f"convex_e{e}", f"concave_e{e}", f"convex_pct_e{e}", f"concave_pct_e{e}"])
    header.extend(["min_angle", "max_angle"])
    with open(csv_path, "w", newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=header)
        writer.writeheader()
        for row in csv_rows:
            # Ensure every header key exists in row (fill zeros)
            for key in header:
                if key not in row:
                    row[key] = 0
            writer.writerow(row)

    # # --- Compute combined plot data (averages, median, mode) ---
    # convex_avg_list = [mean(per_edge_convex_counts[e]) if per_edge_convex_counts[e] else 0.0 for e in edges_sorted]
    # concave_avg_list = [mean(per_edge_concave_counts[e]) if per_edge_concave_counts[e] else 0.0 for e in edges_sorted]
    # totals_median = [int(median(per_edge_total_counts[e])) if per_edge_total_counts[e] else 0 for e in edges_sorted]
    # totals_modes = [multimode(per_edge_total_counts[e]) if per_edge_total_counts[e] else [] for e in edges_sorted]

    # # --- Combined stacked bar plot (averages) ---
    # convex_color = "#1f77b4"
    # concave_color = "#ff7f0e"
    # convex_plot = [int(x) for x in convex_avg_list]
    # concave_plot = [int(x) for x in concave_avg_list]
    # bottoms = convex_plot[:]  # convex part is bottom for concave

    # fig, ax = plt.subplots(figsize=(12, 7))
    # ax.bar(edges_sorted, convex_plot, color=convex_color, label='Avg Convex', edgecolor='black')
    # ax.bar(edges_sorted, concave_plot, bottom=bottoms, color=concave_color, label='Avg Concave', edgecolor='black')

    # ax.set_xlabel("Number of edges")
    # ax.set_ylabel("Average number of polygons per mesh")
    # ax.set_title(f"Average convex vs concave polygons per edge")
    # ax.set_xticks(edges_sorted)
    # ax.legend()

    # # Add median and mode labels with adaptive offsets to avoid overlap
    # for idx, e in enumerate(edges_sorted):
    #     top = convex_plot[idx] + concave_plot[idx]
    #     # adaptive offsets: small absolute min plus relative fraction
    #     offset_m = max(0.5, top * 0.03)
    #     offset_o = max(1.0, top * 0.06) + offset_m
    #     # median
    #     ax.text(e, top + offset_m, f"M={totals_median[idx]}", ha='center', va='bottom', fontsize=9, color='black', fontweight='bold')
    #     # mode: join multiple modes with comma
    #     mode_list = totals_modes[idx]
    #     if mode_list:
    #         mode_str = ",".join(str(int(m)) for m in sorted(mode_list))
    #     else:
    #         mode_str = "0"
    #     ax.text(e, top + offset_o, f"O={mode_str}", ha='center', va='bottom', fontsize=9, color='red', fontweight='bold')

    # plt.tight_layout()
    # combined_plot_path = summary_dir / "combined_convex_concave.pdf"
    # plt.savefig(combined_plot_path, format="pdf")
    # plt.close(fig)
    # del fig, ax

    print(f"Wrote CSV summary to: {csv_path}")
    #print(f"Wrote combined plot to: {combined_plot_path}")

# -------------------------
# Main
# -------------------------
if __name__ == "__main__":
    # CLI mapping follows your previous script order
    refiner = sys.argv[1]
    mesh_type = sys.argv[2]
    comparator = sys.argv[3]
    merging_strategy = sys.argv[4]
    refinement_criterion = sys.argv[5]
    start = int(sys.argv[6])
    end = int(sys.argv[7])
    ascending_or_seed = ""
    sort_key = ""
    criterion_arg = 20.0
    # parse remaining positional args similar to your previous mapping
    if comparator != "null_comparator" and comparator != "random_comparator":
        ascending_or_seed = sys.argv[8]
        sort_key = sys.argv[9]
        next_index = 10
    elif comparator == "random_comparator":
        ascending_or_seed = sys.argv[8]
        next_index = 9
    else:
        next_index = 8

    if refinement_criterion != "null_refinement_criterion":
        criterion_arg = sys.argv[next_index]
        next_index += 1

    skip_exec_flag = False
    if len(sys.argv) > next_index:
        if sys.argv[next_index] != "1":
            global_input_seed = int(sys.argv[next_index])
            next_index += 1
        if len(sys.argv) > next_index:
            skip_exec_flag = sys.argv[next_index] == "1"

    batch_process(refiner, mesh_type, comparator, merging_strategy, refinement_criterion, criterion_arg, start, end, ascending_or_seed, sort_key, skip_exec=skip_exec_flag)
