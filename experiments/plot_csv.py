import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math
import re
from matplotlib import patches as mpatches
import sys
# ============================================================
#                     USER CONFIGURATION
# ============================================================

CSV_PATHS = [
    "summary_polylla.csv",
    "summary_cavity.csv"
]

NUM_POINTS_LIST = [int(sys.argv[1])]                # num_points to plot
COLOR_OFFSET = int(2*(math.log10(NUM_POINTS_LIST[0])))
DECIMAL_COMMA = True

TITLE = f"Polígonos según cantidad de aristas n = {NUM_POINTS_LIST[0]} vértices"
Y_LABEL = "Cantidad de poligonos"
X_LABEL_ROTATION = 0

CONVEX_LABEL = "Convexo"
CONCAVE_LABEL = "Cóncavo"

FIGSIZE = (14, 7)
BAR_OPACITY = 0.85
MIN_HEIGHT = 0.001
LABEL_FONT_SIZE = 8
LABEL_COLOR = "white"
LABEL_WEIGHT = "bold"
HATCH_CONCAVE = None

WHICH = ["Polylla", "Cavidad"]  # dataset names for legend

# ============================================================
#                  READ + NORMALIZE CSVs
# ============================================================

dfs = []
for path in CSV_PATHS:
    df = pd.read_csv(path)
    if DECIMAL_COMMA:
        df = df.map(
            lambda x: float(str(x).replace(",", ".")) if isinstance(x, str) and "," in x else x
        )
    dfs.append(df)

# ============================================================
#      Extract all e{i} columns across all CSVs
# ============================================================

pattern = re.compile(r"e(\d+)$")
all_indices = set()

for df in dfs:
    for col in df.columns:
        m = pattern.match(col)
        if m:
            all_indices.add(int(m.group(1)))

e_indices = sorted(all_indices)

# ============================================================
#      Build data structure:
#      data[dataset_index][num_points][edge_index] = (convex, concave)
# ============================================================

data = []

for df in dfs:
    dataset = {}
    for npv in NUM_POINTS_LIST:
        row = df[df["num_points"] == npv].iloc[0]
        dataset[npv] = {}
        for i in e_indices:
            col_cv = f"convex_e{i}"
            col_cc = f"concave_e{i}"

            if col_cv in df.columns and col_cc in df.columns:
                cv = float(row[col_cv])
                cc = float(row[col_cc])
            else:
                cv = 0.0
                cc = 0.0

            dataset[npv][i] = (cv, cc)
    data.append(dataset)

# ============================================================
#    Filter e{i} that are zero for all datasets
# ============================================================

filtered_indices = []
for i in e_indices:
    show = False
    for dataset in data:
        for npv in NUM_POINTS_LIST:
            cv, cc = dataset[npv][i]
            if cv > 0 or cc > 0:
                show = True
                break
        if show:
            break
    if show:
        filtered_indices.append(i)

x_labels = [str(i) for i in filtered_indices]
x_positions = np.arange(len(filtered_indices))

# ============================================================
#     Colors (Paired colormap)
# ============================================================

cmap = plt.get_cmap("tab20")

# For each dataset: two colors (convex, concave)
np_colors = {
    ds: [
        cmap((2 * ds + COLOR_OFFSET) % 12),
        cmap((2 * ds + 1 + COLOR_OFFSET) % 12)
    ]
    for ds in range(len(CSV_PATHS))
}

# ============================================================
#               PLOT FIGURE
# ============================================================

fig, ax = plt.subplots(figsize=FIGSIZE)

num_datasets = len(data)
bar_width = 0.35    # each dataset gets this width space per group

dataset_handles = []
convex_handle = None
concave_handle = None

for ds_index, dataset in enumerate(data):

    col_convex = np_colors[ds_index][0]
    col_concave = np_colors[ds_index][1]

    # legend for dataset (color meaning)
    dataset_handles.append(
        mpatches.Patch(facecolor=col_convex, edgecolor='black', label=WHICH[ds_index])
    )

    # offset: dataset 0 is left, dataset 1 is right
    offset = (ds_index - (num_datasets - 1) / 2) * bar_width

    # for each num_points selected
    for npv in NUM_POINTS_LIST:

        convex_vals = [dataset[npv][i][0] for i in filtered_indices]
        concave_vals = [dataset[npv][i][1] for i in filtered_indices]

        # drawing height (minimum height so labels appear)
        convex_draw = [v if v > 0 else 0 for v in convex_vals]
        concave_draw = [v if v > 0 else 0 for v in concave_vals]

        # x positions for this dataset
        xpos = x_positions + offset

        # --- Convex bars ---
        bars_convex = ax.bar(
            xpos,
            convex_draw,
            width=bar_width,
            color=col_convex,
            alpha=BAR_OPACITY,
            edgecolor="black",
            label=CONVEX_LABEL if convex_handle is None else None
        )

        # --- Concave bars (stacked) ---
        bars_concave = ax.bar(
            xpos,
            concave_draw,
            bottom=convex_draw,
            width=bar_width,
            color=col_concave,
            alpha=BAR_OPACITY,
            edgecolor="black",
            hatch=HATCH_CONCAVE,
            label=CONCAVE_LABEL if concave_handle is None else None
        )

        if convex_handle is None:
            convex_handle = mpatches.Patch(facecolor=col_convex, edgecolor="black", label=CONVEX_LABEL)

        if concave_handle is None:
            concave_handle = mpatches.Patch(facecolor=col_concave, edgecolor="black",
                                            label=CONCAVE_LABEL, hatch=HATCH_CONCAVE)

        # --- Labels inside bars ---
        # for bar, value in zip(bars_convex, convex_vals):
        #     if value > 0:
        #         ax.text(
        #             bar.get_x() + bar.get_width()/2,
        #             bar.get_y() + bar.get_height()/2,
        #             str((value)),
        #             ha="center", va="center",
        #             color=LABEL_COLOR, fontsize=LABEL_FONT_SIZE, fontweight=LABEL_WEIGHT
        #         )

        # for bar, value in zip(bars_concave, concave_vals):
        #     if value > 0:
        #         ax.text(
        #             bar.get_x() + bar.get_width()/2,
        #             bar.get_y() + bar.get_height()/2,
        #             str((value)),
        #             ha="center", va="center",
        #             color=LABEL_COLOR, fontsize=LABEL_FONT_SIZE, fontweight=LABEL_WEIGHT
        #         )

# ============================================================
#                    LEGENDS + LABELS
# ============================================================

ax.set_xticks(x_positions)
ax.set_xticklabels(x_labels, rotation=X_LABEL_ROTATION)
ax.set_ylabel(Y_LABEL)
ax.set_title(TITLE)

# Legend 1: Bar type (Convex / Concave)
leg1 = ax.legend(
    handles=[convex_handle, concave_handle],
    title="Tipo de polígono",
    loc="upper right",
    bbox_to_anchor=(1.0, 1.0),
    borderpad=0.6
)
ax.add_artist(leg1)

# Legend 2: Dataset color groups
ax.legend(
    handles=dataset_handles,
    title="Algoritmo",
    loc="upper right",
    bbox_to_anchor=(1.0, 0.88),
    borderpad=0.6
)

plt.tight_layout()
plt.show()
