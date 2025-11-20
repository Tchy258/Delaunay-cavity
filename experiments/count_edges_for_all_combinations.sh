#!/usr/bin/env bash

SEED=42            # Any positive integer for random_comparator
CRITERION_ARG=0.5  # Any double for refinement criteria

input_seed=$1

refiners=("delaunay_cavity_refiner")
mesh_types=("half_edge_mesh")
comparators=("null_comparator" "edge_length_comparator" "random_comparator")
merging_strategies=("exclude_previous_cavities_strategy" "merge_triangles_with_best_convexity" "merge_triangles_into_smallest_neighbor" "merge_triangles_into_biggest_neighbor")
criteria=("null_refinement_criterion")

for refiner in "${refiners[@]}"; do
  for mesh in "${mesh_types[@]}"; do
    for comp in "${comparators[@]}"; do
      for merge in "${merging_strategies[@]}"; do
        for crit in "${criteria[@]}"; do

          # Skip invalid combinations
          if [[ "$comp" == "null_comparator" ]]; then
            # No ascending_or_seed, no sort_key
            if [[ "$crit" == "null_refinement_criterion" ]]; then
              python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 $1
            else
              python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 "" "" "$CRITERION_ARG" $1
            fi

          elif [[ "$comp" == "random_comparator" ]]; then
            # Use SEED, no sort_key
            seed_value=$SEED
            if [[ "$crit" == "null_refinement_criterion" ]]; then
              python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 "$seed_value" $1
            else
              python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 "$seed_value" "" "$CRITERION_ARG" $1
            fi

          else
            # ascending_or_seed must be ascending/descending
            for order in "ascending" "descending"; do
              if [[ "$comp" == "edge_length_comparator" ]]; then
                keys=("min_edge" "max_edge")
              elif [[ "$comp" == "angle_comparator" ]]; then
                keys=("min_angle" "max_angle")
              elif [[ "$comp" == "area_comparator" ]]; then
                keys=("doubled" "precise")
              fi

              for key in "${keys[@]}"; do
                if [[ "$crit" == "null_refinement_criterion" ]]; then
                  python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 "$order" "$key" $1
                else
                  python3 count_edges.py "$refiner" "$mesh" "$comp" "$merge" "$crit" 10 1000000 "$order" "$key" "$CRITERION_ARG" $1
                fi
              done
            done
          fi

        done
      done
    done
  done
done
