#!/usr/bin/env bash

# -----------------------------------------------
# Usage:
#   ./count_edges_for_all_combinations.sh --seed 123 --skip-exec
#
# Options:
#   --seed N          Required. Input seed used by Python.
#   --skip-exec       (Optional) Skip running external executable.
#   --random-seed N   (Optional) Default: 42
#   --threshold X     (Optional) Default: 0.5
# -----------------------------------------------

# ---- Default values ----
INPUT_SEED=""
SKIP_EXEC_FLAG=""
RANDOM_SEED=42
THRESHOLD=0.5

# ---- Parse arguments ----
while [[ $# -gt 0 ]]; do
    case "$1" in
        --seed)
            INPUT_SEED="$2"
            shift 2
            ;;
        --skip-exec)
            SKIP_EXEC_FLAG="--skip-exec"
            shift
            ;;
        --random-seed)
            RANDOM_SEED="$2"
            shift 2
            ;;
        --threshold)
            THRESHOLD="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [[ -z "$INPUT_SEED" ]]; then
    echo "ERROR: --seed is required"
    exit 1
fi


# ---- Option sets ----
refiners=("delaunay_cavity_refiner" "polylla_refiner")
mesh_types=("half_edge_mesh")
comparators=("null_comparator" "edge_length_comparator" "angle_comparator" "area_comparator" "random_comparator")
merging_strategies=("exclude_previous_cavities_strategy" "merge_triangles_with_best_convexity_strategy" "merge_triangles_into_smallest_neighbor" "merge_triangles_into_biggest_neighbor")
criteria=("null_refinement_criterion" "min_angle_criterion_robust" "min_angle_criterion" "min_area_criterion" "min_area2_criterion")


# ---- Main loops ----
for refiner in "${refiners[@]}"; do
    for mesh in "${mesh_types[@]}"; do
        for comp in "${comparators[@]}"; do
            for merge in "${merging_strategies[@]}"; do
                for crit in "${criteria[@]}"; do

                    #
                    # ---------------------------------------------
                    # CASE 1: null_comparator
                    # ---------------------------------------------
                    #
                    if [[ "$comp" == "null_comparator" ]]; then

                        if [[ "$crit" == "null_refinement_criterion" ]]; then
                            python3 count_edges.py \
                                --refiner "$refiner" \
                                --mesh "$mesh" \
                                --comparator "$comp" \
                                --merging "$merge" \
                                --criterion "$crit" \
                                --start 10 --end 1000000 \
                                --seed "$INPUT_SEED" \
                                $SKIP_EXEC_FLAG
                        else
                            python3 count_edges.py \
                                --refiner "$refiner" \
                                --mesh "$mesh" \
                                --comparator "$comp" \
                                --merging "$merge" \
                                --criterion "$crit" \
                                --threshold "$THRESHOLD" \
                                --start 10 --end 1000000 \
                                --seed "$INPUT_SEED" \
                                $SKIP_EXEC_FLAG
                        fi
                        continue
                    fi


                    #
                    # ---------------------------------------------
                    # CASE 2: random_comparator
                    # ---------------------------------------------
                    #
                    if [[ "$comp" == "random_comparator" ]]; then

                        if [[ "$crit" == "null_refinement_criterion" ]]; then
                            python3 count_edges.py \
                                --refiner "$refiner" \
                                --mesh "$mesh" \
                                --comparator "$comp" \
                                --merging "$merge" \
                                --criterion "$crit" \
                                --random-seed "$RANDOM_SEED" \
                                --start 10 --end 1000000 \
                                --seed "$INPUT_SEED" \
                                $SKIP_EXEC_FLAG
                        else
                            python3 count_edges.py \
                                --refiner "$refiner" \
                                --mesh "$mesh" \
                                --comparator "$comp" \
                                --merging "$merge" \
                                --criterion "$crit" \
                                --random-seed "$RANDOM_SEED" \
                                --threshold "$THRESHOLD" \
                                --start 10 --end 1000000 \
                                --seed "$INPUT_SEED" \
                                $SKIP_EXEC_FLAG
                        fi
                        continue
                    fi


                    #
                    # ---------------------------------------------
                    # CASE 3: other comparators (need order + key)
                    # ---------------------------------------------
                    #
                    case "$comp" in
                        edge_length_comparator)
                            keys=("min_edge" "max_edge")
                            ;;
                        angle_comparator)
                            keys=("min_angle" "max_angle")
                            ;;
                        area_comparator)
                            keys=("doubled" "precise")
                            ;;
                    esac

                    for order in "ascending" "descending"; do
                        for key in "${keys[@]}"; do

                            if [[ "$crit" == "null_refinement_criterion" ]]; then
                                python3 count_edges.py \
                                    --refiner "$refiner" \
                                    --mesh "$mesh" \
                                    --comparator "$comp" \
                                    --merging "$merge" \
                                    --criterion "$crit" \
                                    --order "$order" \
                                    --key "$key" \
                                    --start 10 --end 1000000 \
                                    --seed "$INPUT_SEED" \
                                    $SKIP_EXEC_FLAG
                            else
                                python3 count_edges.py \
                                    --refiner "$refiner" \
                                    --mesh "$mesh" \
                                    --comparator "$comp" \
                                    --merging "$merge" \
                                    --criterion "$crit" \
                                    --order "$order" \
                                    --key "$key" \
                                    --threshold "$THRESHOLD" \
                                    --start 10 --end 1000000 \
                                    --seed "$INPUT_SEED" \
                                    $SKIP_EXEC_FLAG
                            fi

                        done
                    done

                done
            done
        done
    done
done
