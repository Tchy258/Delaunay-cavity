#!/usr/bin/env python3
import os
import subprocess
import itertools
import re

# -------------------------------
# Macro options
# -------------------------------
MESH_REFINER_OPTIONS = ["DelaunayCavityRefiner", "PolyllaRefiner"]
MESH_TYPE_OPTIONS = ["HalfEdgeMesh"]
TRIANGLE_COMPARATOR_BASE_T_OPTIONS = ["NullComparator", "EdgeLengthComparator", "AngleComparator", "AreaComparator", "RandomComparator"]
COMPARATOR_SORT_KEYS = {
    "EdgeLengthComparator": ["MinEdge","MaxEdge"],
    "AngleComparator": ["MaxAngle","MinAngle"],
    "AreaComparator": ["Area2","Precise"],
}
SORT_ORDER_OPTIONS = ["Ascending", "Descending"]
MERGING_STRATEGY_T_OPTIONS = ["ExcludePreviousCavitiesStrategy"]
REFINEMENT_CRITERION_T_OPTIONS = ["NullRefinementCriterion", "MinAngleCriterionRobust", "MinAngleCriterion", "MinAreaCriterion", "MinArea2Criterion"]

MAIN_FILE = "main2.cpp"  # Can be changed per executable
BUILD_DIR = "build"
os.makedirs(BUILD_DIR, exist_ok=True)

# -------------------------------
# Helper functions
# -------------------------------
def choose_option(name, options):
    print(f"\nChoose {name} (or type 'all' to build all combinations):")
    for i, opt in enumerate(options):
        print(f"{i}: {opt}")
    val = input("Enter index or 'all': ").strip()
    if val.lower() == "all":
        return "all"
    return options[int(val)]

def choose_sort_key(name):
    print(f"\nChoose {name} (or type 'all' to build all combinations):")
    for i, opt in enumerate(COMPARATOR_SORT_KEYS[name]):
        print(f"{i}: {opt}")
    val = input("Enter index or 'all': ").strip()
    if val.lower() == "all":
        return "all"
    return COMPARATOR_SORT_KEYS[name][int(val)]

def to_snake_case(name: str) -> str:
    # Convert CamelCase to snake_case
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def build_target(refiner, mesh, tri_base, sort_order_ascending, sort_key_val, merge, refinement_criterion):
    # Use hyphens between options and snake_case
    parts = [refiner,mesh, tri_base, merge, refinement_criterion]
    parts_snake = [to_snake_case(p) for p in parts]
    target_name = "-".join(parts_snake)
    print(f"\nBuilding target: {target_name}")

    cmake_cmd = [
        "cmake",
        "-DBUILD_MACRO_COMBO=ON",
        f"-DREFINER_T={refiner}",
        f"-DMESH_TYPE={mesh}",
        f"-DTRIANGLE_COMPARATOR_BASE_T={tri_base}",
        f"-DMERGING_STRATEGY_T={merge}",
        f"-DREFINEMENT_CRITERION_T={refinement_criterion}",
    ]
    if refiner == "DelaunayCavityRefiner":
        if tri_base != "NullComparator" and tri_base != "RandomComparator":
            cmake_cmd.append(f"-DTRIANGLE_COMPARATOR_ASCENDING={"true" if sort_order_ascending else "false"}")
            cmake_cmd.append(f"-DTRIANGLE_COMPARATOR_SORT_PARAM_2={"true" if sort_key_val else "false"}")    
            cmake_cmd.append("-DTRIANGLE_COMPARATOR_WITH_ARGS=1")
            cmake_cmd.append("-DDELAUNAY_REFINER=1")
        if refinement_criterion != "NullRefinementCriterion":
            cmake_cmd.append("-DREFINEMENT_CRITERION_WITH_ARG=1")
    cmake_cmd.append("..")
    subprocess.run(cmake_cmd, cwd=BUILD_DIR, check=True)
    subprocess.run(["cmake", "--build", ".", "--target", target_name], cwd=BUILD_DIR, check=True)

# -------------------------------
# Interactive selection
# -------------------------------
refiner = choose_option("REFINER_T", MESH_REFINER_OPTIONS)
mesh = choose_option("MESH_TYPE", MESH_TYPE_OPTIONS)
tri_base = choose_option("TRIANGLE_COMPARATOR_BASE_T", TRIANGLE_COMPARATOR_BASE_T_OPTIONS)
ascending = None
ascending_val = None
sort_key = None
sort_key_val = None
if tri_base != "NullComparator" and tri_base != "RandomComparator":
    ascending = choose_option("SORT_ORDER", SORT_ORDER_OPTIONS)
    if ascending == "Ascending":
        ascending_val = True
    else:
        ascending_val = False
    sort_key = choose_sort_key("SORT_KEY")
    if sort_key == COMPARATOR_SORT_KEYS[tri_base][0]:
        sort_key_val = False
    else:
        sort_key_val = True
merge = choose_option("MERGING_STRATEGY_T", MERGING_STRATEGY_T_OPTIONS)
refinement_criterion = choose_option("REFINEMENT_CRITERION_T", REFINEMENT_CRITERION_T_OPTIONS)
# -------------------------------
# Determine build mode
# -------------------------------
if "all" in [refiner, mesh, tri_base, ascending_val, sort_key_val, merge, refinement_criterion]:
    # Build all combinations
    combinations = list(itertools.product(
        MESH_REFINER_OPTIONS,
        MESH_TYPE_OPTIONS,
        TRIANGLE_COMPARATOR_BASE_T_OPTIONS,
        MERGING_STRATEGY_T_OPTIONS,
        REFINEMENT_CRITERION_T_OPTIONS
    ))
    print(f"\nBuilding all {len(combinations)} combinations...")
    for combo in combinations:
        if combo[2] != "NullComparator" and combo[2] != "RandomComparator":
            build_target(*combo, sort_order_ascending=True, sort_key_val=True)
            build_target(*combo, sort_order_ascending=False, sort_key_val=True)
            build_target(*combo, sort_order_ascending=True, sort_key_val=False)
            build_target(*combo, sort_order_ascending=False, sort_key_val=False)
else:
    # Build single selected combination
    build_target(refiner, mesh, tri_base, ascending_val, sort_key_val, merge, refinement_criterion)

print("\nBuild process complete!")
