import pandas as pd
import glob
import re

def main():
    # Get all CSV files in the current directory
    files = glob.glob("*.csv")
    if not files:
        print("No CSV files found in the current directory.")
        return

    dataframes = [pd.read_csv(f) for f in files]

    # Step 1: Start output with num_points column from the first CSV
    output = pd.DataFrame()
    output["num_points"] = dataframes[0]["num_points"]

    # Step 2: Average simple columns
    simple_avg_cols = [
        "convex_total", "concave_total",
        "total_polygons", "min_angle", "max_angle"
    ]

    for col in simple_avg_cols:
        output[col] = sum(df.get(col, 0) for df in dataframes) / len(dataframes)

    # Step 3: Handle patterned columns e{i}, convex_e{i}, concave_e{i}
    pattern = re.compile(r"^(convex_|concave_)?e(\d+)$")

    # Find all matching columns across all CSVs
    all_columns = set()
    for df in dataframes:
        for col in df.columns:
            if pattern.match(col):
                all_columns.add(col)

    # Sort columns by integer i so ordering is clean
    sorted_cols = sorted(
        all_columns,
        key=lambda x: int(pattern.match(x).group(2))
    )

    # Compute averaged values
    for col in sorted_cols:
        col_sum = sum(df[col] if col in df else 0 for df in dataframes)
        output[col] = col_sum / len(dataframes)

    # Step 4: Compute convex_pct_e{i} and concave_pct_e{i}
    seen_i = sorted({int(pattern.match(col).group(2)) for col in all_columns})

    for i in seen_i:
        e_col = f"e{i}"
        convex_col = f"convex_e{i}"
        concave_col = f"concave_e{i}"

        e_val = output.get(e_col, pd.Series([0]*len(output)))
        convex_val = output.get(convex_col, pd.Series([0]*len(output)))
        concave_val = output.get(concave_col, pd.Series([0]*len(output)))

        # Avoid pd.NA by using numpy NaN
        safe_e = e_val.replace(0, float("nan"))

        pct_convex = (convex_val / safe_e).fillna(0.0).astype(float)
        pct_concave = (concave_val / safe_e).fillna(0.0).astype(float)

        output[f"convex_pct_e{i}"] = pct_convex
        output[f"concave_pct_e{i}"] = pct_concave



    # Step 5: Write final CSV
    output.to_csv("averaged_output.csv", index=False)
    print("Averaged file has been written to 'averaged_output.csv'.")

if __name__ == "__main__":
    main()
